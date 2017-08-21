#ifndef __PARTICLE_ERROR_H__
#define __PARTICLE_ERROR_H__

#include <stdarg.h>
#include "token.h"
#include "file.h"

void error(const char *, ... );
void fail(const char *, ... );
void report(File *, Token *, const char *, ... );
void expected(File *, Token *, const char *, ... );

#endif /* __PARTICLE_ERROR_H__ */
