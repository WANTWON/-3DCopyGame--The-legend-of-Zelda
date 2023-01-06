
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float			g_fAlpha = 1.f;
texture2D		g_DiffuseTexture;
texture2D		g_DepthTexture;
texture2D		g_DissolveTexture;
texture2D		g_SmokeDstTexture;
vector			g_ColorBack = vector(1.f, 1.f, 1.f, 1.f); 
vector			g_ColorFront = vector(1.f, 1.f, 1.f, 1.f);



struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT_SOFTEFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
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

	/* ������ ��ġ�� ���� �� ��������� ���Ѵ�. ���� ������ ViewSpace�� �����ϳ�. */
	/* ������ı��� ���ϸ� ������ġ�� w�� �佺���̽� ���� z�� �����Ѵ�. == Out.vPosition�� �ݵ�� float4�̾���ϴ� ����. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_SOFTEFFECT VS_MAIN_SOFTEFFECT(VS_IN In)
{
	VS_OUT_SOFTEFFECT		Out = (VS_OUT_SOFTEFFECT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* ������ ��ġ�� ���� �� ��������� ���Ѵ�. ���� ������ ViewSpace�� �����ϳ�. */
	/* ������ı��� ���ϸ� ������ġ�� w�� �佺���̽� ���� z�� �����Ѵ�. == Out.vPosition�� �ݵ�� float4�̾���ϴ� ����. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_IN_SOFTEFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
};

/* �̷��� ������� �ȼ��� PS_MAIN�Լ��� ���ڷ� ������. */
/* �����ϴ� ���� Target0 == ��ġ�� 0��°�� ���ε��Ǿ��ִ� ����Ÿ��(�Ϲ������� �����)�� �׸���. */
/* �׷��� ����ۿ� ���� �׷�����. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;
	Out.vDiffuse.rgb = g_ColorFront.rgb;

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.01f)
		discard;

	return Out;
}


PS_OUT PS_MAIN_SOFTEFFECT(PS_IN_SOFTEFFECT In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.rgb = g_ColorFront.rgb;

	float2	vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(LinearSampler, vUV);

	float	fViewZ = In.vProjPos.w;
	float	fOldViewZ = vDepthDesc.y*500.5f;

	Out.vDiffuse.a = Out.vDiffuse.a * (fOldViewZ - fViewZ);

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.00f)
		discard;


	return Out;
}

PS_OUT PS_HITFLASH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	vector OrangeColor = vector(255, 191, 95, 255) /255.f;
	vector BrownColor = vector(127, 95, 28, 255) /255.f;

	Out.vDiffuse.rgb = BrownColor.rgb * (1 - Out.vDiffuse.r) + OrangeColor.rgb * Out.vDiffuse.r;
//	Out.vDiffuse.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_HITFLASH2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	vector BrownColor = vector(127, 95, 28, 255) / 255.f;
	vector OrangeColor = vector(255, 191, 95, 255) / 255.f;
	vector YellowColor = vector(255, 255, 100, 255) / 255.f;
	vector LightOrangeColor = vector(255, 230, 150, 255) / 255.f;

	Out.vDiffuse.rgb = YellowColor.rgb * (1 - Out.vDiffuse.r) + LightOrangeColor.rgb * Out.vDiffuse.r;
	//Out.vDiffuse.a *= g_fAlpha;


	return Out;
}

PS_OUT PS_HITFLASH3(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	vector WhiteColor = vector(255, 255, 200, 255) / 255.f;
	vector OrangeColor = vector(255, 191, 95, 255) / 255.f;
	vector YellowColor = vector(255, 255, 100, 255) / 255.f;
	vector LightOrangeColor = vector(255, 230, 150, 255) / 255.f;

	Out.vDiffuse.rgb = YellowColor.rgb * (1 - Out.vDiffuse.r) + WhiteColor.rgb * Out.vDiffuse.r;
	//Out.vDiffuse.a *= g_fAlpha;


	return Out;
}

PS_OUT PS_TWOCOLOR_NOTALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vector GetColorBack = g_ColorBack / 255.f;
	vector GetColorFront = g_ColorFront / 255.f;
	
	Out.vDiffuse.rgb = GetColorBack.rgb * (1 - Out.vDiffuse.r) + GetColorFront.rgb * Out.vDiffuse.r;
	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.0f)
		discard;

	return Out;
}


PS_OUT PS_TWOCOLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	vector GetColorBack = g_ColorBack / 255.f;
	vector GetColorFront = g_ColorFront / 255.f;

	Out.vDiffuse.rgb = GetColorBack.rgb * (1 - Out.vDiffuse.r) + GetColorFront.rgb * Out.vDiffuse.r;

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.0f)
		discard;

	return Out;
}


PS_OUT PS_SMOKEBACKBLACK(PS_IN_SOFTEFFECT In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	vector BlackColor = vector(20, 3, 17, 255) / 255.f;
	vector PinkColor = vector(176, 21, 184, 255) / 255.f;

	Out.vDiffuse.rgb = PinkColor.rgb * (1 - Out.vDiffuse.r) + BlackColor.rgb * Out.vDiffuse.r;

	float2	vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(LinearSampler, vUV);

	float	fViewZ = In.vProjPos.w;
	float	fOldViewZ = vDepthDesc.y*500.f;

	Out.vDiffuse.a = Out.vDiffuse.a * (fOldViewZ - fViewZ);

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.0f)
		discard;


	return Out;
}

PS_OUT PS_SMOKEFRONT_PURPLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vSmokeDst =  g_SmokeDstTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse = vDiffuse*vSmokeDst;
	Out.vDiffuse.a = vDiffuse.r*vSmokeDst.r;

	if (Out.vDiffuse.a <= 0.0f)
		discard;

	vector FrontColor = g_ColorFront / 255.f;
	vector BackColor = g_ColorBack / 255.f;
	Out.vDiffuse.rgb = FrontColor.rgb;
	vSmokeDst.rgb = BackColor.rgb;

	Out.vDiffuse.rgb *= vSmokeDst.rgb;

	Out.vDiffuse.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_ONLY_TEXTURE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;

	vector GetColorBack = g_ColorBack / 255.f;
	vector GetColorFront = g_ColorFront / 255.f;

	Out.vDiffuse.rgb = GetColorBack.rgb * (1 - Out.vDiffuse.r) + GetColorFront.rgb * Out.vDiffuse.r;


	Out.vDiffuse.a *= g_fAlpha;
	if (Out.vDiffuse.a <= 0.0f)
		discard;

	//Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass OneColor_NotAlphaSet
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SOFTEFFECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SOFTEFFECT();
	}

	pass OneColor_AlphaSet
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass OneColor_AlphaSet_Pri
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
	pass HitFlashEffect
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HITFLASH();
	}

	pass HitFlashEffect2
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HITFLASH2();
	}

	pass HitFlashEffect3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HITFLASH3();
	}

	pass TwoColor_not_AlphaSet
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TWOCOLOR_NOTALPHA();
	}

	pass TwoColor_Priority
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TWOCOLOR();
	}

	pass TwoColor_DepthDefault
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TWOCOLOR();
	}

	pass SmokeBackBlack
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SOFTEFFECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SMOKEBACKBLACK();
	}

	pass SmokeFrontPurple
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SMOKEFRONT_PURPLE();
	}

	pass Only_Texture
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ONLY_TEXTURE();
	}

	pass Trail
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAIL();
	}
}