#include "Cello.h"

static const char *Push_Name(void) { return "Push"; }

static const char *Push_Brief(void) { return "Pushable and Popable object"; }

static const char *Push_Description(void) {
  return ""
         "The `Push` class provides an interface for the addition and removal "
         "of "
         "objects from another in a positional sense."
         "\n\n"
         "`push` can be used to add new objects to a collection and `pop` to "
         "remove "
         "them. Usage of `push` can require `assign` to be defined on the "
         "argument.";
}

static const char *Push_Definition(void) {
  return "struct Push {\n"
         "  void (*push)(var, var);\n"
         "  void (*pop)(var);\n"
         "  void (*push_at)(var, var, var);\n"
         "  void (*pop_at)(var, var);\n"
         "};\n";
}

var Push = Cello(Push, Instance(Doc, Push_Name, Push_Brief, Push_Description,
                                Push_Definition));

void push(var self, var val) { method(self, Push, push, val); }
void push_at(var self, var val, var i) { method(self, Push, push_at, val, i); }
void pop(var self) { method(self, Push, pop); }
void pop_at(var self, var i) { method(self, Push, pop_at, i); }
