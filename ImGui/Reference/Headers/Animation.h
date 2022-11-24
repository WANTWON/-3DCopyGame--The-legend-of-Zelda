#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiAnimation* pAIAnimation);
	HRESULT Bin_Initialize(DATA_BINANIM* pAIAnimation, class CModel* pModel); // �߰�
	_bool Invalidate_TransformationMatrix(_float fTimeDelta, _bool isLoop);
	

public:/*Get*/
	vector<class CChannel*>	Get_Channels(void) { return m_Channels; }
	void	Set_TimeReset();

public:
	_bool Animation_Linear_Interpolation(_float fTimeDelta, CAnimation* NextAnim);

private:
	char				m_szName[MAX_PATH] = "";

	/* ���� �ִϸ��̼� �ϳ��� ����ϴµ� �ɸ��� �� �ð�. */
	_float				m_fDuration = 0.f;

	/* 1�ʿ� ����Ǿ���� �ִϸ��̼��� �ӵ�. */	
	_float				m_fTickPerSecond = 0.f;
	_float				m_fCurrentTime = 0.f;

	/* Channel : ��. */

	/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ����. */
	_uint							m_iNumChannels = 0;

	vector<class CChannel*>			m_Channels;

	_bool							m_isFinished = false;
	_bool							m_isLoop = true;


	/*For. Linear Time*/
	_float m_fTotal_Linear_Duration = 0.2f;
	_float m_fLinear_CurrentTime = 0.f;
	_bool	m_bLinearFinished = false;

public: // For. Data �߰�
	void Get_AnimData(DATA_BINANIM* pAnimData);


public:
	static CAnimation* Create(class CModel* pModel, aiAnimation* pAIAnimation);
	static CAnimation* Bin_Create(DATA_BINANIM* pAIAnimation, class CModel* pModel); // �߰�
	virtual void Free() override;
};

END