#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiNodeAnim* pAIChannel);
	HRESULT Bin_Initialize(DATA_BINCHANNEL* pAIChannel, class CModel* pModel); // �߰�
	void Invalidate_TransformationMatrix(_float fCurrentTime);
	void Reset();
	_bool Linear_Interpolation(KEYFRAME NextKeyFrame, _float fLinearCurrentTime, _float fLinearTotalTime);

public: /*Get*/
	KEYFRAME	Get_StartKeyFrame(void) { return m_KeyFrames[0]; }

private:
	char					m_szName[MAX_PATH] = "";
	class CHierarchyNode*	m_pBoneNode = nullptr;

private:
	_uint					m_iNumKeyframes = 0;
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iCurrentKeyFrameIndex = 0;
	KEYFRAME				m_KeyFrame_Linear;


public: // For. Data
	void Get_ChannelData(DATA_BINCHANNEL* pChannelData);

public:
	static CChannel* Create(class CModel* pModel, aiNodeAnim* pAIChannel);
	static CChannel* Bin_Create(DATA_BINCHANNEL* pAIChannel, class CModel* pModel);	// �߰�
	virtual void Free() override;
};

END