#include "Cello.h"

struct Header *header(var self) {
  return (struct Header *)((char *)self - sizeof(struct Header));
}

var header_init(var head, var type, int alloc) {

  struct Header *self = head;

  self->type = type;

  self->alloc = (var)(intptr_t)alloc;

  self->magic = (var)CELLO_MAGIC_NUM;

  return ((char *)self) + sizeof(struct Header);
}

var Alloc = Cello(Alloc);

enum { ALLOC_STANDARD, ALLOC_RAW, ALLOC_ROOT };

static var alloc_by(var type, int method) {

  struct Alloc *a = type_instance(type, Alloc);
  var self;
  if (a && a->alloc) {
    self = a->alloc();
  } else {
    struct Header *head = calloc(1, sizeof(struct Header) + size(type));

    if (head == NULL) {
      throw(OutOfMemoryError, "Cannot create new '%s', out of memory!", type);
    }

    self = header_init(head, type, AllocHeap);
  }

  switch (method) {
  case ALLOC_STANDARD:
#ifndef CELLO_NGC
    set(current(GC), self, $I(0));
#endif
    break;
  case ALLOC_RAW:
    break;
  case ALLOC_ROOT:
#ifndef CELLO_NGC
    set(current(GC), self, $I(1));
#endif
    break;
  }

  return self;
}

var alloc(var type) { return alloc_by(type, ALLOC_STANDARD); }
var alloc_raw(var type) { return alloc_by(type, ALLOC_RAW); }
var alloc_root(var type) { return alloc_by(type, ALLOC_ROOT); }

void dealloc(var self) {

  struct Alloc *a = instance(self, Alloc);
  if (a && a->dealloc) {
    a->dealloc(self);
    return;
  }

  if (self == NULL) {
    throw(ResourceError, "Attempt to deallocate NULL!");
  }

  if (header(self)->alloc == (var)AllocStatic) {
    throw(ResourceError,
          "Attempt to deallocate %$ "
          "which was allocated statically!",
          self);
  }

  if (header(self)->alloc == (var)AllocStack) {
    throw(ResourceError,
          "Attempt to deallocate %$ "
          "which was allocated on the stack!",
          self);
  }

  if (header(self)->alloc == (var)AllocData) {
    throw(ResourceError,
          "Attempt to deallocate %$ "
          "which was allocated inside a data structure!",
          self);
  }

  size_t s = size(type_of(self));
  for (size_t i = 0; i < (sizeof(struct Header) + s) / sizeof(var); i++) {
    ((var *)header(self))[i] = (var)0xDeadCe110;
  }

  free(((char *)self) - sizeof(struct Header));
}

void dealloc_raw(var self) { dealloc(self); }
void dealloc_root(var self) { dealloc(self); }

var New = Cello(New);

var construct_with(var self, var args) {
  struct New *n = instance(self, New);
  if (n && n->construct_with) {
    n->construct_with(self, args);
  } else if (len(args) == 1) {
    assign(self, get(args, $I(0)));
  }
  return self;
}

var destruct(var self) {
  struct New *n = instance(self, New);
  if (n && n->destruct) {
    n->destruct(self);
  }
  return self;
}

var new_with(var type, var args) { return construct_with(alloc(type), args); }

var new_raw_with(var type, var args) {
  return construct_with(alloc_raw(type), args);
}

var new_root_with(var type, var args) {
  return construct_with(alloc_root(type), args);
}

static void del_by(var self, int method) {

  switch (method) {
  case ALLOC_STANDARD:
  case ALLOC_ROOT:
#ifndef CELLO_NGC
    rem(current(GC), self);
    return;
#endif
    break;
  case ALLOC_RAW:
    break;
  }

  dealloc(destruct(self));
}

void del(var self) { del_by(self, ALLOC_STANDARD); }
void del_raw(var self) { del_by(self, ALLOC_RAW); }
void del_root(var self) { del_by(self, ALLOC_ROOT); }

var Copy = Cello(Copy);

var copy(var self) {

  struct Copy *c = instance(self, Copy);
  if (c && c->copy) {
    return c->copy(self);
  }

  return assign(alloc(type_of(self)), self);
}
