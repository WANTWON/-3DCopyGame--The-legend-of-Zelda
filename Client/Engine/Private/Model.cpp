#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Shader.h"
#include "HierarchyNode.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)	
	, m_Materials(rhs.m_Materials)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)	
	, m_iNumBones(rhs.m_iNumBones)
	, m_PivotMatrix(rhs.m_PivotMatrix)	
	, m_eModelType(rhs.m_eModelType)
{
	for (auto& pMeshContainer : rhs.m_Meshes)
	{
		m_Meshes.push_back((CMeshContainer*)pMeshContainer->Clone());
	}	
	
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pMaterials[i]);
	}	
}

CHierarchyNode * CModel::Get_BonePtr(const char * pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CHierarchyNode* pNode) 
	{
		return !strcmp(pNode->Get_Name(), pBoneName);
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

void CModel::Set_CurrentAnimIndex(_uint iAnimIndex)
{
	m_iCurrentAnimIndex = iAnimIndex;
}

void CModel::Set_TimeReset()
{
	m_Animations[m_iCurrentAnimIndex]->Set_TimeReset();
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	_uint			iFlag = 0;

	if(TYPE_NONANIM == eModelType)	
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Create_MeshContainer()))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;



	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	if (TYPE_NONANIM == m_eModelType)
		return S_OK;

	if (FAILED(Create_HierarchyNodes(m_pAIScene->mRootNode)))
		return E_FAIL;

	for (auto& pMeshContainer : m_Meshes)	
		pMeshContainer->SetUp_Bones(this);

	if (FAILED(Create_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::SetUp_Material(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eType, _uint TextureNum)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterials[eType] == nullptr)
		return S_OK;


	return pShader->Set_ShaderResourceView(pConstantName, m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterials[eType]->Get_SRV(TextureNum));
}

_bool CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{

	if (m_iCurrentAnimIndex != m_iNextAnimIndex)
	{	//TODO: 현재애님과 다음 애님프레임간의 선형보간 함수 호출 할 것.
		if (m_bInterupted)
		{
			m_Animations[m_iCurrentAnimIndex]->Set_TimeReset();
			m_bInterupted = false;
		}
		m_bLinearFinished = m_Animations[m_iCurrentAnimIndex]->Animation_Linear_Interpolation(fTimeDelta, m_Animations[m_iNextAnimIndex]);

		if (m_bLinearFinished == true)
		{
			m_Animations[m_iCurrentAnimIndex]->Set_TimeReset();

			m_iCurrentAnimIndex = m_iNextAnimIndex;

		}
	}
	else
	{
		/* 뼈의 m_TransformationMatrix행렬을 갱신한다. */
		if (m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, isLoop))
		{
			for (auto& pBoneNode : m_Bones)
			{
				/* 뼈의 m_CombinedTransformationMatrix행렬을 갱신한다. */
				pBoneNode->Invalidate_CombinedTransformationmatrix();
			}
			return true;
		}
	}

	for (auto& pBoneNode : m_Bones)
	{
		/* 뼈의 m_CombinedTransformationMatrix행렬을 갱신한다. */
		pBoneNode->Invalidate_CombinedTransformationmatrix();
	}

	return false;
}

HRESULT CModel::Render(CShader * pShader, _uint iMeshIndex, _uint iPassIndex)
{		
	_float4x4		BoneMatrix[256];

	m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrix, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Set_MatrixArray("g_BoneMatrices", BoneMatrix, 256);

	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Set_AnimationReset()
{
	m_Animations[m_iCurrentAnimIndex]->Set_TimeReset();

	return S_OK;
}

_bool CModel::Picking(CTransform * pTransform, _float3 * pOut)
{
	for (int i = 0; i < m_iNumMeshes; ++i)
	{
		if (m_Meshes[i]->Picking(pTransform, pOut))
			return true;
	}

	return false;
}



HRESULT CModel::Create_MeshContainer()
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pAIMesh = m_pAIScene->mMeshes[i];

		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, pAIMesh, this, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Create_Materials(const char* pModelFilePath)
{	
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		MODELMATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;

			if (j == 6)
			{
				if (FAILED(pAIMaterial->GetTexture(aiTextureType(8), 0, &strPath)))
				{
					if((FAILED(pAIMaterial->GetTexture(aiTextureType(2), 0, &strPath))))
						continue;
				}
					
			}
			else
			{
				if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
					continue;
			}
			

			char			szName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = ".dds";
			char			szTextureFileName[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

			if (j == 6)
			{
				char* ptr = strstr(szName, "smt");  

				if (ptr) {
					strncpy(ptr, "nml", 3);  
					puts(szName);
				}
				else
				{
					ptr = strstr(szName, "occ");  

					if (ptr) {
						strncpy(ptr, "nml", 3);
						puts(szName);
					}
				}

			}


			strcpy_s(szTextureFileName, szName);
			strcat_s(szTextureFileName, szExt);

			char			szDirectory[MAX_PATH] = "";
			char			szFullPath[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			strcpy_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szTextureFileName);

			_tchar			szRealPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szRealPath, MAX_PATH);

			ModelMaterial.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szRealPath);
			//if (nullptr == ModelMaterial.pMaterials[j])
				//return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);		
	}

	return S_OK;
}

HRESULT CModel::Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent)
{
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_Bones.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode);

	return S_OK;
}

HRESULT CModel::Create_Animations()
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*	pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*		pAnimation = CAnimation::Create(this, pAIAnimation);
		if (nullptr == pAIAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		//ERR_MSG(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBoneNode : m_Bones)
		Safe_Release(pBoneNode);
	m_Bones.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)		
			Safe_Release(Material.pMaterials[i]);
	}
	m_Materials.clear();
		



	m_Importer.FreeScene();

}
