Texture2D<float> FontTexture : register(t0);
float4 FontColor : register(t1);

SamplerState FontSampler : register(s0);

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};


float4 main(PixelInput input) : SV_TARGET
{
    float4 color = FontTexture.Sample(FontSampler, input.UV);
    return color * FontColor;
}
