#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

enum {
    EBADF,
    EINVAL,
    EOVERFLOW,
    ETIMEDOUT,
};

#endif /* ERRNO_H */
