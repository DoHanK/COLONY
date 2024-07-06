#include "UIControlHelper.h"

void UIControlHelper::GameStart()
{
	m_pSceneManger->ChangeScene(new GamePlayScene);
}

void UIControlHelper::GameQuit()
{
	PostQuitMessage(0);
}
