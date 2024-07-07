#pragma once
#include "stdafx.h"
#include "SceneManager.h"
#include "ColonyScene.h"

class SceneManager;

class UIControlHelper{
public:

public:
	SceneManager* m_pSceneManger;

public:
	void GameStart();
	void GameQuit();
	void GameWin();
	void GameLose();
};

