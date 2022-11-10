#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CMessageBox final : public CObj_UI
{


private:
	CMessageBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMessageBox(const CMessageBox& rhs);
	virtual ~CMessageBox() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Ready_Components(void * pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources()override;  /* ���̴� ���������� ���� �����Ѵ�. */

private:
	_uint		m_iTextureNum = 0;
	_bool		m_bRender = false;
	_float		m_fAlpha = 0.0f;


public:
	static CMessageBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END