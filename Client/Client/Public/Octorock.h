#pragma once
#include "Monster.h"


BEGIN(Client)
class COctorock final : public CMonster
{
public:
	enum STATE { ATTACK_ED, ATTACK_ST, DAMAGE, DEAD, DEAD_FIRE , FALL,
		PIYO, IDLE, WALK };

private:
	COctorock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COctorock() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void Change_Animation(_float fTimeDelta) override;
	virtual HRESULT Ready_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual _bool IsDead() override;
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;

private:
	STATE m_eState = IDLE;

public:
	static COctorock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	

};

END