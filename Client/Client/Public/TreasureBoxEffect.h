#pragma once
#include "Client_Defines.h"
#include "Effect.h"
#include "GameInstance.h"



BEGIN(Client)
class CTreasureBoxEffect final : public CEffect
{
public:
	enum TYPE { 
		/* For TreasureBox*/
		TREASURE_BEAM, TREASURE_HALO, TREASURE_GLOW, TREASURE_GLITTER,
		TREASURE_CROSS, TREASURE_FLASH, TRASURE_ENTRANCEBOX,  
	};



protected:
	CTreasureBoxEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasureBoxEffect(const CTreasureBoxEffect& rhs);
	virtual ~CTreasureBoxEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Target(CBaseObj* pObject) { m_pTarget = pObject; }
	void Set_EndMode(_bool type) { m_bEndMode = type; }

private:
	virtual HRESULT Ready_Components(void* pArg = nullptr);
	virtual HRESULT SetUp_ShaderID();
	virtual void Change_Animation(_float fTimeDelta);
	
private:
	/* For Get Item*/
	void Tick_FlashEffect(_float fTimeDelta);
	void Tick_HorizontalGlowEffect(_float fTimeDelta);
	void Tick_RingEffect(_float fTimeDelta);
	void Tick_HaloEffect(_float fTimeDelta);
	void Tick_GlitterEffect(_float fTimeDelta);
	void Tick_Ray(_float fTimeDelta);

	/* Fir TreasureBox */
	void Tick_GlowEffect(_float fTimeDelta);
	void Tick_TreausureBox(_float fTimeDelta);
	void Tick_TreasureCross(_float fTimeDelta);

private:
	_float	m_fAngle = 0.f;
	_float	m_fColorTime = 0.f;
	_float	m_fSpeed = 0.f;

	vector<_vector> m_vecColor;
	_int			m_iColorIndex = 0;
	_bool			m_bColorChaged = false;
	_bool			m_bEndMode = false;
	_bool			m_bMax = false;
	_float3			m_vMaxScale = _float3(2.f, 0.5f, 0.f);
	_float			m_fZoomValue = 0.f;

public:
	static CTreasureBoxEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END