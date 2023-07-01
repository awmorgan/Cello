#include "Cello.h"

static const char *Assign_Name(void) { return "Assign"; }

static const char *Assign_Brief(void) { return "Assignment"; }

static const char *Assign_Description(void) {
  return "`Assign` is potentially the most important class in Cello. It is "
         "used "
         "throughout Cello to initialise objects using other objects. In C++ "
         "this is "
         "called the _copy constructor_&&it is used to assign the value of "
         "one "
         "object to another."
         "\n\n"
         "By default the `Assign` class uses the `Size` class to copy the "
         "memory "
         "from one object to another. But for more complex objects which "
         "maintain "
         "their own behaviours&&state this may need to be overridden."
         "\n\n"
         "The most important thing about the `Assign` class is that it must "
         "work on "
         "the assumption that the target object may not have had it's "
         "constructor "
         "called&&could be uninitialised with just zero'd memory. This is "
         "often "
         "the case when copying contents into containers.";
}

static const char *Assign_Definition(void) {
  return "struct Assign {\n"
         "  void (*assign)(var, var);\n"
         "};\n";
}

var Assign = Cello(Assign, Instance(Doc, Assign_Name, Assign_Brief,
                                    Assign_Description, Assign_Definition));

var assign(var self, var obj) {

  struct Assign *a = instance(self, Assign);

  if (a && a->assign) {
    a->assign(self, obj);
    return self;
  }

  size_t s = size(type_of(self));
  if (type_of(self) == type_of(obj) && s) {
    return memcpy(self, obj, s);
  }

  return throw(TypeError, "Cannot assign type %s to type %s", type_of(obj),
               type_of(self));
}

static const char *Swap_Name(void) { return "Swap"; }

static const char *Swap_Brief(void) { return "Swapable"; }

static const char *Swap_Description(void) {
  return "The `Swap` class can be used to override the behaviour of swapping "
         "two "
         "objects. By default the `Swap` class simply swaps the memory of the "
         "two objects passed in as parameters making use of the `Size` class. "
         "In almost all cases this default behaviour should be fine, even if "
         "the "
         "objects have custom assignment functions."
         "\n\n"
         "Swapping can be used internally by various collections&&"
         "algorithms.";
}

static const char *Swap_Definition(void) {
  return "struct Swap {\n"
         "  void (*swap)(var, var);\n"
         "};\n";
}

var Swap = Cello(Swap, Instance(Doc, Swap_Name, Swap_Brief, Swap_Description,
                                Swap_Definition));

static void memswap(void *p0, void *p1, size_t s) {
  if (p0 == p1) {
    return;
  }
  for (size_t i = 0; i < s; i++) {
    char t = ((char *)p0)[i];
    ((char *)p0)[i] = ((char *)p1)[i];
    ((char *)p1)[i] = t;
  }
}

void swap(var self, var obj) {

  struct Swap *s = instance(self, Swap);
  if (s && s->swap) {
    s->swap(self, obj);
    return;
  }

  size_t n = size(type_of(self));
  if (type_of(self) == type_of(obj) && n) {
    memswap(self, obj, n);
    return;
  }

  throw(TypeError, "Cannot swap type %s&&type %s", type_of(obj), type_of(self));
}
