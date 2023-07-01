#include "Cello.h"

var Cmp = Cello(Cmp);

int cmp(var self, var obj) {

  struct Cmp *c = instance(self, Cmp);
  if (c && c->cmp) {
    return c->cmp(self, obj);
  }

  size_t s = size(type_of(self));
  if (type_of(self) == type_of(obj) && s) {
    return memcmp(self, obj, s);
  }

  throw(TypeError, "Cannot compare type %s to type %s", type_of(obj),
        type_of(self));

  return 0;
}

bool eq(var self, var obj) { return cmp(self, obj) == 0; }
bool neq(var self, var obj) { return !eq(self, obj); }
bool gt(var self, var obj) { return cmp(self, obj) > 0; }
bool lt(var self, var obj) { return cmp(self, obj) < 0; }
bool ge(var self, var obj) { return !lt(self, obj); }
bool le(var self, var obj) { return !gt(self, obj); }

var Sort = Cello(Sort);

void sort(var self) { method(self, Sort, sort_by, lt); }

void sort_by(var self, bool (*f)(var, var)) { method(self, Sort, sort_by, f); }
