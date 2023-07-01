#include "Cello.h"

var Assign = Cello(Assign);

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

var Swap = Cello(Swap);

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
