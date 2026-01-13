/**
 * printk.c
 * Criado por Matheus Leme Da Silva
 */
#include "stdarg.h"
#include "stdint.h"
#include "string.h"
#include "terminal.h"

// Imprime uma string formatada
int printk(const char *format, ...) {
  if (!format)
    return 0;
  va_list args;
  va_start(args, format);
  int count = 0;

  while (*format) {
    if (*format != '%') {
      count++;
      terminal_putchar(*format++);
      continue;
    }

    format++;

    int neg_pad = 0;
    if (*format == '-') {
      neg_pad = 1;
      format++;
    }

    int zero_pad = 0;
    if (*format == '0') {
      zero_pad = 1;
      format++;
    }

    int pad = 0;
    if (*format >= '0' && *format <= '9') {
      while (*format >= '0' && *format <= '9') {
        pad = pad * 10 + (*format - '0');
        format++;
      }
    }

    int l = 0;
    if (*format == 'l') {
      format++;
      if (*format == 'l') {
        l = 1;
        format++;
      }
    } else if (*format == 'h') {
      format++;
      if (*format == 'h')
        format++;
    }

    switch (*format) {
    case 'c': {
      char c = (char)va_arg(args, int);
      terminal_putchar(c);
      count++;
    } break;
    case 's': {
      char *s = (char *)va_arg(args, char *);
      if (!s) {
        if (!neg_pad) {
          for (int i = 6; i < pad; i++) {
            terminal_putchar(' ');
          }
        }
        terminal_putstring("(null)");
        count += 6 + pad;
        if (neg_pad) {
          for (int i = 6; i < pad; i++) {
            terminal_putchar(' ');
          }
        }

      } else {
        int len = strlen(s);

        if (!neg_pad) {
          for (int i = len; i < pad; i++) {
            terminal_putchar(' ');
            len++;
          }
        }

        while (*s) {
          terminal_putchar(*s++);
        }

        if (neg_pad) {
          for (int i = len; i < pad; i++) {
            terminal_putchar(' ');
            len++;
          }
        }

        count += len;
      }
    } break;
    case 'd': {
      i64 num = 0;
      if (l)
        num = (i64)va_arg(args, i64);
      else
        num = (i64)va_arg(args, i32);
      char buf[65];
      int is_neg = 0;
      u64 unum = (u64)num;

      if (num < 0) {
        is_neg = 1;
        unum = (u64)(-(num + 1)) + 1;
      } else {
        unum = (u64)num;
      }
      count += int_to_ascii(unum, 10, is_neg, 0, pad, zero_pad, buf);
      terminal_putstring(buf);
    } break;
    case 'u': {
      u64 unum = 0;
      if (l)
        unum = (u64)va_arg(args, u64);
      else
        unum = (u64)va_arg(args, u32);
      char buf[65];
      count += int_to_ascii(unum, 10, 0, 0, pad, zero_pad, buf);
      terminal_putstring(buf);
    } break;
    case 'x': {
      u64 unum = 0;
      if (l)
        unum = (u64)va_arg(args, u64);
      else
        unum = (u64)va_arg(args, u32);
      char buf[65];
      count += int_to_ascii(unum, 16, 0, 0, pad, zero_pad, buf);
      terminal_putstring(buf);
    } break;
    case 'X': {
      u64 unum = 0;
      if (l)
        unum = (u64)va_arg(args, u64);
      else
        unum = (u64)va_arg(args, u32);
      char buf[65];
      count += int_to_ascii(unum, 16, 0, 1, pad, zero_pad, buf);
      terminal_putstring(buf);
    } break;
    case 'b': {
      u64 unum = 0;
      if (l)
        unum = (u64)va_arg(args, u64);
      else
        unum = (u64)va_arg(args, u32);
      char buf[65];
      count += int_to_ascii(unum, 2, 0, 0, pad, zero_pad, buf);
      terminal_putstring(buf);
    } break;
    case 'o': {
      u64 unum = 0;
      if (l)
        unum = (u64)va_arg(args, u64);
      else
        unum = (u64)va_arg(args, u32);
      char buf[65];
      count += int_to_ascii(unum, 8, 0, 0, pad, zero_pad, buf);
      terminal_putstring(buf);
    } break;
    default: {
      terminal_putchar(*format++);
      count++;
    } break;
    }
    format++;
  }

  va_end(args);
  return count;
}
