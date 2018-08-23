#ifndef CDEFS_H
#define CDEFS_H

#ifndef __has_attribute
#define __has_attribute(x)  0
#endif
#ifndef __has_extension
#define __has_extension     __has_feature
#endif
#ifndef __has_feature
#define __has_feature(x)    0
#endif
#ifndef __has_include
#define __has_include(x)    0
#endif
#ifndef __has_builtin
#define __has_builtin(x)    0
#endif

#define __GNUC_PREREQ__(major, minor) ((__GNUC__ >= major) || ((__GNUC__ == major) && ((__GNUC_MINOR__) >= minor)))

#endif /* CDEFS_H */
