
struct VertexShader_INPUT
{
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct PixelShader_INPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

PixelShader_INPUT main(VertexShader_INPUT input)
{
    PixelShader_INPUT output;
    output.position.w = 1;
    output.position.x = input.position.x;
    output.position.y = input.position.y;
    output.position.z = input.position.z;

    output.color = input.color;

    return output;
}