#pragma once

#include "BaseObj.h"
BEGIN(Engine)
class CModel;
class CTexture;
END


BEGIN(Client)

class CNonAnim : public CBaseObj
{
public:
	typedef struct NonAnimModelTag
	{
		char pModeltag[MAX_PATH] = "";
		_float3 vPosition = _float3(0.f, 0.f, 0.f);
		_float3 vScale = _float3(1.f, 1.f, 1.f);
		_float3 vRotation = _float3(0.f, 0.f, 0.f);
		_float m_fAngle = 0.f;

	}NONANIMDESC;

protected:
	CNonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNonAnim(const CNonAnim& rhs);
	virtual ~CNonAnim() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() override;
public:
	virtual _bool Picking(_float3* PickingPoint) { return false; };
	virtual void PickingTrue();
	const char* Get_Modeltag() { return m_ModelDesc.pModeltag; }
	const NONANIMDESC Get_ModelDesc() {return m_ModelDesc;}

protected:
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pFogtexture = nullptr;
	NONANIMDESC				m_ModelDesc;
	_float					m_fAlpha = 1.f;


protected:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override; /* ���̴� ���������� ���� �����Ѵ�. */
	virtual HRESULT SetUp_ShaderID() override;

public:
	static CNonAnim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END