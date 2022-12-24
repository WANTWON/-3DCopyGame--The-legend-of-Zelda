#pragma once

#include "Component.h"
#include "HierarchyNode.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice ,ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint	Get_NumMeshContainers() const {
		return m_iNumMeshes;
	}
	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}
	class CHierarchyNode* Get_BonePtr(const char* pBoneName) const;

public:
	void Set_CurrentAnimIndex(_uint iAnimIndex);
	/*For. NextTotalBody_Anim*/
	void Set_NextAnimIndex(_uint iAnimIndex) { if (m_iNextAnimIndex != iAnimIndex) { m_bInterupted = true; }
		m_iNextAnimIndex = iAnimIndex;
	}
	void Set_TimeReset();

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Bin_Initialize_Prototype(DATA_BINSCENE* pScene, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix);	// �߰�
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg); // �߰�

public:
	HRESULT SetUp_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint TextureNum = 0);
	_bool Play_Animation(_float fTimeDelta, _bool isLoop);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
	HRESULT RenderShadow(class CShader* pShader, _uint iMeshIndex, _uint iLevelIndex,  _uint iPassIndex = 0);
	HRESULT Set_AnimationReset();
	_bool Picking(CTransform * pTransform, _float3 * pOut);
	

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;
	DATA_BINSCENE*				m_pBin_AIScene = nullptr; // �߰�
	_bool						m_bIsProto = false; // �߰�
	_bool						m_bIsBin = false; // �߰�

private:
	TYPE								m_eModelType = TYPE_END;
	_uint								m_iNumMeshes = 0;
	vector<class CMeshContainer*>		m_Meshes;

private:
	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

private:
	/* �� �� ��ü ���� ����. */
	_uint								m_iNumBones;
	vector<class CHierarchyNode*>		m_Bones;

private:
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;
	_uint								m_iCurrentAnimIndex = 0;


	/*For. NextIndex*/
	_uint								m_iNextAnimIndex = 0;
	/*For. Lineared*/
	_bool								m_bLinearFinished = false;
	_bool								m_bInterupted = false;
private:
	_float4x4				m_PivotMatrix;

public: // For. Data �߰�
	HRESULT Get_HierarchyNodeData(DATA_BINSCENE* pBinScene);
	HRESULT Get_MaterialData(DATA_BINSCENE* pBinScene);
	HRESULT Get_MeshData(DATA_BINSCENE* pBinScene);
	HRESULT Get_AnimData(DATA_BINSCENE* pBinScene);

private: // �߰�
	vector<DATA_BINMATERIAL>				m_DataMaterials;


private: // �߰�
	HRESULT Bin_Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Bin_Ready_Materials(const char* pModelFilePath);
	HRESULT Bin_Ready_HierarchyNodes();
	HRESULT Bin_Ready_Animations(CModel* pModel);
	HRESULT Safe_Release_Scene();

private:
	HRESULT Create_MeshContainer();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent = nullptr);
	HRESULT Create_Animations();
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CModel* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DATA_BINSCENE* pScene, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity()); // �߰�
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END