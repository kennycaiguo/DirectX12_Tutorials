
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
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
	vOut.oPosH = mul(float4(vIn.iPos, 1.0f), gWorldViewProj);
	vOut.oColor = vIn.iColor;
	return vOut;
}


float4 PS(VertexOut v) : SV_Target
{
	return v.oColor;
}
