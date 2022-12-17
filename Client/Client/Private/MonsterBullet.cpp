#include "stdafx.h"
#include "..\Public\MonsterBullet.h"
#include "ObjectEffect.h"

CMonsterBullet::CMonsterBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CMonsterBullet::CMonsterBullet(const CMonsterBullet & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CMonsterBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBullet::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_BulletDesc, pArg, sizeof(BULLETDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	switch (m_BulletDesc.eBulletType)
	{
	case DEFAULT:
		Set_Scale(_float3(2, 1, 2));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_BulletDesc.vInitPositon);
		break;
	case OCTOROCK:
		Set_Scale(_float3(2, 2, 2));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_BulletDesc.vInitPositon);
		m_pTransformCom->LookDir(m_BulletDesc.vLook);
		break;
	case ROLA:
	{
		Set_Scale(_float3(3, 3, 3));
		_float PosY = XMVectorGetY(m_BulletDesc.vInitPositon);
		//PosY += 2.f;
		//m_BulletDesc.vInitPositon = XMVectorSetY(m_BulletDesc.vInitPositon, PosY);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_BulletDesc.vInitPositon);
		m_pTransformCom->LookDir(m_BulletDesc.vLook);
		break;
	}
	case ALBATOSS:
	{
		Set_Scale(_float3(3, 3, 3));
		m_fSpeed = (rand() % 500)*0.01f + 2;
		m_BulletDesc.vLook += XMVectorSet(0.f, (rand() % 50)*-0.01f, 0.f, 0.f);
		_float PosY = XMVectorGetY(m_BulletDesc.vInitPositon);
		PosY += 2.f;
		m_BulletDesc.vInitPositon = XMVectorSetY(m_BulletDesc.vInitPositon, PosY);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_BulletDesc.vInitPositon);
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(float(rand() % 180)));
		break;
	}
	default:
		break;
	}

	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_MBULLET, this);
	return S_OK;
}

int CMonsterBullet::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_MBULLET, this);
		return OBJ_DEAD;
	}
		

	__super::Tick(fTimeDelta);

	switch (m_BulletDesc.eBulletType)
	{
	case DEFAULT:
		Moving_DefaultBullet(fTimeDelta);
		break;
	case OCTOROCK:
		Moving_OctorockBullet(fTimeDelta);
		break;
	case ROLA:
		Moving_RolaBullet(fTimeDelta);
		break;
	case ALBATOSS:
		Moving_AlbatossBullet(fTimeDelta);
		break;
	default:
		break;
	}


	return OBJ_NOEVENT;
}

void CMonsterBullet::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	SetUp_ShaderID();

	switch (m_BulletDesc.eBulletType)
	{
	case OCTOROCK:
		LateTick_Octorock(fTimeDelta);
		break;
	default:
		break;
	}
}

HRESULT CMonsterBullet::Render()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderID)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CMonsterBullet::Ready_Components(void * pArg)
{
	LEVEL iLevel = (LEVEL)CGameInstance::Get_Instance()->Get_DestinationLevelIndex();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	if(m_BulletDesc.eBulletType == OCTOROCK)
		TransformDesc.fSpeedPerSec = 5.0f;
	else
		TransformDesc.fSpeedPerSec = 2.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_Model*/
	switch (m_BulletDesc.eBulletType)
	{
	case OCTOROCK:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), iLevel, TEXT("Prototype_Component_Model_OctorockBullet"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
		ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), iLevel, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
			return E_FAIL;

		break;
	case ROLA:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), iLevel, TEXT("Prototype_Component_Model_RolaBullet"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(5.f, 0.2f, 0.2f);
		ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
		ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), iLevel, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;

		break;
	case ALBATOSS:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), iLevel, TEXT("Albatoss_Feather"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
		ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), iLevel, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
			return E_FAIL;
		break;
	default:
		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
		ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), iLevel, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
			return E_FAIL;
		break;
	}



	return S_OK;
}

HRESULT CMonsterBullet::SetUp_ShaderResources()
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


	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMonsterBullet::SetUp_ShaderID()
{
	return S_OK;
}

void CMonsterBullet::Change_Animation(_float fTimeDelta)
{
}


void CMonsterBullet::Moving_DefaultBullet(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	if (m_BulletDesc.fDeadTime < m_fDeadtime)
		m_bDead = true;
}

void CMonsterBullet::Moving_OctorockBullet(_float fTimeDelta)
{

	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CMonsterBullet::Moving_RolaBullet(_float fTimeDelta)
{
	_vector vAxis = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	/*_float Y = XMVectorGetY(vAxis);
	Y += 1;
	vAxis = XMVectorSetY(vAxis, Y);*/

	m_pTransformCom->Turn(vAxis, 10.f);
	m_pTransformCom->Go_PosDir(fTimeDelta*2, m_BulletDesc.vLook);
}

void CMonsterBullet::Moving_AlbatossBullet(_float fTimeDelta)
{

	if (XMVectorGetY(Get_TransformState(CTransform::STATE_POSITION)) < 16.f)
	{
		m_fDeadtime += fTimeDelta;
		if (m_fDeadtime >= 1.f)
		{
			m_fAlpha -= 0.05f;

			if (m_fAlpha <= 0.f)
				m_bDead = true;
		}
	}
	else
	{
		m_pTransformCom->Go_PosDir(fTimeDelta*m_fSpeed, m_BulletDesc.vLook);
	}

	m_fSpeed -= 0.01f;
	if (m_fSpeed <= 0.f)
	{
		m_fSpeed = 0.f;

		if(XMVectorGetY(Get_TransformState(CTransform::STATE_POSITION)) >= 16.f)
			m_bDead = true;
	}
		

}

void CMonsterBullet::LateTick_Octorock(_float fTimeDelta)
{
	m_fEffectTimeEnd = 0.1f;
	m_fEffectTime += fTimeDelta;
	if (m_fEffectTime > m_fEffectTimeEnd)
	{
		CEffect::EFFECTDESC EffectDesc;
		EffectDesc.eEffectType = CEffect::VIBUFFER_RECT;
		EffectDesc.eEffectID = CObjectEffect::SMOKE;
		EffectDesc.vInitPositon = Get_TransformState(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.2f, 0.f, 0.f);
		EffectDesc.fDeadTime = 0.1f;
		EffectDesc.vColor = XMVectorSet(214, 201, 187, 255);
		EffectDesc.vInitScale = _float3(0.7f, 0.7f, 0.7f);
		CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_ObjectEffect"), LEVEL_STATIC, TEXT("Layer_PlayerEffect"), &EffectDesc);
		m_fEffectTime = 0.f;
	}
}

CMonsterBullet * CMonsterBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterBullet*	pInstance = new CMonsterBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMonsterBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterBullet::Clone(void * pArg)
{
	CMonsterBullet*	pInstance = new CMonsterBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMonsterBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBullet::Free()
{
	__super::Free();

	CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_MBULLET, this);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);
}
