#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct UBO
{
    float4x4 ModelViewProj;
};

struct main0_out
{
    float4 out_color [[user(locn0)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 in_position [[attribute(0)]];
    float3 in_color [[attribute(1)]];
};

vertex main0_out main0(main0_in in [[stage_in]], constant UBO& _30 [[buffer(0)]])
{
    main0_out out = {};
    out.out_color = float4(in.in_color, 1.0);
    out.gl_Position = _30.ModelViewProj * float4(in.in_position, 1.0);
    return out;
}

