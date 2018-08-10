
struct PixelShader_INPUT
{
    float4 position : SV_POSITION;
    float2 tex    : TEXCOORD0;
};

SamplerState sample_state;
//Texture2D tex_r;
//Texture2D tex_g;
//Texture2D tex_b;
//Texture2D tex_a;
Texture2D texture_resource;
float4 main(PixelShader_INPUT input) : SV_TARGET
{
    float4 pixel = texture_resource.Sample(sample_state, input.tex);
    return pixel;
}