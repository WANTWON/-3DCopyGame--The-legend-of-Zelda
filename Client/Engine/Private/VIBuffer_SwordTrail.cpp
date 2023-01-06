#include "..\Public\VIBuffer_SwordTrail.h"

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_SwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Initialize(void * pArg)
{

	//ó������ �޸�Ǯó�� �̸� ������ ��������?

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = m_iLimitPointCount*2;
	m_iNumVertexBuffers = 1;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//���ؽ� ���� ���ε��
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTEX*			pVertices = new VTXTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);

	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_iIndicesByte = sizeof(FACEINDICES32);
	m_iNumPrimitive = m_iLimitPointCount*2 - 2;
	m_iNumIndicesPerPrimitive = 3;

	m_BufferDesc.ByteWidth = m_iIndicesByte * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* �������۸� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;				//�ε��� ���ۿ�
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);

	FACEINDICES32*			pIndices = new FACEINDICES32[m_iNumPrimitive];

	for (_uint iIndex = 0; iIndex < m_iNumPrimitive; iIndex += 2)
	{
		pIndices[iIndex]._0 = iIndex + 3;
		pIndices[iIndex]._1 = iIndex + 1;
		pIndices[iIndex]._2 = iIndex;

		pIndices[iIndex + 1]._0 = iIndex + 2;
		pIndices[iIndex + 1]._1 = iIndex + 3;
		pIndices[iIndex + 1]._2 = iIndex;
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Update(TRAILPOINT TrailPoint)
{
	_float3 PointDown;
	_float3 PointUp;

	XMStoreFloat3(&PointDown, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vPointDown), XMLoadFloat4x4(&TrailPoint.vTrailMatrix)));
	XMStoreFloat3(&PointUp, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vPointUp), XMLoadFloat4x4(&TrailPoint.vTrailMatrix)));

	if (!isnan(PointDown.x))
	{
		m_TrailPoint.push_back(PointDown);
		m_TrailPoint.push_back(PointUp);
	}

	if (m_TrailPoint.size() > m_iLimitPointCount)
	{
		m_TrailPoint.pop_front();
		m_TrailPoint.pop_front();
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXTEX* pVertices = (VTXTEX*)SubResource.pData;

	auto TrailPointIter = m_TrailPoint.begin();

	_vector		vPos[50]; 
							 
	for (_uint iIndex = 0; iIndex < m_TrailPoint.size(); iIndex += 2)
	{
		vPos[iIndex] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);
		XMStoreFloat3(&pVertices[iIndex].vPosition, XMLoadFloat3(&(*TrailPointIter)));
		XMStoreFloat2(&pVertices[iIndex].vTexture, XMLoadFloat2(&_float2((_float)iIndex / (m_TrailPoint.size() - 2), 1.f)));

		++TrailPointIter;

		vPos[iIndex + 1] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);
		XMStoreFloat3(&pVertices[iIndex + 1].vPosition, XMLoadFloat3(&(*TrailPointIter)));
		XMStoreFloat2(&pVertices[iIndex + 1].vTexture, XMLoadFloat2(&_float2((_float)iIndex / (m_TrailPoint.size() - 2), 0.f)));

		++TrailPointIter;
	}


#pragma region ����

	m_iCatmullRomCount = m_TrailPoint.size();
	if (m_iCatmullRomCount < 2)
		goto Unmaping;

	XMVECTOR vec;
	_float3	 vCat[80];

	if (m_TrailPoint.size() >= m_iLimitPointCount) //Ʈ���ϰ��� �� ä��������
	{
		_float4 vCatPos = { 0.f,0.f ,0.f ,0.f };

		//�ҵ� Ʈ������ ���κ� ����
		for (_int i = 19; i >= 10; i--) //���� ������ 10���� ��´�
		{
			vec = XMVectorCatmullRom(vPos[10], vPos[11], vPos[19], vPos[18], _float((i - 10) / 20.f));
			XMStoreFloat4(&vCatPos, vec);
			XMStoreFloat3(&pVertices[(m_iLimitPointCount-1 - i) * 2 + 1].vPosition, XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));
			XMStoreFloat3(&vCat[(m_iLimitPointCount - 1 - i) * 2 + 1], XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));
		}
		
		//�ҵ� Ʈ������ �Ʒ��κ� ����
		for (_int i = 19; i >= 10; i--) //���� ������ 10���� ��´�
		{
			vec = XMVectorCatmullRom(vPos[11], vPos[10], vPos[18], vPos[19], _float((i - 10) / 20.f));
			XMStoreFloat4(&vCatPos, vec);
			XMStoreFloat3(&pVertices[(m_iLimitPointCount - 1 - i) * 2].vPosition, XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));
			XMStoreFloat3(&vCat[(m_iLimitPointCount - 1 - i) * 2], XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));
		}
	}


#pragma endregion ��ü ���� ����


	Unmaping :
			 m_pContext->Unmap(m_pVB, 0);

			 return S_OK;

}

CVIBuffer_SwordTrail * CVIBuffer_SwordTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_SwordTrail*	pInstance = new CVIBuffer_SwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CVIBuffer_SwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CVIBuffer_SwordTrail::Clone(void * pArg)
{
	CVIBuffer_SwordTrail*	pInstance = new CVIBuffer_SwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVIBuffer_SwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SwordTrail::Free()
{
	__super::Free();
}
