#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

/* ���� �����ϴ� ������ �޽ô�. */
class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	_bool Picking(class CTransform * pTransform, _float3 * pOut = nullptr);

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);


private:
	const aiMesh*				m_pAIMesh = nullptr;
	_uint						m_iMaterialIndex = 0;


public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END