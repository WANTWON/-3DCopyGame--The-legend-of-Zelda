
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float			g_fAlpha = 1.f;
texture2D		g_DiffuseTexture;
float			g_TexUV = 1.f;


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
};

/* DrawIndexed함수를 호출하면. */
/* 인덱스버퍼에 담겨있는 인덱스번째의 정점을 VS_MAIN함수에 인자로 던진다. VS_IN에 저장된다. */
/* 일반적으로 TriangleList로 그릴경우, 정점 세개를 각각 VS_MAIN함수의 인자로 던진다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재하낟. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = vNormal.xyz;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
};

/* 이렇게 만들어진 픽셀을 PS_MAIN함수의 인자로 던진다. */
/* 리턴하는 색은 Target0 == 장치에 0번째에 바인딩되어있는 렌더타겟(일반적으로 백버퍼)에 그린다. */
/* 그래서 백버퍼에 색이 그려진다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;


	Out.vDiffuse.rgb = 1.f;
	Out.vDiffuse.b -= 0.5f;

	if (1 - g_TexUV < In.vTexUV.x)
		discard;

	return Out;
}


PS_OUT PS_MAIN_RollCut(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;


	Out.vDiffuse.rg = 1.f;
	Out.vDiffuse.g -= 0.2f;
	Out.vDiffuse.b  = (1 - Out.vDiffuse.a);

	if (1 - g_TexUV < In.vTexUV.x)
		discard;

	return Out;
}

PS_OUT PS_MAIN_HitFlash(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	vector OrangeColor = vector(255, 191, 95, 255) / 255.f;
	vector BrownColor = vector(127, 95, 28, 255) / 255.f;
	vector YellowColor = vector(255, 255, 100, 255) / 255.f;
	vector LightOrangeColor = vector(255, 230, 150, 255) / 255.f;

	Out.vDiffuse.rgb = BrownColor.rgb * (1 - Out.vDiffuse.r) + OrangeColor.rgb * Out.vDiffuse.r;


	return Out;
}

PS_OUT PS_MAIN_HitRing(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	vector OrangeColor = vector(255, 121, 0, 255) / 255.f;
	vector BrownColor = vector(127, 95, 28, 255) / 255.f;
	vector YellowColor = vector(255, 255, 100, 255) / 255.f;
	vector LightOrangeColor = vector(255, 230, 150, 255) / 255.f;

	Out.vDiffuse.rgb = OrangeColor.rgb * (1 - Out.vDiffuse.r) + LightOrangeColor.rgb * Out.vDiffuse.r;


	Out.vDiffuse.a *= g_fAlpha;

	return Out;
}


PS_OUT PS_MAIN_HitSpark(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	vector OrangeColor = vector(255, 121, 0, 255) / 255.f;
	vector LightOrangeColor = vector(255, 191, 95, 255) / 255.f;
	vector BrownColor = vector(137, 74, 25, 255) / 255.f;
	vector DarkBrownColor = vector(50, 24, 0, 255) / 255.f;
	

	Out.vDiffuse.rgb = OrangeColor.rgb * (1 - Out.vDiffuse.r) + LightOrangeColor.rgb * Out.vDiffuse.r;
	Out.vDiffuse.a *= g_fAlpha;
	return Out;
}


PS_OUT PS_GRASS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a *= g_fAlpha;

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultSlash
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
	pass RollCut
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RollCut();
	}
	
	pass HitFlash
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HitFlash();
	}
	
	pass HitRing
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HitRing();
	}
	
	pass HitSpark
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HitSpark();
	}

	pass Grass
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GRASS();
	}

}