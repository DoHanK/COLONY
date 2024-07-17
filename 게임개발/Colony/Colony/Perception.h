#pragma once
#include "stdafx.h"
#include "ColonyPlayer.h"
#include "AlienSpider.h"

class AlienSpider;

class Perception{
public:
	Perception(AlienSpider* pBody);
	
public:
	AlienSpider*			m_pOwner;

	void IsLookPlayer(Player* pPlayer);

};

