
struct PixelShader_INPUT
{
    float4 position : SV_POSITION;
    float2 tex    : TEXCOORD0;
};

SamplerState sample_state;
Texture2D tex_r;
Texture2D tex_g;
Texture2D tex_b;
Texture2D tex_a;
float4 main(PixelShader_INPUT input) : SV_TARGET
{
    float r = tex_r.Sample(sample_state, input.tex).r;
    float g = tex_g.Sample(sample_state, input.tex).r;
    float b = tex_b.Sample(sample_state, input.tex).r;
    float a = tex_a.Sample(sample_state, input.tex).r;
    return float4(r, g, b, a);
}