#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float4 out_color [[color(0)]];
};

struct main0_in
{
    float2 in_texcoord [[user(locn0)]];
    float4 in_color [[user(locn1)]];
    float4 in_rect [[user(locn2), flat]];
    float2 in_shape [[user(locn3), flat]];
    float2 in_position [[user(locn4)]];
};

static inline __attribute__((always_inline))
float roundedRect(thread const float2& p, thread const float2& b, thread const float& r)
{
    float2 q = (abs(p) - b) + float2(r);
    return (length(fast::max(q, float2(0.0))) + fast::min(fast::max(q.x, q.y), 0.0)) - r;
}

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler texSmplr [[sampler(0)]])
{
    main0_out out = {};
    out.out_color = in.in_color * tex.sample(texSmplr, in.in_texcoord);
    if (in.in_rect.z > 0.0)
    {
        float r = fast::min(in.in_shape.x, fast::min(in.in_rect.z, in.in_rect.w));
        float2 param = in.in_position - in.in_rect.xy;
        float2 param_1 = in.in_rect.zw;
        float param_2 = r;
        float d = roundedRect(param, param_1, param_2);
        if (in.in_shape.y > 0.0)
        {
            d = fast::max(d, -(d + in.in_shape.y));
        }
        float aa = fast::max(fwidth(d), 9.9999997473787516355514526367188e-05);
        out.out_color.w *= fast::clamp(0.5 - (d / aa), 0.0, 1.0);
    }
    return out;
}

