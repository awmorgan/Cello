#include "Cello.h"

var Format = Cello(Format);

int format_to_va(var self, int pos, const char *fmt, va_list va) {
  return method(self, Format, format_to, pos, fmt, va);
}

int format_from_va(var self, int pos, const char *fmt, va_list va) {
  return method(self, Format, format_from, pos, fmt, va);
}

int format_to(var self, int pos, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int ret = format_to_va(self, pos, fmt, va);
  va_end(va);
  return ret;
}

int format_from(var self, int pos, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int ret = format_from_va(self, pos, fmt, va);
  va_end(va);
  return ret;
}

var Show = Cello(Show);

int show(var self) { return show_to(self, $(File, stdout), 0); }

int show_to(var self, var out, int pos) {

  struct Show *s = instance(self, Show);
  if (s && s->show) {
    return s->show(self, out, pos);
  }

  return print_to(out, pos, "<'%s' At 0x%p>", type_of(self), self);
}

int print_with(const char *fmt, var args) {
  return print_to_with($(File, stdout), 0, fmt, args);
}

int println_with(const char *fmt, var args) {
  int pos = 0;
  pos = print_to_with($(File, stdout), pos, fmt, args);
  pos = print_to($(File, stdout), pos, "\n");
  return pos;
}

int print_to_with(var out, int pos, const char *fmt, var args) {

  char *fmt_buf = malloc(strlen(fmt) + 1);
  size_t index = 0;

  while (true) {

    if (*fmt == '\0') {
      break;
    }

    const char *start = fmt;

    /* Match String */
    while (*fmt != '\0' && *fmt != '%') {
      fmt++;
    }

    if (start != fmt) {
      memcpy(fmt_buf, start, fmt - start);
      fmt_buf[fmt - start] = '\0';
      int off = format_to(out, pos, fmt_buf);
      if (off < 0) {
        throw(FormatError, "Unable to output format!");
      }
      pos += off;
      continue;
    }

    /* Match %% */
    if (*fmt == '%' && *(fmt + 1) == '%') {
      int off = format_to(out, pos, "%%");
      if (off < 0) {
        throw(FormatError, "Unable to output '%%%%'!");
      }
      pos += off;
      fmt += 2;
      continue;
    }

    /* Match Format Specifier */
    while (!strchr("diuoxXfFeEgGaAxcsp$", *fmt)) {
      fmt++;
    }

    if (start != fmt) {

      memcpy(fmt_buf, start, fmt - start + 1);
      fmt_buf[fmt - start + 1] = '\0';

      if (index >= len(args)) {
        throw(FormatError, "!enough arguments to Format String!");
      }

      var a = get(args, $I(index));
      index++;

      if (*fmt == '$') {
        pos = show_to(a, out, pos);
      }

      if (*fmt == 's') {
        int off = format_to(out, pos, fmt_buf, c_str(a));
        if (off < 0) {
          throw(FormatError, "Unable to output String!");
        }
        pos += off;
      }

      if (strchr("diouxX", *fmt)) {
        int off = format_to(out, pos, fmt_buf, c_int(a));
        if (off < 0) {
          throw(FormatError, "Unable to output Int!");
        }
        pos += off;
      }

      if (strchr("fFeEgGaA", *fmt)) {
        int off = format_to(out, pos, fmt_buf, c_float(a));
        if (off < 0) {
          throw(FormatError, "Unable to output Real!");
        }
        pos += off;
      }

      if (*fmt == 'c') {
        int off = format_to(out, pos, fmt_buf, c_int(a));
        if (off < 0) {
          throw(FormatError, "Unable to output Char!");
        }
        pos += off;
      }

      if (*fmt == 'p') {
        int off = format_to(out, pos, fmt_buf, a);
        if (off < 0) {
          throw(FormatError, "Unable to output Object!");
        }
        pos += off;
      }

      fmt++;
      continue;
    }

    throw(FormatError, "Invalid Format String!");
  }

  free(fmt_buf);

  return pos;
}

int look(var self) { return look_from(self, $(File, stdin), 0); }

int look_from(var self, var input, int pos) {
  return method(self, Show, look, input, pos);
}

int scan_with(const char *fmt, var args) {
  return scan_from_with($(File, stdin), 0, fmt, args);
}

int scanln_with(const char *fmt, var args) {
  int pos = 0;
  pos = scan_from_with($(File, stdin), pos, fmt, args);
  pos = scan_from($(File, stdin), pos, "\n");
  return pos;
}

int scan_from_with(var input, int pos, const char *fmt, var args) {

  char *fmt_buf = malloc(strlen(fmt) + 4);
  size_t index = 0;

  while (true) {

    if (*fmt == '\0') {
      break;
    }

    const char *start = fmt;

    /* Match String */
    while (*fmt != '\0' && *fmt != '%') {
      fmt++;
    }

    if (start != fmt) {
      memcpy(fmt_buf, start, fmt - start);
      fmt_buf[fmt - start] = '\0';
      format_from(input, pos, fmt_buf);
      pos += (int)(fmt - start);
      continue;
    }

    /* Match %% */
    if (*fmt == '%' && *(fmt + 1) == '%') {
      int err = format_from(input, pos, "%%");
      if (err < 0) {
        throw(FormatError, "Unable to input '%%%%'!");
      }
      pos += 2;
      fmt += 2;
      continue;
    }

    /* Match Format Specifier */
    while (!strchr("diuoxXfFeEgGaAxcsp$[^]", *fmt)) {
      fmt++;
    }

    if (start != fmt) {

      int off = 0;
      memcpy(fmt_buf, start, fmt - start + 1);
      fmt_buf[fmt - start + 1] = '\0';
      strcat(fmt_buf, "%n");

      if (index >= len(args)) {
        throw(FormatError, "!enough arguments to Format String!");
      }

      var a = get(args, $I(index));
      index++;

      if (*fmt == '$') {
        pos = look_from(a, input, pos);
      }

      else if (*fmt == 's') {
        int err = format_from(input, pos, fmt_buf, c_str(a), &off);
        if (err < 1) {
          throw(FormatError, "Unable to input String!");
        }
        pos += off;
      }

      /* TODO: Test */
      else if (*fmt == ']') {
        int err = format_from(input, pos, fmt_buf, c_str(a), &off);
        if (err < 1) {
          throw(FormatError, "Unable to input Scanset!");
        }
        pos += off;
      }

      else if (strchr("diouxX", *fmt)) {
        long tmp = 0;
        int err = format_from(input, pos, fmt_buf, &tmp, &off);
        if (err < 1) {
          throw(FormatError, "Unable to input Int!");
        }
        pos += off;
        assign(a, $I(tmp));
      }

      else if (strchr("fFeEgGaA", *fmt)) {
        if (strchr(fmt_buf, 'l')) {
          double tmp = 0;
          int err = format_from(input, pos, fmt_buf, &tmp, &off);
          if (err < 1) {
            throw(FormatError, "Unable to input Float!");
          }
          pos += off;
          assign(a, $F(tmp));
        } else {
          float tmp = 0;
          int err = format_from(input, pos, fmt_buf, &tmp, &off);
          if (err < 1) {
            throw(FormatError, "Unable to input Float!");
          }
          pos += off;
          assign(a, $F(tmp));
        }
      }

      else if (*fmt == 'c') {
        char tmp = '\0';
        int err = format_from(input, pos, fmt_buf, &tmp, &off);
        if (err < 1) {
          throw(FormatError, "Unable to input Char!");
        }
        pos += off;
        assign(a, $I(tmp));
      }

      else if (*fmt == 'p') {
        void *tmp = NULL;
        int err = format_from(input, pos, fmt_buf, &tmp, &off);
        if (err < 1) {
          throw(FormatError, "Unable to input Ref!");
        }
        pos += off;
        assign(a, $R(tmp));
      }

      else {
        /* TODO: Report Better */
        throw(FormatError, "Invalid Format Specifier!");
      }

      fmt++;
      continue;
    }
  }

  free(fmt_buf);

  return pos;
}
