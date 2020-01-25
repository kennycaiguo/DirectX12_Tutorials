
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
    float4 gPulseColor;
    float gTime;
}

struct VertexIn
{
	float3 iPos : POSITION;
	float4 iColor : COLOR;
};

struct VertexOut
{
	float4 oPosH : SV_POSITION;
	float4 oColor : COLOR;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
    vIn.iPos.xy += 0.5f * sin(vIn.iPos.x) * sin(3.0f * gTime);
    vIn.iPos.z *= 0.6f + 0.4f * sin(2.0f * gTime);
	vOut.oPosH = mul(float4(vIn.iPos, 1.0f), gWorldViewProj);
	vOut.oColor = vIn.iColor;
	return vOut;
}


float4 PS(VertexOut v) : SV_Target
{
    const float pi = 3.14159;
    float s = 0.5f * sin(2 * gTime - 0.25f * pi) + 0.5f;
    float4 c = lerp(v.oColor, gPulseColor, s);
    return c;
}
