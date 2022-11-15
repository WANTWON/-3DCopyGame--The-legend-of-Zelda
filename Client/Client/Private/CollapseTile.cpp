#include "stdafx.h"
#include "..\Public\CollapseTile.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"

CCollapseTile::CCollapseTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNonAnim(pDevice, pContext)
{
}

CCollapseTile::CCollapseTile(const CCollapseTile & rhs)
	: CNonAnim(rhs)
{
}

HRESULT CCollapseTile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollapseTile::Initialize(void * pArg)
{
	
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	m_eObjectID = OBJ_KEY;

	if (pArg != nullptr)
	{
		_vector vecPostion = XMLoadFloat3((_float3*)pArg);
		vecPostion = XMVectorSetW(vecPostion, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	}

	Set_Scale(_float3(3.f, 3.f, 3.f));
	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_ITEM, this);

	return S_OK;
}

int CCollapseTile::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_ITEM, this);
		return OBJ_DEAD;
	}
	

	return OBJ_NOEVENT;
}

void CCollapseTile::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (m_pOBBCom != nullptr && m_pOBBCom->Collision(pTarget->Get_Collider()))
	{
		m_fAlpha -= 0.05f;

		if (m_fAlpha <= 0.f)
			m_bDead = true;	
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCollapseTile::Render()
{
	__super::Render();

	return S_OK;

}

HRESULT CCollapseTile::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 4.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_CollapseTile"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(0.5f, 0.2f, 0.5f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}


CCollapseTile * CCollapseTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCollapseTile*	pInstance = new CCollapseTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollapseTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCollapseTile::Clone(void * pArg)
{
	CCollapseTile*	pInstance = new CCollapseTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollapseTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollapseTile::Free()
{
	__super::Free();

}
