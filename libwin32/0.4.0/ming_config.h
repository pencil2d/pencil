/* src/ming_config.h.  Generated from ming_config.h.in by configure.  */
/* src/ming_config.h.in.  Generated from configure.in by autoheader.  */

#ifndef __MING_CONFIG_H
#define __MING_CONFIG_H


/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if you have the <gif_lib.h> header file. */
/* #undef HAVE_GIF_LIB_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkstemp' function. */
/* #undef HAVE_MKSTEMP */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vasprintf' function. */
/* #undef HAVE_VASPRINTF */

/* Define to 1 if you have the <zlib.h> header file. */
#define HAVE_ZLIB_H 1

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "ming"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "ming"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ming 0.4.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ming"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.4.2"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define this if you want Ming to track all objects allocations. Ming will
   mantain a doubly linked list of allocated objects, call
   Ming_collectGarbage() to get rid of them all */
#define TRACK_ALLOCS 1

/* Use c++ */
#define USE_CXX 1

/* Use freetype library */
#define USE_FREETYPE 1

/* Use gif library */
/* #undef USE_GIF */

/* Use png library */
#define USE_PNG 1

/* Use zlib */
#define USE_ZLIB 1

/* Version number of package */
#define VERSION "0.4.2"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

#endif

