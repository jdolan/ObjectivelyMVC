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
    float4 out_rect [[user(locn2)]];
    float2 out_shape [[user(locn3)]];
    float2 out_position [[user(locn4)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 in_position [[attribute(0)]];
    float2 in_texcoord [[attribute(1)]];
    float4 in_color [[attribute(2)]];
    float4 in_rect [[attribute(3)]];
    float2 in_shape [[attribute(4)]];
};

vertex main0_out main0(main0_in in [[stage_in]], constant Projection& _19 [[buffer(0)]])
{
    main0_out out = {};
    out.gl_Position = _19.proj * float4(in.in_position, 0.0, 1.0);
    out.out_texcoord = in.in_texcoord;
    out.out_color = in.in_color;
    out.out_rect = in.in_rect;
    out.out_shape = in.in_shape;
    out.out_position = in.in_position;
    return out;
}

