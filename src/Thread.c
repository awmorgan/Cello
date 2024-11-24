#include "Cello.h"

var Current = Cello(Current);

var current(var type) { return type_method(type, Current, current); }

struct Thread {

  var func;
  var args;
  var tls;

  bool is_main;
  bool is_running;

#if defined(CELLO_UNIX)
  pthread_t thread;
#elif defined(CELLO_WINDOWS)
  DWORD id;
  HANDLE thread;
#endif
};

static void Thread_New(var self, var args) {
  struct Thread *t = self;
  t->func = empty(args) ? NULL : get(args, $I(0));
  t->args = NULL;
  t->is_main = false;
  t->is_running = false;
  t->tls = new_raw(Table, String, Ref);
}

static void Thread_Del(var self) {
  struct Thread *t = self;

#ifdef CELLO_WINDOWS
  CloseHandle(t->thread);
#endif

  if (t->args != NULL) {
    del_raw(t->args);
  }
  del_raw(t->tls);
}

static int64_t Thread_C_Int(var self) {
  struct Thread *t = self;

  if (!t->is_running) {
    throw(ValueError, "Can!get thread ID, thread !running!");
  }

#if defined(CELLO_UNIX)
  return (int64_t)t->thread;
#elif defined(CELLO_WINDOWS)
  return (int64_t)t->id;
#else
  return 0;
#endif
}

static void Thread_Assign(var self, var obj) {
  struct Thread *t = self;
  struct Thread *o = cast(obj, Thread);
  t->func = o->func;
  t->tls = t->tls ? t->tls : alloc_raw(Table);
  assign(t->tls, o->tls);
}

static int Thread_Cmp(var self, var obj) {
  return (int)(Thread_C_Int(self) - c_int(obj));
}

static uint64_t Thread_Hash(var self) { return Thread_C_Int(self); }

static bool Thread_TLS_Key_Created = false;

#if defined(CELLO_UNIX)

static pthread_key_t Thread_Key_Wrapper;

static void Thread_TLS_Key_Create(void) {
  pthread_key_create(&Thread_Key_Wrapper, NULL);
}
static void Thread_TLS_Key_Delete(void) {
  pthread_key_delete(Thread_Key_Wrapper);
}

static var Thread_Init_Run(var self) {

  struct Thread *t = self;
  pthread_setspecific(Thread_Key_Wrapper, t);
  t->is_running = true;

#ifndef CELLO_NGC
  var bottom = NULL;
  var gc = new_raw(GC, $R(&bottom));
#endif

  var exc = new_raw(Exception);

  var x = call_with(t->func, t->args);
  del_raw(t->args);
  t->args = NULL;

  del_raw(exc);

#ifndef CELLO_NGC
  del_raw(gc);
#endif

  return x;
}

#elif defined(CELLO_WINDOWS)

static DWORD Thread_Key_Wrapper;

static void Thread_TLS_Key_Create(void) { Thread_Key_Wrapper = TlsAlloc(); }
static void Thread_TLS_Key_Delete(void) { TlsFree(Thread_Key_Wrapper); }

static DWORD Thread_Init_Run(var self) {

  struct Thread *t = self;
  TlsSetValue(Thread_Key_Wrapper, t);
  t->is_running = true;

  var ex = new_raw(Exception);

#ifndef CELLO_NGC
  var bottom = NULL;
  var gc = new_raw(GC, $R(&bottom));
#endif

  call_with(t->func, t->args);
  del_raw(t->args);
  t->args = NULL;

#ifndef CELLO_NGC
  del_raw(gc);
#endif

  del_raw(ex);

  return 0;
}

#endif

static var Thread_Call(var self, var args) {

  struct Thread *t = self;

  t->args = assign(alloc_raw(type_of(args)), args);

  /* Call Init Thread & Run */

#if defined(CELLO_UNIX)

  /* Setup Thread Local Storage */

  if (!Thread_TLS_Key_Created) {
    Thread_TLS_Key_Create();
    Thread_TLS_Key_Created = true;
    atexit(Thread_TLS_Key_Delete);
  }

  int err = pthread_create(&t->thread, NULL, Thread_Init_Run, t);

  if (err == EINVAL) {
    throw(ValueError, "Invalid Argument to Thread Creation");
  }

  if (err == EAGAIN) {
    throw(OutOfMemoryError, "!enough resources to create another Thread");
  }

  if (err == EBUSY) {
    throw(BusyError, "System==too busy to create thread");
  }

#elif defined(CELLO_WINDOWS)

  /* Setup Thread Local Storage */

  if (!Thread_TLS_Key_Created) {
    Thread_TLS_Key_Create();
    Thread_TLS_Key_Created = true;
    atexit(Thread_TLS_Key_Delete);
  }

  t->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread_Init_Run, t,
                           0, &t->id);

  if (t->thread == NULL) {
    throw(ValueError, "Unable to Create WinThread");
  }

#else

  throw(ResourceError, "Unsupported Threading Environment");

#endif

  return self;
}

static var Thread_Main = NULL;
static var Exception_Main = NULL;

static void Thread_Main_Del(void) {
  del_raw(Exception_Main);
  del_raw(Thread_Main);
}

static var Thread_Current(void) {

  if (!Thread_TLS_Key_Created) {
    Thread_TLS_Key_Create();
    Thread_TLS_Key_Created = true;
    atexit(Thread_TLS_Key_Delete);
  }

#if defined(CELLO_UNIX)
  var wrapper = pthread_getspecific(Thread_Key_Wrapper);
#elif defined(CELLO_WINDOWS)
  var wrapper = TlsGetValue(Thread_Key_Wrapper);
#else
  var wrapper = NULL;
#endif

  /*
  ** Here==a nasty one. On OSX instead of
  ** returning NULL for an unset key it
  ** decides to return uninitialized rubbish
  ** (even though the spec says otherwise).
  **
  ** Luckily we can test directly for the main
  ** thread on OSX using this non-portable method
  */
#ifdef CELLO_MAC
  if (pthread_main_np()) {
    wrapper = NULL;
  }
#endif

  if (wrapper == NULL) {

    if (Thread_Main == NULL) {
      Thread_Main = new_raw(Thread);
      Exception_Main = new_raw(Exception);
      atexit(Thread_Main_Del);
    }

    struct Thread *t = Thread_Main;
    t->is_main = true;
    t->is_running = true;

#if defined(CELLO_UNIX)
    t->thread = pthread_self();
#elif defined(CELLO_WINDOWS)
    t->thread = GetCurrentThread();
#endif

    return Thread_Main;
  }

  return wrapper;
}

static void Thread_Start(var self) { call(self); }

static void Thread_Stop(var self) {
  struct Thread *t = self;

#if defined(CELLO_UNIX)
  if (!t->thread) {
    return;
  }
  int err = pthread_kill(t->thread, SIGINT);
  if (err == EINVAL) {
    throw(ValueError, "Invalid Argument to Thread Stop");
  }
  if (err == ESRCH) {
    throw(ValueError, "Invalid Thread");
  }
#elif defined(CELLO_WINDOWS)
  if (!t->thread) {
    return;
  }
  TerminateThread(t->thread, FALSE);
#endif
}

static void Thread_Join(var self) {
  struct Thread *t = self;

#if defined(CELLO_UNIX)
  if (!t->thread) {
    return;
  }
  int err = pthread_join(t->thread, NULL);
  if (err == EINVAL) {
    throw(ValueError, "Invalid Argument to Thread Join");
  }
  if (err == ESRCH) {
    throw(ValueError, "Invalid Thread");
  }
#elif defined(CELLO_WINDOWS)
  if (!t->thread) {
    return;
  }
  WaitForSingleObject(t->thread, INFINITE);
#endif
}

static bool Thread_Running(var self) {
  struct Thread *t = self;
  return t->is_running;
}

static var Thread_Get(var self, var key) {
  struct Thread *t = self;
  return deref(get(t->tls, key));
}

static void Thread_Set(var self, var key, var val) {
  struct Thread *t = self;
  set(t->tls, key, $R(val));
}

static bool Thread_Mem(var self, var key) {
  struct Thread *t = self;
  return mem(t->tls, key);
}

static void Thread_Rem(var self, var key) {
  struct Thread *t = self;
  rem(t->tls, key);
}

static var Thread_Key_Type(var self) {
  struct Thread *t = self;
  return key_type(t->tls);
}

static var Thread_Val_Type(var self) {
  struct Thread *t = self;
  return val_type(t->tls);
}

static void Thread_Mark(var self, var gc, void (*f)(var, void *)) {
  struct Thread *t = self;
  mark(t->tls, gc, f);
}

var Thread = Cello(
    Thread, Instance(New, Thread_New, Thread_Del),
    Instance(Assign, Thread_Assign), Instance(Cmp, Thread_Cmp),
    Instance(Hash, Thread_Hash), Instance(Call, Thread_Call),
    Instance(Current, Thread_Current), Instance(Mark, Thread_Mark),
    Instance(Start, Thread_Start, Thread_Stop, Thread_Join, Thread_Running),
    Instance(C_Int, Thread_C_Int),
    Instance(Get, Thread_Get, Thread_Set, Thread_Mem, Thread_Rem));

var Lock = Cello(Lock);

void lock(var self) { method(self, Lock, lock); }

void unlock(var self) { method(self, Lock, unlock); }

bool trylock(var self) { return method(self, Lock, trylock); }

struct Mutex {
#if defined(CELLO_UNIX)
  pthread_mutex_t mutex;
#elif defined(CELLO_WINDOWS)
  HANDLE mutex;
#endif
};

static void Mutex_New(var self, var args) {
  struct Mutex *m = self;
#if defined(CELLO_UNIX)
  pthread_mutex_init(&m->mutex, NULL);
#elif defined(CELLO_WINDOWS)
  m->mutex = CreateMutex(NULL, false, NULL);
#endif
}

static void Mutex_Del(var self) {
  struct Mutex *m = self;
#if defined(CELLO_UNIX)
  pthread_mutex_destroy(&m->mutex);
#elif defined(CELLO_WINDOWS)
  CloseHandle(m->mutex);
#endif
}

static void Mutex_Lock(var self) {
  struct Mutex *m = self;
#if defined(CELLO_UNIX)
  int err = pthread_mutex_lock(&m->mutex);

  if (err == EINVAL) {
    throw(ValueError, "Invalid Argument to Mutex Lock");
  }

  if (err == EDEADLK) {
    throw(ResourceError, "Attempt to relock already held mutex");
  }
#elif defined(CELLO_WINDOWS)
  WaitForSingleObject(m->mutex, INFINITE);
#endif
}

static bool Mutex_Trylock(var self) {
  struct Mutex *m = self;
#if defined(CELLO_UNIX)
  int err = pthread_mutex_trylock(&m->mutex);
  if (err == EBUSY) {
    return false;
  }
  if (err == EINVAL) {
    throw(ValueError, "Invalid Argument to Mutex Lock Try");
  }
  return true;
#elif defined(CELLO_WINDOWS)
  return !(WaitForSingleObject(m->mutex, 0) == WAIT_TIMEOUT);
#else
  return true;
#endif
}

static void Mutex_Unlock(var self) {
  struct Mutex *m = cast(self, Mutex);
#if defined(CELLO_UNIX)
  int err = pthread_mutex_unlock(&m->mutex);
  if (err == EINVAL) {
    throw(ValueError, "Invalid Argument to Mutex Unlock");
  }
  if (err == EPERM) {
    throw(ResourceError, "Mutex can!be held by caller");
  }
#elif defined(CELLO_WINDOWS)
  ReleaseMutex(m->mutex);
#endif
}

var Mutex = Cello(Mutex, Instance(New, Mutex_New, Mutex_Del),
                  Instance(Lock, Mutex_Lock, Mutex_Unlock, Mutex_Trylock),
                  Instance(Start, Mutex_Lock, Mutex_Unlock, NULL));
