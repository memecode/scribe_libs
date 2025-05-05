#pragma once

#include <assert.h>
#include <stdlib.h>

#define STDC_HEADERS		1
#define CACHE_SORTED		1
#define HAVE_STRING_H		1

#ifndef __cplusplus
#define true				1
#define false				0
#endif

#define LIBBTREE			extern

#ifdef __cplusplus
extern "C" {
#endif

extern int __CacheHit;
extern int __ReadHit;
extern int __colide;
extern int __total;

#ifdef __cplusplus
}
#endif

#if 0

#include <stdio.h>
extern size_t my_fread(void *buffer, size_t size, size_t count, FILE *stream);
extern size_t my_fwrite(void *buffer, size_t size, size_t count, FILE *stream);
#define fread				my_fread
#define fwrite				my_fwrite

#endif

void
btreeSetError(const char *Err, ...);

extern const char *btreeGetError(void);
