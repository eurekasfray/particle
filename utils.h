#ifndef __PARTICLE_UTILS_H__
#define __PARTICLE_UTILS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// String utils
int uppercase(int);
int lowercase(int);
char *substr(const char *, size_t);
char *dupstr(const char *);

// Error-trapped utils
FILE *efopen(const char *, const char *);
void *emalloc(size_t);
void *erealloc(void *, size_t);

#endif /* __PARTICLE_UTILS_H__ */
