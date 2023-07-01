#include "Cello.h"

var Start = Cello(Start);

void start(var self) { method(self, Start, start); }

void stop(var self) { method(self, Start, stop); }

void join(var self) { method(self, Start, join); }

bool running(var self) { return method(self, Start, running); }

var start_in(var self) {
  struct Start *s = instance(self, Start);
  if (s && s->start) {
    s->start(self);
  }
  return self;
}

var stop_in(var self) {
  struct Start *s = instance(self, Start);
  if (s && s->stop) {
    s->stop(self);
  }
  return NULL;
}
