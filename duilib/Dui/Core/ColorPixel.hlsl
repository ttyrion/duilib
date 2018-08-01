
struct PixelShader_INPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

float4 main(PixelShader_INPUT input) : SV_TARGET
{
    return float4(input.color.r, input.color.g, input.color.b, 1.0f);
}