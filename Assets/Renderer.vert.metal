#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Projection
{
    float4x4 proj;
};

struct main0_out
{
    float2 out_texcoord [[user(locn0)]];
    float4 out_color [[user(locn1)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 in_position [[attribute(0)]];
    float2 in_texcoord [[attribute(1)]];
    float4 in_color [[attribute(2)]];
};

vertex main0_out main0(main0_in in [[stage_in]], constant Projection& _19 [[buffer(0)]])
{
    main0_out out = {};
    out.gl_Position = _19.proj * float4(in.in_position, 0.0, 1.0);
    out.out_texcoord = in.in_texcoord;
    out.out_color = in.in_color;
    return out;
}

