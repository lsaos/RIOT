#include "stddef.h"

int
strcmp (const char *s1,
	const char *s2)
{
    while (*s1 != '\0' && *s1 == *s2)
    {
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

int 
strncmp (const char *s1,
	const char *s2,
	size_t n)
{
    if (n == 0)
        return 0;

    while (n-- != 0 && *s1 == *s2)
    {
        if (n == 0 || *s1 == '\0')
            break;
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

void *
memcpy (void *__restrict dst0,
	const void *__restrict src0,
	size_t len0)
{
    char *dst = (char *) dst0;
    char *src = (char *) src0;

    void *save = dst0;

    while (len0--)
    {
        *dst++ = *src++;
    }

    return save;
}

void *
memmove (void *dst_void,
	const void *src_void,
	size_t length)
{
    char *dst = dst_void;
    const char *src = src_void;

    if (src < dst && dst < src + length)
    {
        /* Have to copy backwards */
        src += length;
        dst += length;
        while (length--)
    {
        *--dst = *--src;
    }
    }
    else
    {
        while (length--)
    {
        *dst++ = *src++;
    }
    }

    return dst_void;
}
