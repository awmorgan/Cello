#include "Cello.h"

var Push = Cello(Push);

void push(var self, var val) { method(self, Push, push, val); }
void push_at(var self, var val, var i) { method(self, Push, push_at, val, i); }
void pop(var self) { method(self, Push, pop); }
void pop_at(var self, var i) { method(self, Push, pop_at, i); }
