#include "UIControlHelper.h"

void UIControlHelper::GameStart()
{
	m_pSceneManger->ChangeScene(new GamePlayScene);
}

void UIControlHelper::GameQuit()
{
	PostQuitMessage(0);
}

void UIControlHelper::GameWin()
{
	m_pSceneManger->ChangeScene(new WinScene);
}

void UIControlHelper::GameLose()
{
	m_pSceneManger->ChangeScene(new LoseScene);
}
