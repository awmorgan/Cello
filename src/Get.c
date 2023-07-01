#include "Cello.h"

var Get = Cello(Get);

var get(var self, var key) { return method(self, Get, get, key); }

void set(var self, var key, var val) { method(self, Get, set, key, val); }

bool mem(var self, var key) { return method(self, Get, mem, key); }

void rem(var self, var key) { method(self, Get, rem, key); }

var key_type(var self) { return method(self, Get, key_type); }

var val_type(var self) { return method(self, Get, val_type); }
