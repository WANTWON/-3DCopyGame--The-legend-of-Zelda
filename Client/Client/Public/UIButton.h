#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CUIButton final : public CObj_UI
{
public:
	enum BUTTON_TYPE { BTN_FIX, BTN_INVEN, BTN_INTERACT };
	enum BUTTON_COLOR { BTN_BLACK, BTN_GREEN, BTN_WHITE };
	enum BUTTON_KEY { BTN_X, BTN_Y, BTN_A };

	enum INTERACT_TEX { OPEN , TALK};

	typedef struct InvenTiletag
	{
		BUTTON_TYPE eButtonType = BTN_FIX;
		_uint iTexNum = BTN_BLACK;
		BUTTON_KEY eState = BTN_X;
		_float2 vPosition = _float2(0.f, 0.f);

	}BUTTONDESC;

private:
	CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIButton(const CUIButton& rhs);
	virtual ~CUIButton() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	void Set_Visible(_bool type) { m_bShow = type; }
	void Set_TexType(_uint iNum) { m_ButtonDesc.iTexNum = iNum; }

private:
	virtual HRESULT Ready_Components(void * pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources()override;  /* 셰이더 전역변수에 값을 전달한다. */
	
private:
	BUTTONDESC  m_ButtonDesc;
	_bool		m_bShow = true;
	_float		m_fAlpha = 0.0f;
public:
	static CUIButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END