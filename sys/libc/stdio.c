#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "fmt.h"

enum {
    FLAG_ZERO_PAD               = 0x1,
    FLAG_ALT                    = 0x2,
    FLAG_LEFT_ADJUST            = 0x4,
    FLAG_BLANK_BEFORE_POSITIVE  = 0x8,
    FLAG_ALWAYS_SIGN            = 0x10,
    FLAG_PRECISION              = 0x20,
    FLAG_WIDTH                  = 0x40,
    FLAG_LONG                   = 0x80,
    FLAG_LONG_LONG              = 0x100,
};

typedef struct {
    FILE super;
    int fd;
} _FD_FILE;

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
              FILE *stream)
{
    size_t written = 0;
    while (nmemb--) {
        written += stream->out(stream, ptr, size);
        ptr += size;
    }
    return written;
}

int getc(FILE *f)
{
    unsigned char c;
    ssize_t res = f->in(f, (char *)&c, 1);
    if (res == 1) {
        return (int)c;
    }
    else {
        return res;
    }
}

int getchar(void)
{
    return getc(stdin);
}

int printf(const char *restrict fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    va_end(ap);
    return ret;
}

int putc(int c, FILE *stream)
{
    unsigned char _c = (unsigned char) c;
    ssize_t res = stream->out(stream, (char *)&_c, 1);
    if (res <= 0) {
        return EOF;
    }
    else {
        return c;
    }
}

int putchar(int c)
{
    return putc(c, stdout);
}

int puts(const char *s)
{
    print_str(s);
    print_str("\n");
    return 0;
}

size_t fprint_str(FILE *restrict f, const char *s)
{
    const char *tmp = s;

    while (s) {
        f->out(f, s++, 1);
    }
    return (s - tmp);
}

static size_t _fpad(FILE *restrict f, int width, int len, char pad_char)
{
    if (width >= 0) {
        if (len < width) {
            unsigned n = width - len;
            while (n--) {
                f->out(f, &pad_char, 1);
            }
            return width - len;
        }
    }
    return 0;
}

int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap)
{
    char buf[16];
    const char *prefix;

    size_t len = 0;
    char c;
    char *s;

    while ((c = *fmt++)) {
        if (c != '%') {
            f->out(f, &c, 1);
            len++;
        }
        else {
            unsigned flags = 0;
            int width = -1;
            int precision = -1;
            prefix = NULL;

            size_t (*int_conv_func)(uint8_t*, uint32_t) = NULL;

            /* flags */
            while ((c = *fmt++)) {
                switch (c) {
                    case '0':
                        flags |= FLAG_ZERO_PAD;
                        continue;
                    case '#':
                        flags |= FLAG_ALT;
                        continue;
                    case '-':
                        flags |= FLAG_LEFT_ADJUST;
                        continue;
                    case ' ':
                        flags |= FLAG_BLANK_BEFORE_POSITIVE;
                        continue;
                    case '+':
                        flags |= FLAG_ALWAYS_SIGN;
                        continue;
                    default:
                        fmt--;
                }
                break;
            }

            while ((c = *fmt++)) {
                switch (c) {
                    case 'l':
                        flags |= flags & FLAG_LONG ? FLAG_LONG_LONG : FLAG_LONG;
                        break;
                    case '.':
                        flags |= FLAG_PRECISION;
                        break;
                    case 's':
                        s = va_arg(ap, char *);
                        unsigned slen = (flags & FLAG_PRECISION) ? (size_t)precision : strlen(s);
                        if (! (flags & FLAG_LEFT_ADJUST)) {
                            len += _fpad(f, width, slen, ' ');
                        }
                        len += f->out(f, s, slen);
                        if (flags & FLAG_LEFT_ADJUST) {
                            len += _fpad(f, width, slen, ' ');
                        }
                        goto done;
                    case 'd':
                    case 'i':
                        int_conv_func = (void *)fmt_s32_dec;
                        goto done;
                    case 'u':
                        int_conv_func = (void *)fmt_u32_dec;
                        goto done;
                    case 'p':
                        prefix = "0x";
                    case 'x':
                    case 'X':
                        int_conv_func = (void *)fmt_u32_hex;
                        goto done;
                    default:
                        if (isdigit(c)) {
                            uint32_t val = scn_u32_dec(fmt - 1, strlen(fmt - 1));
                            if (flags & FLAG_PRECISION) {
                                precision = val;
                                (void)precision;                         /* no float support */
                            }
                            else {
                                width = val;
                            }
                            while (isdigit(*fmt)) {
                                fmt++;
                            }
                        }
                }
            }
done:
            if (int_conv_func) {
                size_t clen;

                int32_t value;
                if ((sizeof(int) == 4) || (flags & FLAG_LONG)) {
                    value = va_arg(ap, int32_t);
                }
                else {
                    value = (int)va_arg(ap, int);
                }
                clen = int_conv_func((void*)buf, value);
                size_t prefix_len = prefix ? strlen(prefix) : 0;
                if (! (flags & FLAG_LEFT_ADJUST)) {
                    len += _fpad(f, width, clen + prefix_len, ' ');
                }
                if (prefix_len) {
                    len += f->out(f, prefix, prefix_len);
                }
                len += f->out(f, buf, clen);
                if (flags & FLAG_LEFT_ADJUST) {
                    len += _fpad(f, width, clen + prefix_len, ' ');
                }
            }
        }
    }

    return len;
}

static ssize_t _fdwrite(FILE *restrict f, const char *ptr, size_t n)
{
    return write(((_FD_FILE*)f)->fd, ptr, n);
}

static ssize_t _fdread(FILE *restrict f, char *ptr, size_t n)
{
    return read(((_FD_FILE*)f)->fd, ptr, n);
}


static const _FD_FILE _stdin = { .super.out=_fdwrite, .super.in=_fdread, .fd=STDIN_FILENO };
static const _FD_FILE _stdout = { .super.out=_fdwrite, .super.in=_fdread, .fd=STDOUT_FILENO };

FILE *const stdin = (FILE*)&_stdin;
FILE *const stdout = (FILE*)&_stdout;
