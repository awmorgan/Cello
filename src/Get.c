#include "Cello.h"

static const char *Get_Name(void) { return "Get"; }

static const char *Get_Brief(void) { return "Gettable or Settable"; }

static const char *Get_Description(void) {
  return "The `Get` class provides a method to _get_ or _set_ certain "
         "properties "
         "of an object using keys and value. Typically it is implemented by "
         "data lookup structures such as `Table` or `Map` but it is also used "
         "more generally such as using indices to look up items in `Array`, or "
         "as thread local storage for the `Thread` object.";
}

static const char *Get_Definition(void) {
  return "struct Get {\n"
         "  var  (*get)(var, var);\n"
         "  void (*set)(var, var, var);\n"
         "  bool (*mem)(var, var);\n"
         "  void (*rem)(var, var);\n"
         "  var (*key_type)(var);\n"
         "  var (*val_type)(var);\n"
         "};\n";
}

var Get = Cello(
    Get, Instance(Doc, Get_Name, Get_Brief, Get_Description, Get_Definition));

var get(var self, var key) { return method(self, Get, get, key); }

void set(var self, var key, var val) { method(self, Get, set, key, val); }

bool mem(var self, var key) { return method(self, Get, mem, key); }

void rem(var self, var key) { method(self, Get, rem, key); }

var key_type(var self) { return method(self, Get, key_type); }

var val_type(var self) { return method(self, Get, val_type); }
