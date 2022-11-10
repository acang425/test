#pragma once
#include "MyConsoleEngine.h"
#include "MyPlayer.h"
#include "BaseLogic.h"

namespace MyGame {
	//-----------------------����------------------------
	//ģ������
	const float CUBE_LENGTH = 100.0f;
	const float PLAYER_WIDTH = 40.0f;
	const float PLAYER_DEPTH = 40.0f;
	const float PLAYER_HEIGHT = 40.0f;
	const float PLAYER_STEP = 25.0f;
    //�����Զ��
	const float FAR_AWAY = 500.0f;
	const int MAXN = 8192;
	const float MAX_V = -40.0f;
	const float MIN_V = -10.0f;
	//ʱ�䵥λ
	const int HARDEST_SECOND = 100;
	const int FRAME_PER_SECOND = 50;
	//��Ϸ״̬�л�
	const int GAMESTATE_STARTMENU_START = 11;
	const int GAMESTATE_STARTMENU_RULE =12;
	const int GAMESTATE_STARTMENU_EXIT = 13;
	const int GAMESTATE_PAUSEMENU_CONTINUE = 21;
	const int GAMESTATE_PAUSEMENU_EXIT = 22;

	const int GAMESTATE_RULE_1 = 41;
	const int GAMESTATE_RULE_2= 42;
	const int GAMESTATE_RULE_3= 43;
	
	const int GAMESTATE_STARTMENU = 1;
	const int GAMESTATE_PAUSEMENU = 2;
	const int GAMESTATE_MAINGAME = 3;
	const int GAMESTATE_RULE = 4;
	const int GAMESTATE_GAMECONTINUE = 5;
	//�������Ⱦ���������ƶ��ٶ�
	extern VEC3 CAMERA_POS;
	extern VEC3 LOOK_AT;
	extern float vel_sudu;
	extern int rootState;
	extern ICamera myCamera;
	extern IRenderer myRender;

};
