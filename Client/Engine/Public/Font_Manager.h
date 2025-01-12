#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)
public:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, _fvector vPos, _fvector vColor, _float fScale =1.f);

private:
	map<const _tchar*, class CCustomFont*>			m_Fonts;
	typedef map<const _tchar*, class CCustomFont*>	FONTS;

private:
	class CCustomFont* Find_Fonts(const _tchar* pFontTag);

public:
	virtual void Free() override;
};

END