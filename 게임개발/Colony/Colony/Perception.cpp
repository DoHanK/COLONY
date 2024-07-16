#include "Perception.h"

Perception::Perception(AlienSpider* pBody):m_pOwner(pBody)
{


}

void Perception::IsLookPlayer(Player* pPlayer)
{
	// AI 반지름 AISIGHTRANGE
	// PlayerRange
	XMFLOAT3 pPosition(pPlayer->m_xmf4x4World._41, pPlayer->m_xmf4x4World._42+ AISIGHTHEIGHT, pPlayer->m_xmf4x4World._43);
	//AlienPosition
	XMFLOAT3 aPosition(m_pOwner->m_xmf4x4World._41, m_pOwner->m_xmf4x4World._42 + AISIGHTHEIGHT, m_pOwner->m_xmf4x4World._43);

	float PAdistance = XM3CalDis(pPosition, aPosition);

	XMFLOAT3 PADir = Vector3::Normalize(Vector3::Subtract(pPosition, aPosition));
	XMFLOAT3 LookDir(m_pOwner->GetLook());
	 //각도 계산
	float dotProduct = Vector3::DotProduct(LookDir, PADir);//물체의 방향과 이동방향을 내적..
	float v1Size = VectorSize(PADir);//사이각을 구하기 위한 벡터들의 크기구하기
	float v2Size = VectorSize(LookDir);
	float m_BetweenAngle = acosf(dotProduct / (v1Size * v2Size));//사이각 구하기
	float angle;
	//각도 예외처리
	if (std::acosf(-1) != 0)  angle = m_BetweenAngle * (180.f / std::acosf(-1));
	else angle = 0;
	if (std::isnan(angle)) {
		angle = 0.f;
	}
	m_pOwner->m_fPAangle = angle;


	if (m_pOwner->m_pPlayer &&PAdistance < PlayerRange + AISIGHTRANGE) {
		m_pOwner->m_pPlayer = pPlayer;
	}
	else {
		//범위 안에 있을때
		if (PAdistance <= PlayerRange + AISIGHTRANGE) {


		


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
}

void Perception::IsLookPlayer(Player* pPlayer,float scale)
{
	// AI 반지름 AISIGHTRANGE
	// PlayerRange
	XMFLOAT3 pPosition(pPlayer->m_xmf4x4World._41, pPlayer->m_xmf4x4World._42 + AISIGHTHEIGHT* scale, pPlayer->m_xmf4x4World._43);
	//AlienPosition
	XMFLOAT3 aPosition(m_pOwner->m_xmf4x4World._41, m_pOwner->m_xmf4x4World._42 + AISIGHTHEIGHT* scale, m_pOwner->m_xmf4x4World._43);

	float PAdistance = XM3CalDis(pPosition, aPosition);

	XMFLOAT3 PADir = Vector3::Normalize(Vector3::Subtract(pPosition, aPosition));
	XMFLOAT3 LookDir(m_pOwner->GetLook());
	//각도 계산
	float dotProduct = Vector3::DotProduct(LookDir, PADir);//물체의 방향과 이동방향을 내적..
	float v1Size = VectorSize(PADir);//사이각을 구하기 위한 벡터들의 크기구하기
	float v2Size = VectorSize(LookDir);
	float m_BetweenAngle = acosf(dotProduct / (v1Size * v2Size));//사이각 구하기
	float angle;
	//각도 예외처리
	if (std::acosf(-1) != 0)  angle = m_BetweenAngle * (180.f / std::acosf(-1));
	else angle = 0;
	if (std::isnan(angle)) {
		angle = 0.f;
	}
	m_pOwner->m_fPAangle = angle;


	if (m_pOwner->m_pPlayer && PAdistance < PlayerRange* scale + AISIGHTRANGE* scale) {
		m_pOwner->m_pPlayer = pPlayer;
	}
	else {
		//범위 안에 있을때
		if (PAdistance <= PlayerRange* scale + AISIGHTRANGE* scale) {





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
}