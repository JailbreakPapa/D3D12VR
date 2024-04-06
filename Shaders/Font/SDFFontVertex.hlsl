struct VS_INPUT
{
    float2 vert_pos : POSITION;
    float2 vert_tex : TEXCOORD0;
    float4 vert_col : COLOR0;
};

struct VS_OUTPUT
{
    float2 frag_tex : TEXCOORD0;
    float4 frag_col : COLOR0;
    float4 pos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // You will need to apply a transformation to vert_pos if it wasn't done in c
    output.pos = float4(input.vert_pos, 0.0, 1.0);
    output.frag_tex = input.vert_tex;
    output.frag_col = input.vert_col;

    return output;
}