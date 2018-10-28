#pragma once

#include <stdint.h>

typedef uint8_t u8;

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

inline float lerp(float v0, float v1, float t) { return (1 - t) * v0 + t * v1; }
