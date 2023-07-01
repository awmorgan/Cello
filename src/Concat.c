#include "Cello.h"

static const char *Concat_Name(void) { return "Concat"; }

static const char *Concat_Brief(void) { return "Concatenate Objects"; }

static const char *Concat_Description(void) {
  return "The `Concat` class is implemented by objects that can have other "
         "objects "
         "either _appended_ to their, on _concatenated_ to them. For example "
         "collections or strings.";
}

static const char *Concat_Definition(void) {
  return "struct Concat {\n"
         "  void (*concat)(var, var);\n"
         "  void (*append)(var, var);\n"
         "};\n";
}

var Concat = Cello(Concat, Instance(Doc, Concat_Name, Concat_Brief,
                                    Concat_Description, Concat_Definition));

void append(var self, var obj) { method(self, Concat, append, obj); }

void concat(var self, var obj) { method(self, Concat, concat, obj); }
