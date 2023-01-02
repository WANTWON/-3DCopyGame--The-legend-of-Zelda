
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_NoiseTexture;
texture2D		g_AlphaTexture;

float			g_fAlpha = 1.f;


vector			g_ColorBack = vector(1.f, 1.f, 1.f, 1.f);
vector			g_ColorFront = vector(1.f, 1.f, 1.f, 1.f);

cbuffer NoiseBuffer
{
	float			frameTime= 0.f;
	float3			scrollSpeeds = float3(1.3f, 2.1f, 2.3f);
	float3			scales = float3(1.0f, 2.0f, 3.0f);
	float			padding = 0.f;
};


cbuffer DistortionBuffer
{
	float2 distortion1= float2(0.1f, 0.2f);
	float2 distortion2= float2(0.1f, 0.3f);
	float2 distortion3= float2(0.1f, 0.1f);
	float  distortionScale = 0.8f;
	float  distortionBias = 0.5f;
};

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

	float2 texCoords1 : TEXCOORD2;
	float2 texCoords2 : TEXCOORD3;
	float2 texCoords3 : TEXCOORD4;
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

	// ù��° ������ �ؽ����� ��ǥ�� ù��° ũ�� �� ������ ��ũ�� �ӵ� ���� �̿��Ͽ� ����մϴ�.
	Out.texCoords1 = (In.vTexUV * scales.x);
	Out.texCoords1.y = Out.texCoords1.y + (frameTime * scrollSpeeds.x);

	// �ι�° ������ �ؽ����� ��ǥ�� �ι�° ũ�� �� ������ ��ũ�� �ӵ� ���� �̿��Ͽ� ����մϴ�.
	Out.texCoords2 = (In.vTexUV * scales.y);
	Out.texCoords2.y = Out.texCoords2.y + (frameTime * scrollSpeeds.y);

	// ����° ������ �ؽ����� ��ǥ�� ����° ũ�� �� ������ ��ũ�� �ӵ� ���� �̿��Ͽ� ����մϴ�.
	Out.texCoords3 = (In.vTexUV * scales.z);
	Out.texCoords3.y = Out.texCoords3.y + (frameTime * scrollSpeeds.z);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;

	float2 texCoords1 : TEXCOORD2;
	float2 texCoords2 : TEXCOORD3;
	float2 texCoords3 : TEXCOORD4;
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

	// ������ ������ �ؽ��ĸ� ���� �ٸ� �� �ؽ��� ��ǥ�� ����Ͽ� �� ���� �ٸ� ũ���� ����� ����ϴ�.
	float4 noise1 = g_NoiseTexture.Sample(LinearSampler, In.texCoords1);
	float4 noise2 = g_NoiseTexture.Sample(LinearSampler, In.texCoords2);
	float4 noise3 = g_NoiseTexture.Sample(LinearSampler, In.texCoords3);

	// ������ ���� ������ (0, 1)���� (-1, +1)�� �ǵ��� �մϴ�.
	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// �������� x�� y���� �� ���� �ٸ� �ְ� x�� y��ǥ�� ��߸��ϴ�.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// �ְ�� �� ������ ������ �ϳ��� ������� �Լ��մϴ�.
	float4 finalNoise = noise1 + noise2 + noise3;

	// �Է����� ���� �ؽ����� Y��ǥ�� �ְ� ũ��� ���̾ ������ ������ŵ�ϴ�.
	// �� ������ �ؽ����� �������� ������ �������� �� ���ʿ��� �����̴� ȿ���� �������ϴ�.
	float  perturb = ((1.0f - In.vTexUV.y) * distortionScale) + distortionBias;

	// �Ҳ� ���� �ؽ��ĸ� ���ø��ϴµ� ���� �ְ� �� ������ �ؽ��� ��ǥ�� ����ϴ�.
	float2 noiseCoords = (finalNoise.xy * perturb) + In.vTexUV;


	// �ְ�ǰ� ������ �ؽ��� ��ǥ�� �̿��Ͽ� �Ҳ� �ؽ��Ŀ��� ������ ���ø��մϴ�.
	// wrap�� ����ϴ� ������Ʈ ��� clamp�� ����ϴ� ������Ʈ�� ����Ͽ� �Ҳ� �ؽ��İ� ���εǴ� ���� �����մϴ�.
	float4 fireColor = g_DiffuseTexture.Sample(FireSampler, noiseCoords.xy);
	vector GetColorBack = g_ColorBack / 255.f;
	vector GetColorFront = g_ColorFront / 255.f;
	fireColor.rgb = GetColorBack.rgb * (1 - fireColor.r) + GetColorFront.rgb * fireColor.r;

	// �ְ�ǰ� ������ �ؽ��� ��ǥ�� �̿��Ͽ� ���� �ؽ��Ŀ��� ���İ��� ���ø��մϴ�.
	// �Ҳ��� ������ �����ϴ� �� ���� ���Դϴ�.
	// wrap�� ����ϴ� ������Ʈ ��� clamp�� ����ϴ� ������Ʈ�� ����Ͽ� �Ҳ� �ؽ��İ� ���εǴ� ���� �����մϴ�.
	float4 alphaColor = g_AlphaTexture.Sample(FireSampler, noiseCoords.xy);

	// �ְ� �� ������ ���� �ؽ��� ���� ���ĺ����� ����մϴ�.
	fireColor.a = alphaColor.r;

	Out.vDiffuse = fireColor;
	if (Out.vDiffuse.a <= 0.0f)
		discard;

	return Out;
}



technique11 DefaultTechnique
{
	pass Fire
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
}