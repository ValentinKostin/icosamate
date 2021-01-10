#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <cmath>
#include <stdint.h>
#include "../def.h"

bool LoadFile(const char *fileName, bool binary, uint8_t **buffer, uint32_t *size);

#define LOG_ERROR(str,...) {} // ZAGL
#define LOG_DEBUG(str,...) {} // ZAGL
#define ASSERT(a) check(a)

#endif /* COMMON_H */
