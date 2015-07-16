#pragma once
#include "d3dcommon.h"
#include <memory>

#define comptr Microsoft::WRL::ComPtr
#define uptr std::unique_ptr
#define sptr std::shared_ptr

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef struct float3 { float x, y, z; } float3;
typedef struct float4 { float x, y, z, w; } float4;
typedef D3DXVECTOR3 vec3f;
typedef D3DXVECTOR4 vec4f;
typedef D3DXMATRIX mat4x4;

typedef struct int2 { int x, y; } int2;
typedef struct int3 { int x, y, z; } int3;
typedef struct uint2 { uint x, y; } uint2;
typedef struct uint3 { uint x, y, z; } uint3;
