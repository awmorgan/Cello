#include "Cello.h"

var Len = Cello(Len);

size_t len(var self) { return method(self, Len, len); }

bool empty(var self) { return len(self) == 0; }
