
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
    // To be compatible with 2D(render text), the backbuffer and texture are specified with a format of DXGI_FORMAT_B8G8R8A8_UNORM,
    // but actually the image resource loaded by Direct3DImage::LoadImage has a format of "RGBA" for performance.
    // So the components of the pixel sampled from texture_resource here should be swapped to make 
    // a vector of format "(r,g,b,a)" as the output of the pixel shader.
    float4 pixel = texture_resource.Sample(sample_state, input.tex);
    float r = pixel.b;
    pixel.b = pixel.r;
    pixel.r = r;
    
    return pixel;
}