#ifndef __PARTICLE_EMIT_H__
#define __PARTICLE_EMIT_H__

#include <stdarg.h>
#include "file.h"

void emit(File *file, const char *format, ... );
void emitln(File *file, const char *format, ... );

#endif /* __PARTICLE_EMIT_H__ */
