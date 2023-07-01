#include "Cello.h"

static const char *Cmp_Name(void) { return "Cmp"; }

static const char *Cmp_Brief(void) { return "Comparison"; }

static const char *Cmp_Description(void) {
  return "The `Cmp` class is used to define comparison between two object "
         "values. "
         "This class is important as it is used by many data structures to "
         "test "
         "equality or ordering of objects."
         "\n\n"
         "By default, if passed two objects of the same type, the `Cmp` class "
         "will "
         "simply compare the raw memory of both objects, using the `Size` "
         "class."
         "\n\n"
         "To implement this class a `cmp` function must be provided which "
         "returns "
         "`< 0` if the first object is _less than_ the second, `> 0` if the "
         "first "
         "object is _greater than_ the second,&&`0` if they are _equal_. "
         "\n\n"
         "For objects that manage their own data this class may need to be "
         "overridden to ensure that objects of the same _value_ are still "
         "treated "
         "as equal. E.G. for string types."
         "\n\n"
         "This class to used to test for _value_ equality between objects, "
         "I.E. if "
         "they represent the same thing. For _object_ equality the `is` "
         "keyword can "
         "be used, which will return `true` only if two variables are pointing "
         "to "
         "the same object in memory.";
}

static const char *Cmp_Definition(void) {
  return "struct Cmp {\n"
         "  int (*cmp)(var, var);\n"
         "};\n";
}

var Cmp = Cello(
    Cmp, Instance(Doc, Cmp_Name, Cmp_Brief, Cmp_Description, Cmp_Definition));

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

static const char *Sort_Name(void) { return "Sort"; }

static const char *Sort_Brief(void) { return "Sortable"; }

static const char *Sort_Description(void) {
  return "The `Sort` class can be implemented by types which can be sorted in "
         "some "
         "way such as `Array`. By default the sorting function uses the `lt` "
         "method "
         "to compare elements, but a custom function can also be provided.";
}

static const char *Sort_Definition(void) {
  return "struct Sort {\n"
         "  void (*sort_by)(var,bool(*f)(var,var));\n"
         "};";
}

var Sort = Cello(Sort, Instance(Doc, Sort_Name, Sort_Brief, Sort_Description,
                                Sort_Definition));

void sort(var self) { method(self, Sort, sort_by, lt); }

void sort_by(var self, bool (*f)(var, var)) { method(self, Sort, sort_by, f); }
