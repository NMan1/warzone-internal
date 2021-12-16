#pragma once
#include "..\imports\crt\crt.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D11_IMPLEMENTATION

#define STBTT_ifloor(x)   ((int) crt::floorf(x))
#define STBTT_iceil(x)    ((int) crt::ceilf(x))
#define STBTT_sqrt(x)      crt::sqrtf(x)
#define STBTT_pow(x,y)     crt::pow(x,y)
#define STBTT_fmod(x,y)    crt::fmod(x,y)
#define STBTT_cos(x)       crt::cosf(x)
#define STBTT_acos(x)      crt::acosf(x)
#define STBTT_fabs(x)      crt::fabs(x)
#define STBTT_strlen(x)    crt::strlen(x)
#define STBTT_memcpy       crt::memcpy
#define STBTT_memset       crt::memset
#define STBRP_SORT		   crt::qsort

#include "nuklear.h"
#include "nuklear_d3d11.h"