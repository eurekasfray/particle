// Debug
#ifndef __PARTICLE_DEBUG_H__
#define __PARTICLE_DEBUG_H__

#define DEBUG 0

#if DEBUG
#define D(x) x
#else
#define D(x)
#endif

#if DEBUG
#define E(x) printf("DEBUG: "); printf(x); printf("\n");
#else
#define E(x)
#endif

#endif /* __PARTICLE_DEBUG_H__ */
