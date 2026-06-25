/**
 * ObjectivelyMVC UI fragment shader.
 *
 * Samples a 2D texture at the interpolated UV and multiplies by a per-draw
 * color pushed via SDL_PushGPUFragmentUniformData.
 *
 * Inputs  (TEXCOORD0): float2 texcoord from vertex stage
 * Sampler (t0/s0, space2): 2D RGBA texture + sampler
 * Uniform (b0, space3): float4 color multiplier
 * Output             : float4 blended RGBA
 */

cbuffer Color : register(b0, space3) {
    float4 rgba;
};

Texture2D tex    : register(t0, space2);
SamplerState smp : register(s0, space2);

struct PSInput {
    float2 texcoord : TEXCOORD0;
};

float4 fs_main(PSInput input) : SV_Target {
    return rgba * tex.Sample(smp, input.texcoord);
}
