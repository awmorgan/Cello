#include "Cello.h"

static const char *Start_Name(void) { return "Start"; }

static const char *Start_Brief(void) { return "Can be started or stopped"; }

static const char *Start_Description(void) {
  return "The `Start` class can be implemented by types which provide an "
         "abstract "
         "notion of a started && stopped state. This can be real processes "
         "such "
         "as `Thread`, or something like `File` where the on/off correspond to "
         "if the file is open or not."
         "\n\n"
         "The main nicety of the `Start` class is that it allows use of the "
         "`with` "
         "macro which performs the `start` function at the opening of a scope "
         "block "
         "&& the `stop` function at the end.";
}

static const char *Start_Definition(void) {
  return "struct Start {\n"
         "  void (*start)(var);\n"
         "  void (*stop)(var);\n"
         "  void (*join)(var);\n"
         "  bool (*running)(var);\n"
         "};\n";
}

var Start = Cello(Start, Instance(Doc, Start_Name, Start_Brief,
                                  Start_Description, Start_Definition));

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
