#include "Cello.h"

var C_Int = Cello(C_Int);

var C_Float = Cello(C_Float);

int64_t c_int(var self) {

  if (type_of(self) == Int) {
    return ((struct Int *)self)->val;
  }

  return method(self, C_Int, c_int);
}

double c_float(var self) {

  if (type_of(self) == Float) {
    return ((struct Float *)self)->val;
  }

  return method(self, C_Float, c_float);
}

static void Int_Assign(var self, var obj) {
  struct Int *i = self;
  i->val = c_int(obj);
}

static int64_t Int_C_Int(var self) {
  struct Int *i = self;
  return i->val;
}

static int Int_Cmp(var self, var obj) {
  return (int)(Int_C_Int(self) - c_int(obj));
}

static uint64_t Int_Hash(var self) { return (uint64_t)c_int(self); }

static int Int_Show(var self, var output, int pos) {
  return print_to(output, pos, "%li", self);
}

static int Int_Look(var self, var input, int pos) {
  return scan_from(input, pos, "%li", self);
}

var Int = Cello(Int, Instance(Assign, Int_Assign), Instance(Cmp, Int_Cmp),
                Instance(Hash, Int_Hash), Instance(C_Int, Int_C_Int),
                Instance(Show, Int_Show, Int_Look));

static void Float_Assign(var self, var obj) {
  struct Float *f = self;
  f->val = c_float(obj);
}

static double Float_C_Float(var self) {
  struct Float *f = self;
  return f->val;
}

static int Float_Cmp(var self, var obj) {
  double c = Float_C_Float(self) - c_float(obj);
  return c > 0 ? 1 : c < 0 ? -1 : 0;
}

union interp_cast {
  double as_flt;
  uint64_t as_int;
};

static uint64_t Float_Hash(var self) {
  union interp_cast ic;
  ic.as_flt = c_float(self);
  return ic.as_int;
}

int Float_Show(var self, var output, int pos) {
  return print_to(output, pos, "%f", self);
}

int Float_Look(var self, var input, int pos) {
  return scan_from(input, pos, "%f", self);
}

var Float =
    Cello(Float, Instance(Assign, Float_Assign), Instance(Cmp, Float_Cmp),
          Instance(Hash, Float_Hash), Instance(C_Float, Float_C_Float),
          Instance(Show, Float_Show, Float_Look));
