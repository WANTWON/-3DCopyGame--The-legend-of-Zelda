#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(CModel* pModel, aiAnimation * pAIAnimation)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*		pAIChannel = pAIAnimation->mChannels[i];

		CChannel*		pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, _bool isLoop)
{
	/* ���� ������� �ð�. */
	m_fCurrentTime += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime >= m_fDuration)
	{
		m_fCurrentTime = 0.f;
		m_isFinished = true;
	}
	else
		m_isFinished = false;

	for (auto& pChannel : m_Channels)
	{
		if (true == m_isFinished)
			pChannel->Reset();

		pChannel->Invalidate_TransformationMatrix(m_fCurrentTime);
	}

	if (true == m_isFinished && true == isLoop)
		m_isFinished = false; //������ �� ���� ������ false�� ��ȯ�ϰ� �Ϸ���
	 
	return m_isFinished; //������ ���� �ʰ� �ִϸ��̼��� ������ ��
}

CAnimation * CAnimation::Create( CModel* pModel, aiAnimation * pAIAnimation)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		ERR_MSG(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
