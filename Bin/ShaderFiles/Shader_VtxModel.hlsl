
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float			g_fAlpha = 1.f;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_SpecularTexture;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;

	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

/* DrawIndexed�Լ��� ȣ���ϸ�. */
/* �ε������ۿ� ����ִ� �ε�����°�� ������ VS_MAIN�Լ��� ���ڷ� ������. VS_IN�� ����ȴ�. */
/* �Ϲ������� TriangleList�� �׸����, ���� ������ ���� VS_MAIN�Լ��� ���ڷ� ������. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

	/* ������ ��ġ�� ���� �� ��������� ���Ѵ�. ���� ������ ViewSpace�� �����ϳ�. */
	/* ������ı��� ���ϸ� ������ġ�� w�� �佺���̽� ���� z�� �����Ѵ�. == Out.vPosition�� �ݵ�� float4�̾���ϴ� ����. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = vNormal.xyz;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = cross(Out.vNormal, Out.vTangent);
	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;

	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vSpecular : SV_TARGET3;
	vector		vLightDepth : SV_TARGET4;
};

/* �̷��� ������� �ȼ��� PS_MAIN�Լ��� ���ڷ� ������. */
/* �����ϴ� ���� Target0 == ��ġ�� 0��°�� ���ε��Ǿ��ִ� ����Ÿ��(�Ϲ������� �����)�� �׸���. */
/* �׷��� ����ۿ� ���� �׷�����. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	
	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.0f)
		discard;

	return Out;
}



PS_OUT PS_MAIN_SHADOW(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vLightDepth.r = In.vProjPos.w / 500.f;

	Out.vLightDepth.a = 1.f;

	return Out;
}


PS_OUT PS_BLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vNormal = vector(1.f, 1.f, 1.f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);


	float4 vTextureColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//float luminance = dot(vTextureColor, float3(0.3, 0.59, 0.11));
	//float threshold = 0.5;
	////if (luminance > threshold)
	////{
	//	// Sample the texture multiple times at offsets from the current texel
	//	float2 offsets[9] = {
	//		float2(-1, -1), float2(0, -1), float2(1, -1),
	//		float2(-1,  0), float2(0,  0), float2(1,  0),
	//		float2(-1,  1), float2(0,  1), float2(1,  1)
	//	};
	//	float4 sum = 0;
	//	for (int i = 0; i < 9; i++)
	//	{
	//		sum += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offsets[i] * 0.004);
	//	}
	//	// Average the samples and blend with the original image
	//	float4 blur = sum / 9;
	//	// blend the original image with the blurred bright pixels
	//	float blendFactor = 0.5;
	//	vTextureColor = lerp(vTextureColor, blur, blendFactor);
	////}

	vTextureColor.rgb += 0.1f;
	Out.vDiffuse = vTextureColor;
	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_AlphaBlend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

	pass Glow_Shader
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}

}