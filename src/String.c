#include "Cello.h"

var C_Str = Cello(C_Str);

char *c_str(var self) {

  if (type_of(self) == String) {
    return ((struct String *)self)->val;
  }

  return method(self, C_Str, c_str);
}

static void String_Assign(var self, var obj);

static void String_New(var self, var args) {
  struct String *s = self;
  if (len(args) > 0) {
    String_Assign(self, get(args, $I(0)));
  } else {
    s->val = calloc(1, 1);
  }

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }
}

static void String_Del(var self) {
  struct String *s = self;

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot destruct String, not on heap!");
  }

  free(s->val);
}

static void String_Assign(var self, var obj) {
  struct String *s = self;
  char *val = c_str(obj);

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  s->val = realloc(s->val, strlen(val) + 1);

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }

  strcpy(s->val, val);
}

static char *String_C_Str(var self) {
  struct String *s = self;
  return s->val;
}

static int String_Cmp(var self, var obj) {
  return strcmp(String_C_Str(self), c_str(obj));
}

static size_t String_Len(var self) {
  struct String *s = self;
  return strlen(s->val);
}

static void String_Clear(var self) {
  struct String *s = self;

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  s->val = realloc(s->val, 1);

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }

  s->val[0] = '\0';
}

static bool String_Mem(var self, var obj) {

  struct C_Str *c = instance(obj, C_Str);
  if (c && c->c_str) {
    return strstr(String_C_Str(self), c->c_str(obj));
  }

  return false;
}

static void String_Rem(var self, var obj) {

  struct C_Str *c = instance(obj, C_Str);
  if (c && c->c_str) {
    char *pos = strstr(String_C_Str(self), c->c_str(obj));
    size_t count =
        strlen(String_C_Str(self)) - strlen(pos) - strlen(c->c_str(obj)) + 1;
    memmove((char *)pos, pos + strlen(c->c_str(obj)), count);
  }
}

static uint64_t String_Hash(var self) {
  struct String *s = self;
  return hash_data(s->val, strlen(s->val));
}

static void String_Concat(var self, var obj) {
  struct String *s = self;

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  s->val = realloc(s->val, strlen(s->val) + strlen(c_str(obj)) + 1);

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }

  strcat(s->val, c_str(obj));
}

static void String_Resize(var self, size_t n) {
  struct String *s = self;

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  size_t m = String_Len(self);
  s->val = realloc(s->val, n + 1);

  if (n > m) {
    memset(&s->val[m], 0, n - m);
  } else {
    s->val[n] = '\0';
  }

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }
}

static int String_Format_To(var self, int pos, const char *fmt, va_list va) {

  struct String *s = self;

#ifdef CELLO_WINDOWS

  va_list va_tmp;
  va_copy(va_tmp, va);
  int size = _vscprintf(fmt, va_tmp);
  va_end(va_tmp);

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  s->val = realloc(s->val, pos + size + 1);

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }

  return vsprintf(s->val + pos, fmt, va);

#elif defined(CELLO_MAC)

  va_list va_tmp;
  va_copy(va_tmp, va);
  char *tmp = NULL;
  int size = vasprintf(&tmp, fmt, va_tmp);
  va_end(va_tmp);

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  s->val = realloc(s->val, pos + size + 1);

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }

  s->val[pos] = '\0';
  strcat(s->val, tmp);
  free(tmp);

  return size;

#else

  va_list va_tmp;
  va_copy(va_tmp, va);
  int size = vsnprintf(NULL, 0, fmt, va_tmp);
  va_end(va_tmp);

  if (header(self)->alloc == (var)AllocStack ||
      header(self)->alloc == (var)AllocStatic) {
    throw(ValueError, "Cannot reallocate String, not on heap!");
  }

  s->val = realloc(s->val, pos + size + 1);

  if (s->val == NULL) {
    throw(OutOfMemoryError, "Cannot allocate String, out of memory!");
  }

  return vsprintf(s->val + pos, fmt, va);

#endif
}

static int String_Format_From(var self, int pos, const char *fmt, va_list va) {
  struct String *s = self;
  return vsscanf(s->val + pos, fmt, va);
}

static int String_Show(var self, var out, int pos) {
  struct String *s = self;
  pos = print_to(out, pos, "\"", self);
  char *v = s->val;
  while (*v) {
    switch (*v) {
    case '\a':
      pos = print_to(out, pos, "\\a");
      break;
    case '\b':
      pos = print_to(out, pos, "\\b");
      break;
    case '\f':
      pos = print_to(out, pos, "\\f");
      break;
    case '\n':
      pos = print_to(out, pos, "\\n");
      break;
    case '\r':
      pos = print_to(out, pos, "\\r");
      break;
    case '\t':
      pos = print_to(out, pos, "\\t");
      break;
    case '\v':
      pos = print_to(out, pos, "\\v");
      break;
    case '\\':
      pos = print_to(out, pos, "\\\\");
      break;
    case '\'':
      pos = print_to(out, pos, "\\'");
      break;
    case '\"':
      pos = print_to(out, pos, "\\\"");
      break;
    case '\?':
      pos = print_to(out, pos, "\\?");
      break;
    default:
      pos = print_to(out, pos, "%c", $I(*v));
    }
    v++;
  }
  return print_to(out, pos, "\"", self);
}

static int String_Look(var self, var input, int pos) {

  String_Clear(self);

  var chr = $I(0);
  pos = scan_from(input, pos, "%c", chr);

  if (c_int(chr) != '\"') {
    throw(FormatError, "String literal does not start with quotation marks!");
  }

  while (true) {

    pos = scan_from(input, pos, "%c", chr);

    if (c_int(chr) == '"') {
      break;
    }

    if (c_int(chr) == '\\') {
      pos = scan_from(input, pos, "%c", chr);
      switch (c_int(chr)) {
      case 'a':
        String_Concat(self, $S("\a"));
        break;
      case 'b':
        String_Concat(self, $S("\b"));
        break;
      case 'f':
        String_Concat(self, $S("\f"));
        break;
      case 'n':
        String_Concat(self, $S("\n"));
        break;
      case 'r':
        String_Concat(self, $S("\r"));
        break;
      case 't':
        String_Concat(self, $S("\t"));
        break;
      case 'v':
        String_Concat(self, $S("\v"));
        break;
      case '\\':
        String_Concat(self, $S("\\"));
        break;
      case '\'':
        String_Concat(self, $S("\'"));
        break;
      case '"':
        String_Concat(self, $S("\""));
        break;
      case '?':
        String_Concat(self, $S("\?"));
        break;
      default:
        throw(FormatError, "Unknown Escape Sequence '\\%c'!", chr);
      }
    }

    char buffer[2];
    buffer[0] = (char)c_int(chr);
    buffer[1] = '\0';

    String_Concat(self, $S(buffer));
  }

  return pos;
}

var String = Cello(String, Instance(New, String_New, String_Del),
                   Instance(Assign, String_Assign), Instance(Cmp, String_Cmp),
                   Instance(Hash, String_Hash), Instance(Len, String_Len),
                   Instance(Get, NULL, NULL, String_Mem, String_Rem),
                   Instance(Resize, String_Resize),
                   Instance(Concat, String_Concat, String_Concat),
                   Instance(C_Str, String_C_Str),
                   Instance(Format, String_Format_To, String_Format_From),
                   Instance(Show, String_Show, String_Look));
