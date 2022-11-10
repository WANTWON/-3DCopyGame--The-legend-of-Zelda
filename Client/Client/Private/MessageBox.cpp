#include "stdafx.h"
#include "..\Public\MessageBox.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CMessageBox::CMessageBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_UI(pDevice, pContext)
{
}

CMessageBox::CMessageBox(const CMessageBox & rhs)
	: CObj_UI(rhs)
{
}

HRESULT CMessageBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMessageBox::Initialize(void * pArg)
{

	m_fSize.x = 1166/ 1.5f;
	m_fSize.y = 365/ 1.5f;


	m_fPosition.x = g_iWinSizeX * 0.5f;
	m_fPosition.y = 600;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eShaderID = UI_ALPHASET;

	return S_OK;
}

int CMessageBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

void CMessageBox::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI_FRONT, this);

	m_iTextureNum = CUI_Manager::Get_Instance()->Get_MessageType();
	if (CUI_Manager::Get_Instance()->Get_OpenMessage())
	{
		m_fAlpha += 0.1f;
		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
		m_bRender = true;
	}
	else
	{
		m_fAlpha -= 0.1f;

		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			m_bRender = false;
		}
			
	}

}

HRESULT CMessageBox::Render()
{
	if (!m_bRender)
		return E_FAIL;
	__super::Render();

	return S_OK;
}

HRESULT CMessageBox::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_GetMessage"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;

}


HRESULT CMessageBox::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iTextureNum))))
		return E_FAIL;

	return S_OK;
}


CMessageBox * CMessageBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMessageBox*	pInstance = new CMessageBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMessageBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMessageBox::Clone(void * pArg)
{
	CMessageBox*	pInstance = new CMessageBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMessageBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMessageBox::Free()
{
	__super::Free();
}