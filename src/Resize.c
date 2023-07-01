#include "Cello.h"

static const char *Resize_Name(void) { return "Reserve"; }

static const char *Resize_Brief(void) { return "Object can be resized"; }

static const char *Resize_Description(void) {
  return "The `Resize` class can be implemented by objects which can be "
         "resized in "
         "some way. Resizing to a larger size than the current may allow for "
         "some "
         "resource or other to be preallocated or reserved. For example this "
         "class "
         "is implemented by `Array` and `Table` to either remove a number of "
         "items "
         "quickly or to preallocate memory space if it is known that many "
         "items are "
         "going to be added at a later date.";
}

static const char *Resize_Definition(void) {
  return "struct Resize {\n"
         "  void (*resize)(var, size_t);\n"
         "};\n";
}

var Resize = Cello(Resize, Instance(Doc, Resize_Name, Resize_Brief,
                                    Resize_Description, Resize_Definition));

void resize(var self, size_t n) { method(self, Resize, resize, n); }
