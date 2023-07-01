#include "Cello.h"

var Concat = Cello(Concat);

void append(var self, var obj) { method(self, Concat, append, obj); }

void concat(var self, var obj) { method(self, Concat, concat, obj); }
