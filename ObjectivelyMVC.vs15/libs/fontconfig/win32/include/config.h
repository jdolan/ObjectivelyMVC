#pragma once

#define HAVE_CHSIZE 1
#define HAVE_FCNTL_H 1
#define HAVE_FT_BITMAP_SIZE_Y_PPEM 1
#define HAVE_FT_GET_NEXT_CHAR 1
#define HAVE_FT_GET_PS_FONT_INFO 1
#define HAVE_FT_HAS_PS_GLYPH_NAMES 1
#define HAVE_FT_SELECT_SIZE 1
//#define HAVE_FT_GET_BDF_PROPERTY 1
#define HAVE_INTTYPES_H 1
#define HAVE_MEMMOVE 1

#define HAVE_MEMORY_H 1
#define HAVE_MEMSET 1
#define HAVE_RAND 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRCHR 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_STRRCHR 1
#define HAVE_STRTOL 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_VPRINTF 1
#define STDC_HEADERS 1
#define USE_ICONV 0

#define FLEXIBLE_ARRAY_MEMBER 1

// Don't include mingw unistd.h
#define _UNISTD_H

#define VERSION "2.10.1"

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#define FC_DEFAULT_FONTS "WINDOWSFONTDIR"
#define FC_CACHEDIR "LOCAL_APPDATA_FONTCONFIG_CACHE"

#undef UNICODE
#undef _UNICODE

#ifndef MBCS
#define MBCS
#endif

#ifndef _MBCS
#define _MBCS
#endif

#define F_OK 0
#define S_ISREG(x) (((x) & S_IFREG) == S_IFREG)

#ifndef __cplusplus
#define inline __inline
#endif

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of `int', as computed by sizeof. */
#if defined(_M_X64)
#define SIZEOF_INT 8
#else
#define SIZEOF_INT 4
#endif

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG SIZEOF_INT

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `void*', as computed by sizeof. */
#define SIZEOF_VOIDP SIZEOF_INT

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P SIZEOF_INT

/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE 8

/* The normal alignment of `double', in bytes. */
#define ALIGNOF_DOUBLE SIZEOF_DOUBLE

/* The normal alignment of `void *', in bytes. */
#define ALIGNOF_VOID_P SIZEOF_VOID_P

#include <io.h>

struct _FcMatrix { double xx, xy, yx, yy; };

typedef struct _FcMatrix FcMatrix;