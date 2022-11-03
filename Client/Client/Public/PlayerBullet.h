#pragma once
#include "Client_Defines.h"
#include "BaseObj.h"
#include "GameInstance.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CPlayerBullet final : public CBaseObj
{
public:
	enum BULLETTYPE { SWORD, BOW, BOOMERANG, BULLET_END};

	typedef struct Bullettag
	{
		BULLETTYPE eBulletType = BULLET_END;
		_vector	   vInitPositon = XMVectorSet(0.f,0.f,0.f,1.f);
		_vector	   vLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_float	   fDeadTime = 0.f;
	}BULLETDESC;

protected:
	CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerBullet(const CPlayerBullet& rhs);
	virtual ~CPlayerBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Target(CBaseObj* pObject) { m_pTarget = pObject; }


private:
	virtual HRESULT Ready_Components(void* pArg = nullptr);
	virtual HRESULT SetUp_ShaderResources();
	virtual HRESULT SetUp_ShaderID();
	virtual void Change_Animation(_float fTimeDelta);

private:
	void Moving_SwordBullet(_float fTimeDelta);
	void Moving_BowBullet(_float fTimeDelta);

protected: /* For.Components */
	CModel*					m_pModelCom = nullptr;
	CBaseObj*				m_pTarget = nullptr;

	_float					m_fDeadtime = 0.f;
	_float					m_fAnimSpeed = 1.f;
	BULLETDESC				m_BulletDesc;

public:
	static CPlayerBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END