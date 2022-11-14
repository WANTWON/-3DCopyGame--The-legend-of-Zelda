#pragma once
#include "Monster.h"


BEGIN(Client)
class CTailBoss final : public CMonster
{
public:
	enum TAILTYPE { TAIL1, TAIL2, TAIL3, TAIL4, TAIL5 };
	enum STATE { APPEAR, DAMAGE, DAMAGE_TURN, DEAD, FEINT, GUARD, IDLE, WAIT_MOVE };

	typedef struct TailBossTag
	{
		TAILTYPE eTailType = TAIL1;
		_vector InitPostion = XMVectorSet(0.f,0.f,0.f,1.f);
		CTailBoss* pParent = nullptr;
	}TAILDESC;

private:
	CTailBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTailBoss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Check_Navigation(_float fTimeDelta)  override;

public:
	virtual _uint Take_Damage(float fDamage, void* DamageType, CBaseObj* DamageCauser) override;
	STATE Get_AnimState() { return m_eState; }
	void Set_AnimState(STATE eState) { m_eState = eState; }
	void Change_Parent_State(STATE TailState);

private:
	virtual void Change_Animation(_float fTimeDelta) override;
	virtual HRESULT Ready_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual _bool IsDead() override;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Behaviour_Head(_float fTimeDelta);
	void Behaviour_Tail(_float fTimeDelta);
	HRESULT Create_Tail(TAILTYPE eType);
	

private:
	TAILDESC m_TailDesc;
	STATE m_eState = APPEAR;
	_float m_fTurnAngle = 0.5f;
	_bool m_bChangeDirection = false;
	_bool m_bStart = false;

public:
	static CTailBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END