/**
 * ObjectivelyMVC UI vertex shader.
 *
 * Inputs  (location 0): float2 position
 *         (location 1): float2 texcoord
 * Uniform (b0, space1): float4x4 projection matrix
 * Outputs             : float2 texcoord (passed through), SV_Position
 */

cbuffer Projection : register(b0, space1) {
    float4x4 matrix;
};

struct VSInput {
    float2 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
};

struct VSOutput {
    float2 texcoord : TEXCOORD0;
    float4 position : SV_Position;
};

VSOutput vs_main(VSInput input) {
    VSOutput output;
    output.position = mul(matrix, float4(input.position, 0.0, 1.0));
    output.texcoord = input.texcoord;
    return output;
}
