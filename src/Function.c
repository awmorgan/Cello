#include "Cello.h"

var Call = Cello(Call);

var call_with(var self, var args) {
  return method(self, Call, call_with, args);
}

static var Function_Call(var self, var args) {
  struct Function *f = self;
  return f->func(args);
}

var Function = Cello(Function,

                     Instance(Call, Function_Call));
