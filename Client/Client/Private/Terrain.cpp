#include "stdafx.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_FilterTexture()))
		return E_FAIL;
	
	return S_OK;
}

int CTerrain::Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG		
		m_pRendererCom->Add_Debug(m_pNavigationCom);
#endif
	}
	
}

HRESULT CTerrain::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;



	return S_OK;
}

HRESULT CTerrain::Ready_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_VIBuffer */
	CVIBuffer_Terrain::TERRAINDESC TerrainDesc;
	
	TerrainDesc.m_iPositionX = 0;
	TerrainDesc.m_iPositionZ = 0;
	TerrainDesc.m_iVerticeNumX = 30;
	TerrainDesc.m_iVerticeNumZ = 30;
	TerrainDesc. m_fHeight = -0.01f;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom, &TerrainDesc)))
		return E_FAIL;

	if (*(LEVEL*)pArg == LEVEL_GAMEPLAY)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Field"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Field"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}
	else if (*(LEVEL*)pArg == LEVEL_TAILCAVE)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Components(TEXT("Com_Navigation_TailCave"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_TailCave"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}
	else if (*(LEVEL*)pArg == LEVEL_ROOM)
	{
		CUI_Manager::ROOMTYPE eRoomType = CUI_Manager::Get_Instance()->Get_RoomType();

		switch (eRoomType)
		{
		case Client::CUI_Manager::MARINHOUSE:
			/* For.Com_Navigation */
			if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Room"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Room"), (CComponent**)&m_pNavigationCom)))
				return E_FAIL;
			break;
		case Client::CUI_Manager::SHOP:
			/* For.Com_Navigation */
			if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Shop"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Shop"), (CComponent**)&m_pNavigationCom)))
				return E_FAIL;
			break;
		case Client::CUI_Manager::CRANEGAME:
			/* For.Com_Navigation */
			if (FAILED(__super::Add_Components(TEXT("Com_Navigation_CraneGame"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_CraneGame"), (CComponent**)&m_pNavigationCom)))
				return E_FAIL;
			break;
		case Client::CUI_Manager::TELEPHONE:
			/* For.Com_Navigation */
			if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Telephone"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Telephone"), (CComponent**)&m_pNavigationCom)))
				return E_FAIL;
			break;
		default:
			break;
		}
		
		
	}
	else if (*(LEVEL*)pArg == LEVEL_TOWER)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Tower"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Tower"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	ID3D11ShaderResourceView*		pSRVs[] = {
		m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(0), 
		m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(1),
	};

	if (FAILED(m_pShaderCom->Set_ShaderResourceViewArray("g_DiffuseTexture", pSRVs, 2)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_BrushTexture", m_pTextureCom[TYPE_BRUSH]->Get_SRV(0))))
		return E_FAIL;
	/*if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_FilterTexture", m_pTextureCom[TYPE_FILTER]->Get_SRV(0))))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_FilterTexture", m_pFilterTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Create_FilterTexture()
{
	ID3D11Texture2D*			pTexture2D = nullptr;

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 128;
	TextureDesc.Height = 128;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint*			pPixel = new _uint[TextureDesc.Width * 	TextureDesc.Height];

	for (_uint i = 0; i < TextureDesc.Height; ++i)
	{
		for (_uint j = 0; j < TextureDesc.Width; ++j)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			if(j < 64)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = 128 * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture2D)))
		return E_FAIL;

	
	/*D3D11_MAP_WRITE_NO_OVERWRITE : ������ �ִ� ���� �����ѻ��·� �޸��� �ּҸ� ��������. */
	/*D3D11_MAP_WRITE_DISCARD : ������ �ִ� ���� ������ �޸� �ּҸ� ������. */

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	//pPixel[0] = D3DCOLOR_ARGB(255, 255, 255, 0);

	memcpy(SubResource.pData, pPixel, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(pTexture2D, 0);

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../../../Bin/Resources/Textures/Terrain/Newfilter.dds"))))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &m_pFilterTexture)))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture2D);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*	pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFilterTexture);

	for (_uint i = 0; i < TYPE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	
}
