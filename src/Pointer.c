#include "Cello.h"

var Pointer = Cello(Pointer);

void ref(var self, var item) { method(self, Pointer, ref, item); }

var deref(var self) { return method(self, Pointer, deref); }

static void Ref_Ref(var self, var val);
static var Ref_Deref(var self);
static void Ref_Assign(var self, var obj);

static void Ref_Assign(var self, var obj) {
  struct Pointer *p = instance(obj, Pointer);
  if (p && p->deref) {
    Ref_Ref(self, p->deref(obj));
  } else {
    Ref_Ref(self, obj);
  }
}

static void Ref_Ref(var self, var val) {
  struct Ref *r = self;
  r->val = val;
}

static var Ref_Deref(var self) {
  struct Ref *r = self;
  return r->val;
}

var Ref = Cello(Ref, Instance(Assign, Ref_Assign),
                Instance(Pointer, Ref_Ref, Ref_Deref));

static void Box_Ref(var self, var val);
static var Box_Deref(var self);
static void Box_Assign(var self, var obj);

static void Box_New(var self, var args) { Box_Assign(self, get(args, $I(0))); }

static void Box_Del(var self) {
  var obj = Box_Deref(self);
  if (obj) {
    del(obj);
  }
  Box_Ref(self, NULL);
}

static void Box_Assign(var self, var obj) {
  struct Pointer *p = instance(obj, Pointer);
  if (p && p->deref) {
    Box_Ref(self, p->deref(obj));
  } else {
    Box_Ref(self, obj);
  }
}

static int Box_Show(var self, var output, int pos) {
  return print_to(output, pos, "<'Box' at 0x%p (%$)>", self, Box_Deref(self));
}

static void Box_Ref(var self, var val) {
  struct Box *b = self;
  b->val = val;
}

static var Box_Deref(var self) {
  struct Box *b = self;
  return b->val;
}

var Box = Cello(Box, Instance(New, Box_New, Box_Del),
                Instance(Assign, Box_Assign), Instance(Show, Box_Show, NULL),
                Instance(Pointer, Box_Ref, Box_Deref));
