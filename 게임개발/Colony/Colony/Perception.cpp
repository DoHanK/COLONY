#include "Perception.h"

Perception::Perception(AlienSpider* pBody):m_pOwner(pBody)
{


}

void Perception::IsLookPlayer( Player* pPlayer)
{
	// AI ������ AISIGHTRANGE
	// PlayerRange
	XMFLOAT3 pPosition(pPlayer->m_xmf4x4World._41, pPlayer->m_xmf4x4World._42+ AISIGHTHEIGHT, pPlayer->m_xmf4x4World._43);
	
	XMFLOAT3 aPosition(m_pOwner->m_xmf4x4World._41, m_pOwner->m_xmf4x4World._42 + AISIGHTHEIGHT, m_pOwner->m_xmf4x4World._43);

	float PAdistance = XM3CalDis(pPosition, aPosition);

	//���� �ȿ� ������
	if (PAdistance <= PlayerRange + AISIGHTRANGE) {

		XMFLOAT3 PADir = Vector3::Normalize(Vector3::Subtract(pPosition, aPosition));


		XMFLOAT3 LookDir(m_pOwner->GetLook());


		float dotProduct = Vector3::DotProduct(LookDir, PADir);//��ü�� ����� �̵������� ����..
		float v1Size = VectorSize(PADir);//���̰��� ���ϱ� ���� ���͵��� ũ�ⱸ�ϱ�
		float v2Size = VectorSize(LookDir);
		float m_BetweenAngle = acosf(dotProduct / (v1Size * v2Size));//���̰� ���ϱ�
		float angle;
		//���� ����ó��
		if (std::acosf(-1) != 0)  angle = m_BetweenAngle * (180.f / std::acosf(-1));
		else angle = 0;
		if (std::isnan(angle)) {
			angle = 0.f;
		}
		


		if (angle < AIFOV) {
			m_pOwner->m_pPlayer = pPlayer;
		}
		else {
			m_pOwner->m_pPlayer = NULL;
		}
	}
	else {
		m_pOwner->m_pPlayer = NULL;
	}
}
