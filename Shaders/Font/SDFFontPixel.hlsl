Texture2D u_sampler;
SamplerState u_sampler_state;

cbuffer Constants : register(b0)
{
    float onedge_value = 0.8;
};

struct PS_INPUT
{
    float2 frag_tex : TEXCOORD0;
    float4 frag_col : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // Sample the texture to find out how far from the edge we are
    // when `dist > onedge_value` you are inside the glyph
    // otherwise you are outside the glyph
    // Higher on edge values give better outlines
    float dist = u_sampler.Sample(u_sampler_state, input.frag_tex).r;

    // Gets the sum of the absolute derivatives in x and y
    // using local differencing for the input argument
    float w = fwidth(dist);

    // Use this value to smooth out a tiny edge,
    // this will automatically scale nicely for any size text
    float4 out_col = input.frag_col;
    out_col.a *= smoothstep(onedge_value - w, onedge_value + w, dist);

    return out_col;
}