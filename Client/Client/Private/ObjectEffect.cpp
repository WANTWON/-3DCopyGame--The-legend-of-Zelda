#include "stdafx.h"
#include "..\Public\ObjectEffect.h"
#include "Weapon.h"
#include "Player.h"
#include "GameInstance.h"


CObjectEffect::CObjectEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CObjectEffect::CObjectEffect(const CObjectEffect & rhs)
	: CEffect(rhs)
{
}

HRESULT CObjectEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CObjectEffect::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_Scale(m_EffectDesc.vInitScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vInitPositon);
	m_pTransformCom->LookDir(m_EffectDesc.vLook);
	m_vScale = m_EffectDesc.vInitScale;


	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
	case LAWN:
		m_pTransformCom->Rotation(XMVectorSet((rand() % 10 + 1)*0.1f, (rand() % 10)*0.1f, (rand() % 10)*0.1f, 0.f), XMConvertToRadians(float(rand() % 180)));
		m_eShaderID = SHADERM_ONLY_TEXTURE;
		break;
	case GRASS_TEX:
		m_eShaderID = SHADER_TWOCOLOR_NOT_ALPHASET;
		m_vColorFront = XMVectorSet(80, 153, 36, 255);
		m_vColorBack = XMVectorSet(17, 82, 34, 255);
		m_vDirection = _float3((rand() % 20 - 10) * 0.1f, 1.f, (rand() % 20 - 10)* 0.1f);
		m_fAngle = XMConvertToRadians(float(rand() % 180));
		break;
	case ITEM_GET_FLASH:
		m_eShaderID = SHADER_TWOCOLOR_PRIORITY;
		m_vecColor.push_back(XMVectorSet(255.f, 63.f, 63.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 127.f, 0.f, 255.f));
		m_vecColor.push_back(XMVectorSet(0.f, 255.f, 0.f, 255.f));
		m_vecColor.push_back(XMVectorSet(63.f, 127.f, 255.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 127.f, 255.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 63.f, 63.f, 255.f));
		m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		m_vColorBack = m_vecColor.front();
		break;
	case ITEM_GET_GLOW:
		m_eShaderID = SHADER_TWOCOLOR_DEFAULT;

		if (m_EffectDesc.fStartTime != 0.f)
		{
			m_vColorBack = XMVectorSet(63, 127, 255, 255.f);
			m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		}
		else
		{
			m_vecColor.push_back(XMVectorSet(255.f, 191.f, 191.f, 255.f));
			m_vecColor.push_back(XMVectorSet(255.f, 255.f, 127.f, 255.f));
			m_vecColor.push_back(XMVectorSet(127.f, 255.f, 127.f, 255.f));
			m_vecColor.push_back(XMVectorSet(191.f, 255.f, 255.f, 255.f));
			m_vecColor.push_back(XMVectorSet(255.f, 191.f, 255.f, 255.f));
			m_vecColor.push_back(XMVectorSet(255.f, 191.f, 191.f, 255.f));
			m_vColorBack = m_vecColor.front();
			m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		}
		break;
	case HORIZONTAL_GLOW:
		m_fAlpha = 0.f;
		m_vMaxScale = _float3(15.f, 4.f, 0.f);
		m_eShaderID = SHADER_TWOCOLOR_DEFAULT;
		m_vecColor.push_back(XMVectorSet(255.f, 63.f, 63.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 127.f, 0.f, 255.f));
		m_vecColor.push_back(XMVectorSet(0.f, 255.f, 0.f, 255.f));
		m_vecColor.push_back(XMVectorSet(63.f, 127.f, 255.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 127.f, 255.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 63.f, 63.f, 255.f));
		m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		m_vColorBack = m_vecColor.front();
		break;
	case RAINBOW_RING:
		m_eShaderID = SHADERM_ONLY_TEXTURE;
		break;
	case RAINBOW_HALO:
		m_fAlpha = 0.f;
		m_eShaderID = SHADERM_ONLY_TEXTURE;
		m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		m_vColorBack = XMVectorSet(63, 191.f, 228.f, 255.f);
		break;
	case GRAD_RING:
		m_eShaderID = SHADERM_TWOCOLOR_DEFAULT;
		m_vecColor.push_back(XMVectorSet(255.f, 63.f, 63.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 127.f, 0.f, 255.f));
		m_vecColor.push_back(XMVectorSet(0.f, 255.f, 0.f, 255.f));
		m_vecColor.push_back(XMVectorSet(63.f, 127.f, 255.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 127.f, 255.f, 255.f));
		m_vecColor.push_back(XMVectorSet(255.f, 63.f, 63.f, 255.f));
		m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		m_vColorBack = m_vecColor.front();

		break;

	case GLITTER:
	{
		m_eShaderID = SHADERM_TWOCOLOR_DEFAULT;
		m_vColorFront = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		_int iRandNum = rand() % 6 + 1;
		if (iRandNum == 1) m_vColorBack = XMVectorSet(255.f, 63.f, 63.f, 255.f);
		else if (iRandNum == 2) m_vColorBack = XMVectorSet(255.f, 127.f, 0.f, 255.f);
		else if (iRandNum == 3)	m_vColorBack = XMVectorSet(0.f, 255.f, 0.f, 255.f);
		else if (iRandNum == 4)	m_vColorBack = XMVectorSet(63.f, 127.f, 255.f, 255.f);
		else if (iRandNum == 5) m_vColorBack = XMVectorSet(255.f, 63.f, 63.f, 255.f);
		else  m_vColorBack = XMVectorSet(228.f, 226.f, 228.f, 255.f);
		m_fSpeed = (rand() % 3 + 1)* 0.1f;
		m_vMaxScale = m_EffectDesc.vInitScale;
		break;
	}
	case RAY:
		m_eShaderID = SHADER_ONLY_TEXTURE;
		m_fAlpha = 0.f;
		break;
	default:
		break;
	}


	return S_OK;
}

int CObjectEffect::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		return OBJ_DEAD;
	}

	__super::Tick(fTimeDelta);


	if (m_bEndMode)
	{
		m_vScale.x -= 0.05f;
		m_vScale.y -= 0.05f;
		m_vScale.z -= 0.05f;
		m_fAlpha -= 0.05f;

		if (m_fAlpha < 0.f || m_vScale.x <= 0.f)
			m_bDead = true;

		return OBJ_NOEVENT;
	}


	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
	case LAWN:
	case GRASS_TEX:
		Tick_Grass(fTimeDelta);
		break;
	case ITEM_GET_GLOW:
		Tick_GlowEffect(fTimeDelta);
		break;
	case ITEM_GET_FLASH:
		Tick_FlashEffect(fTimeDelta);
		break;
	case HORIZONTAL_GLOW:
		Tick_HorizontalGlowEffect(fTimeDelta);
		break;
	case RAINBOW_RING:
	case GRAD_RING:
		Tick_RingEffect(fTimeDelta);
		break;
	case RAINBOW_HALO:
		Tick_HaloEffect(fTimeDelta);
		break;
	case GLITTER:
		Tick_GlitterEffect(fTimeDelta);
		break;
	case RAY:
		Tick_Ray(fTimeDelta);
		break;
	default:
		break;
	}

	return OBJ_NOEVENT;
}

void CObjectEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);



}

HRESULT CObjectEffect::Render()
{

	__super::Render();


	return S_OK;
}

HRESULT CObjectEffect::Ready_Components(void * pArg)
{
	LEVEL iLevel = (LEVEL)CGameInstance::Get_Instance()->Get_DestinationLevelIndex();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* For.Com_Model*/
	switch (m_EffectDesc.eEffectType)
	{
	case MODEL:
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Model"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case VIBUFFER_RECT:
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		break;
	default:
		break;
	}


	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_GrassLeaf"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case LAWN:
		/* For.Com_Model*/
		if (m_EffectDesc.iTextureNum == 0)
		{
			if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_LawnLeaf_0"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_LawnLeaf_1"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		break;
	case GRASS_TEX:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case ITEM_GET_FLASH:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Flash"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case ITEM_GET_GLOW:
	case HORIZONTAL_GLOW:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Glow"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case RAINBOW_RING:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_RainbowRing"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case RAINBOW_HALO:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_RainbowHalo"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case GRAD_RING:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_GradRing"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case GLITTER:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Glow"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case RAY:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ray"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CObjectEffect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	if (m_EffectDesc.eEffectType == MODEL)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_TexUV", &m_fTexUV, sizeof(_float))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_EffectDesc.iTextureNum))))
			return E_FAIL;

		if (FAILED(pGameInstance->Bind_RenderTarget_SRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color", &m_vColorBack, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ColorFront", &m_vColorFront, sizeof(_vector))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CObjectEffect::SetUp_ShaderID()
{
	return S_OK;
}

void CObjectEffect::Change_Animation(_float fTimeDelta)
{
}

void CObjectEffect::Change_Texture(_float fTimeDelta)
{

}


void CObjectEffect::Tick_Grass(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
	case LAWN:
		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
			m_fAlpha -= 0.05f;

		m_pTransformCom->Go_PosDir(fTimeDelta*0.5f, m_EffectDesc.vLook);
		break;
	case GRASS_TEX:
	{
		m_pTransformCom->Go_PosDir(fTimeDelta, XMLoadFloat3(&m_vDirection));
		SetUp_BillBoard();
		m_pTransformCom->Rotation(Get_TransformState(CTransform::STATE_LOOK), m_fAngle);

		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
			m_fAlpha -= 0.05f;
		break;
	}
	default:
		break;
	}



	if (m_fAlpha <= 0)
		m_bDead = true;

}

void CObjectEffect::Tick_GlowEffect(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;
	m_fColorTime += fTimeDelta;

	SetUp_BillBoard();

	if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + m_EffectDesc.vDistance);

	if (m_EffectDesc.fStartTime == 0)
	{
		if (m_fColorTime > m_EffectDesc.fDeadTime*0.3f)
		{
			if (m_iColorIndex + 1 >= m_vecColor.size() - 1)
				m_iColorIndex = 0/*m_vecColor.size() - 2*/;
			else
				m_iColorIndex++;
				m_fColorTime = 0.f;
		}


		m_vColorBack = XMVectorLerp(m_vecColor[m_iColorIndex], m_vecColor[m_iColorIndex + 1], m_fColorTime / 0.3f);

	}

	if (m_fDeadtime < m_EffectDesc.fDeadTime*0.3f && m_fDeadtime >= m_EffectDesc.fStartTime)
	{
		m_vScale.x += 0.04f;
		m_vScale.y += 0.04f;
	}
	else if (m_fDeadtime < m_EffectDesc.fDeadTime*0.6f)
	{
		m_vScale.x += 0.01f;
		m_vScale.y += 0.01f;
	}
	Set_Scale(m_vScale);

}

void CObjectEffect::Tick_FlashEffect(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;
	m_fColorTime += fTimeDelta;

	SetUp_BillBoard();

	if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + m_EffectDesc.vDistance);

	if (m_EffectDesc.fStartTime == 0)
	{
		if (m_fColorTime > m_EffectDesc.fDeadTime*0.3f)
		{
			if (m_iColorIndex + 1 >= m_vecColor.size() - 1)
				m_iColorIndex = 0;
			else
				m_iColorIndex++;

			m_fColorTime = 0.f;
		}


		m_vColorBack = XMVectorLerp(m_vecColor[m_iColorIndex], m_vecColor[m_iColorIndex + 1], m_fColorTime / 0.3f);

	}

	if (m_fDeadtime < m_EffectDesc.fDeadTime)
	{
		m_vScale.x += 0.05f;
		m_vScale.y += 0.05f;
	}
	else
	{
		m_vScale.x -= 0.2f;
		m_vScale.y -= 0.2f;
		m_fAlpha -= 0.05f;

		if (m_fAlpha <= 0.f || m_vScale.x <= 0.f)
		{
			CEffect::EFFECTDESC EffectDesc;

			EffectDesc.vInitPositon = Get_TransformState(CTransform::STATE_POSITION);// +XMVectorSet(0.f, Get_Scale().y - 0.4f, 0.f, 0.f);
			EffectDesc.pTarget = m_EffectDesc.pTarget;
			EffectDesc.eEffectType = CEffect::VIBUFFER_RECT;
			for (int i = 0; i < 10; ++i)
			{
				
				EffectDesc.eEffectID = CObjectEffect::GLITTER;
				_float fRandNum = (rand() % 5 + 1) * 0.1f;
				EffectDesc.vInitScale = _float3(fRandNum, fRandNum, 0.0f);
				EffectDesc.fDeadTime = 1.5f;
				EffectDesc.iTextureNum = 1;
				EffectDesc.vLook = XMVectorSet((rand() % 20 - 10) * 0.1f, (rand() % 20 - 10)*0.1f, 0.f, 0.f);
				CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_ObjectEffect"), LEVEL_STATIC, TEXT("Layer_ObjectEffect"), &EffectDesc);
			}

			/*for (int i = 0; i < 4; ++i)
			{
				EffectDesc.vDistance = (rand()%2 == 0) ? XMVectorSetX(EffectDesc.vDistance, XMVectorGetX(EffectDesc.vDistance) + 1.f)
					: XMVectorSetX(EffectDesc.vDistance, XMVectorGetX(EffectDesc.vDistance) -1.f);
				
				EffectDesc.vDistance = (rand() % 2 == 0) ? XMVectorSetY(EffectDesc.vDistance, XMVectorGetY(EffectDesc.vDistance) + 1.f)
					: XMVectorSetY(EffectDesc.vDistance, XMVectorGetY(EffectDesc.vDistance) -1.f);

				EffectDesc.eEffectID = CObjectEffect::RAY;
				_float fRandNum = (rand() % 5 + 1) * 0.1f;
				EffectDesc.vInitScale = _float3(fRandNum, fRandNum, 0.0f);
				EffectDesc.fDeadTime = (rand() % 3 + 1) * 0.1f;
				EffectDesc.iTextureNum = 0;
				CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_ObjectEffect"), LEVEL_STATIC, TEXT("Layer_ObjectEffect"), &EffectDesc);
			}*/

			m_bDead = true;
		}
			
	}



	

	Set_Scale(m_vScale);
}

void CObjectEffect::Tick_HorizontalGlowEffect(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;
	m_fColorTime += fTimeDelta;

	SetUp_BillBoard();

	if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + m_EffectDesc.vDistance);

	
		if (m_fColorTime > m_EffectDesc.fDeadTime*0.3f)
		{
			if (m_iColorIndex + 1 >= m_vecColor.size() - 1)
				m_iColorIndex = 0;
			else
				m_iColorIndex++;

			m_fColorTime = 0.f;
		}


		m_vColorBack = XMVectorLerp(m_vecColor[m_iColorIndex], m_vecColor[m_iColorIndex + 1], m_fColorTime / 0.3f);

	

	if (!m_bMax)
	{
		m_fAlpha += 0.1f;
		m_vScale.x += 0.3f;
		m_vScale.y += 0.1f;

		if (m_vScale.x >= m_vMaxScale.x)
		{
			m_vScale.x = m_vMaxScale.x;
			m_bMax = true;
		}	
		if (m_vScale.y >= m_vMaxScale.y)
			m_vScale.y = m_vMaxScale.y;

		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
	}
	else
	{
		m_vScale.y -= 0.02f;
		m_fAlpha -= 0.05f;

		if (m_fAlpha < 0.f || m_vScale.x < 0.f)
			m_bDead = true;
	}

	Set_Scale(m_vScale);
}

void CObjectEffect::Tick_RingEffect(_float fTimeDelta)
{
	SetUp_BillBoard();

	m_fDeadtime += fTimeDelta;
	m_fColorTime += fTimeDelta;

	if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + m_EffectDesc.vDistance);


	if (m_vecColor.size() != 0)
	{
		if (m_fColorTime > m_EffectDesc.fDeadTime*0.3f)
		{
			if (m_iColorIndex + 1 >= m_vecColor.size() - 1)
				m_iColorIndex = 0;
			else
				m_iColorIndex++;

			m_fColorTime = 0.f;
		}

		m_vColorBack = XMVectorLerp(m_vecColor[m_iColorIndex], m_vecColor[m_iColorIndex + 1], m_fColorTime / 0.3f);

	}


	if (m_fDeadtime < m_EffectDesc.fDeadTime*0.5  && m_fDeadtime > m_EffectDesc.fStartTime)
	{
		m_fAlpha += 0.1f;
		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
		m_vScale.x += 0.05f;
		m_vScale.y += 0.05f;
		m_vScale.z += 0.05f;
	}
	else if (m_fDeadtime < m_EffectDesc.fDeadTime)
	{
		m_vScale.x += 0.025f;
		m_vScale.y += 0.025f;
		m_vScale.z += 0.025f;

	}
	else
	{
		m_vScale.x += 0.01f;
		m_vScale.y += 0.01f;
		m_vScale.z += 0.01f;
		m_fAlpha -= 0.025f;
	}

	Set_Scale(m_vScale);

	if (m_fAlpha <= 0)
		m_bDead = true;
}

void CObjectEffect::Tick_HaloEffect(_float fTimeDelta)
{
	SetUp_BillBoard();

	m_fDeadtime += fTimeDelta;
	m_fColorTime += fTimeDelta;

	if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + m_EffectDesc.vDistance);


	if (!m_bMax)
	{
		m_fAlpha += 0.002f;
		if (m_fAlpha >= 0.5f)
		{
			m_fAlpha = 0.5f;
			m_bMax = true;
		}
			

	}
	else if(m_fDeadtime > m_EffectDesc.fDeadTime)
	{
		m_fAlpha -= 0.002f;

		if (m_fAlpha <= 0)
			m_bDead = true;
	}


	
}

void CObjectEffect::Tick_GlitterEffect(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	SetUp_BillBoard();

	m_pTransformCom->Go_PosDir(fTimeDelta*m_fSpeed, m_EffectDesc.vLook);

	if (m_fDeadtime < m_EffectDesc.fDeadTime*0.2f)
	{
		m_vScale.x += 0.05f;
		m_vScale.y += 0.05f;

		if (m_vScale.x >= m_vMaxScale.x)
		{
			m_vScale = m_vMaxScale;
		}
	}
	else if (m_fDeadtime < m_EffectDesc.fDeadTime*0.4f)
	{
		m_vScale.x -= 0.05f;
		m_vScale.y -= 0.05f;
	}
	else if (m_fDeadtime < m_EffectDesc.fDeadTime*0.6f)
	{
		m_vScale.x += 0.05f;
		m_vScale.y += 0.05f;

		if (m_vScale.x >= m_vMaxScale.x)
		{
			m_vScale = m_vMaxScale;
		}
	}
	else if (m_fDeadtime < m_EffectDesc.fDeadTime*0.8f)
	{
		m_vScale.x -= 0.05f;
		m_vScale.y -= 0.05f;
	}
	else if (m_fDeadtime < m_EffectDesc.fDeadTime)
	{
		m_vScale.x += 0.05f;
		m_vScale.y += 0.05f;

		if (m_vScale.x >= m_vMaxScale.x)
		{
			m_vScale = m_vMaxScale;
		}
	}
	else
	{
		m_vScale.x -= 0.05f;
		m_vScale.y -= 0.05f;
		m_fAlpha -= 0.25f;
	}

	Set_Scale(m_vScale);

	if (m_fAlpha <= 0.f)
		m_bDead = true;
}

void CObjectEffect::Tick_Ray(_float fTimeDelta)
{
	if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + m_EffectDesc.vDistance);
	}

	if (m_fDeadtime < m_EffectDesc.fDeadTime)
	{
		m_vScale.y += 0.01f;
		m_fAlpha += 0.01f;

		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
	}
	else
	{
		m_vScale.y += 0.01f;
		m_fAlpha -= 0.01f;
	}

	Set_Scale(m_vScale);

	if (m_fAlpha <= 0.f)
		m_bDead = true;
	
}


CObjectEffect * CObjectEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CObjectEffect*	pInstance = new CObjectEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CObjectEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CObjectEffect::Clone(void * pArg)
{
	CObjectEffect*	pInstance = new CObjectEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CObjectEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObjectEffect::Free()
{
	__super::Free();
}