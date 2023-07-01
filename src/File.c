#include "Cello.h"

var Stream = Cello(Stream);

var sopen(var self, var resource, var options) {
  return method(self, Stream, sopen, resource, options);
}

void sclose(var self) { method(self, Stream, sclose); }

void sseek(var self, int64_t pos, int origin) {
  method(self, Stream, sseek, pos, origin);
}

int64_t stell(var self) { return method(self, Stream, stell); }

void sflush(var self) { method(self, Stream, sflush); }

bool seof(var self) { return method(self, Stream, seof); }

size_t sread(var self, void *output, size_t size) {
  return method(self, Stream, sread, output, size);
}

size_t swrite(var self, void *input, size_t size) {
  return method(self, Stream, swrite, input, size);
}

static var File_Open(var self, var filename, var access);
static void File_Close(var self);

static void File_New(var self, var args) {
  struct File *f = self;
  if (len(args) > 0) {
    File_Open(self, get(args, $I(0)), get(args, $I(1)));
  }
}

static void File_Del(var self) {
  struct File *f = self;
  if (f->file != NULL) {
    File_Close(self);
  }
}

static var File_Open(var self, var filename, var access) {
  struct File *f = self;

  if (f->file != NULL) {
    File_Close(self);
  }

  f->file = fopen(c_str(filename), c_str(access));

  if (f->file == NULL) {
    throw(IOError, "Could !open file: %s", filename);
  }

  return self;
}

static void File_Close(var self) {
  struct File *f = self;

  int err = fclose(f->file);
  if (err != 0) {
    throw(IOError, "Failed to close file: %i", $I(err));
  }

  f->file = NULL;
}

static void File_Seek(var self, int64_t pos, int origin) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!seek file - no file open.");
  }

  int err = fseek(f->file, pos, origin);
  if (err != 0) {
    throw(IOError, "Failed to seek in file: %i", $I(err));
  }
}

static int64_t File_Tell(var self) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!tell file - no file open.");
  }

  int64_t i = ftell(f->file);
  if (i == -1) {
    throw(IOError, "Failed to tell file: %i", $I(i));
  }

  return i;
}

static void File_Flush(var self) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!flush file - no file open.");
  }

  int err = fflush(f->file);
  if (err != 0) {
    throw(IOError, "Failed to flush file: %i", $I(err));
  }
}

static bool File_EOF(var self) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!eof file - no file open.");
  }

  return feof(f->file);
}

static size_t File_Read(var self, void *output, size_t size) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!read file - no file open.");
  }

  size_t num = fread(output, size, 1, f->file);
  if (num != 1 && size != 0 && !feof(f->file)) {
    throw(IOError, "Failed to read from file: %i", $I(num));
    return num;
  }

  return num;
}

static size_t File_Write(var self, void *input, size_t size) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!write file - no file open.");
  }

  size_t num = fwrite(input, size, 1, f->file);
  if (num != 1 && size != 0) {
    throw(IOError, "Failed to write to file: %i", $I(num));
  }

  return num;
}

static int File_Format_To(var self, int pos, const char *fmt, va_list va) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!format to file - no file open.");
  }

  return vfprintf(f->file, fmt, va);
}

static int File_Format_From(var self, int pos, const char *fmt, va_list va) {
  struct File *f = self;

  if (f->file == NULL) {
    throw(IOError, "Can!format from file - no file open.");
  }

  return vfscanf(f->file, fmt, va);
}

var File = Cello(File,

                 Instance(New, File_New, File_Del),
                 Instance(Start, NULL, File_Close, NULL),
                 Instance(Stream, File_Open, File_Close, File_Seek, File_Tell,
                          File_Flush, File_EOF, File_Read, File_Write),
                 Instance(Format, File_Format_To, File_Format_From));

static var Process_Open(var self, var filename, var access);
static void Process_Close(var self);

static void Process_New(var self, var args) {
  struct Process *p = self;
  p->proc = NULL;
  Process_Open(self, get(args, $I(0)), get(args, $I(1)));
}

static void Process_Del(var self) {
  struct Process *p = self;
  if (p->proc != NULL) {
    Process_Close(self);
  }
}

static var Process_Open(var self, var filename, var access) {
  struct Process *p = self;

  if (p->proc != NULL) {
    Process_Close(self);
  }

  p->proc = popen(c_str(filename), c_str(access));

  if (p->proc == NULL) {
    throw(IOError, "Could !open process: %s", filename);
  }

  return self;
}

static void Process_Close(var self) {
  struct Process *p = self;

  int err = pclose(p->proc);
  if (err != 0) {
    throw(IOError, "Failed to close process: %i", $I(err));
  }

  p->proc = NULL;
}

static void Process_Seek(var self, int64_t pos, int origin) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!seek process - no process open.");
  }

  int err = fseek(p->proc, pos, origin);
  if (err != 0) {
    throw(IOError, "Failed to seek in process: %i", $I(err));
  }
}

static int64_t Process_Tell(var self) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!tell process - no process open.");
  }

  int64_t i = ftell(p->proc);
  if (i == -1) {
    throw(IOError, "Failed to tell process: %i", $I(i));
  }

  return i;
}

static void Process_Flush(var self) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!flush process - no process open.");
  }

  int err = fflush(p->proc);
  if (err != 0) {
    throw(IOError, "Failed to flush process: %i", $I(err));
  }
}

static bool Process_EOF(var self) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!eof process - no process open.");
  }

  return feof(p->proc);
}

static size_t Process_Read(var self, void *output, size_t size) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!read process - no process open.");
  }

  size_t num = fread(output, size, 1, p->proc);
  if (num != 1 && size != 0 && !feof(p->proc)) {
    throw(IOError, "Failed to read from process: %i", $I(num));
    return num;
  }

  return num;
}

static size_t Process_Write(var self, void *input, size_t size) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!write process - no process open.");
  }

  size_t num = fwrite(input, size, 1, p->proc);
  if (num != 1 && size != 0) {
    throw(IOError, "Failed to write to process: %i", $I(num));
  }

  return num;
}

static int Process_Format_To(var self, int pos, const char *fmt, va_list va) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!format to process - no process open.");
  }

  return vfprintf(p->proc, fmt, va);
}

static int Process_Format_From(var self, int pos, const char *fmt, va_list va) {
  struct Process *p = self;

  if (p->proc == NULL) {
    throw(IOError, "Can!format from process - no process open.");
  }

  return vfscanf(p->proc, fmt, va);
}

var Process = Cello(Process,

                    Instance(New, Process_New, Process_Del),
                    Instance(Start, NULL, Process_Close, NULL),
                    Instance(Stream, Process_Open, Process_Close, Process_Seek,
                             Process_Tell, Process_Flush, Process_EOF,
                             Process_Read, Process_Write),
                    Instance(Format, Process_Format_To, Process_Format_From));
