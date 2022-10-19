#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CInvenItem;
class CInvenTile final : public CObj_UI
{
public:
	enum TILE_TYPE { INEVEN_TILE, EQUIP_TILE };
	enum TILE_STATE { STATE_DEFAULT, STATE_EQUIP };

	typedef struct InvenTiletag
	{
		TILE_TYPE eTileType = INEVEN_TILE;
		TILE_STATE eState = STATE_DEFAULT;
		_float2 vPosition = _float2(0.f, 0.f);

	}INVENDESC;

private:
	CInvenTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvenTile(const CInvenTile& rhs);
	virtual ~CInvenTile() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_SelectTile(_bool type) { m_bSelected = type; }
	void Set_TileState(TILE_STATE eState) {m_InvenDesc.eState = eState;}

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT SetUp_ShaderResources()override;  /* ���̴� ���������� ���� �����Ѵ�. */
	virtual HRESULT SetUp_ShaderID() override;

private:
	class CInvenItem* m_pItem = nullptr;
	INVENDESC  m_InvenDesc;
	_bool m_bSelected = false;

public:
	static CInvenTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END