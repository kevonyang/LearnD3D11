cbuffer ConstantBuffer:register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer LightBuffer:register(b1)
{
	float4 LightDir;
	float4 LightColor;
}

Texture2D txDiffuse:register(t0);
SamplerState samLinear:register(s0);

struct VS_INPUT
{
	float4 Pos:POSITION;
	float3 Normal:NORMAL;
	float4 Color:COLOR;
	float2 Texcoord:TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos:SV_POSITION;
	float3 Normal:NORMAL;
	float4 Color:COLOR;
	float2 Texcoord:TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Color = input.Color;
	output.Normal = mul(input.Normal, World);
	output.Texcoord = input.Texcoord;
	return output;
}

float4 PS(PS_INPUT input):SV_Target
{
	float4 color = 0;
	float3 dir = normalize(LightDir.xyz);
	float3 normal = normalize(input.Normal.xyz);
	float4 texColor = txDiffuse.Sample(samLinear, input.Texcoord);
	color.rgb = saturate(dot(normal, dir) * LightColor.rgb * texColor.rgb);
	color.a = 1;
	return color;
}