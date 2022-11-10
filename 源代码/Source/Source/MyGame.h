#pragma once
#include "MyConsoleEngine.h"
#include "MyPlayer.h"
#include "BaseLogic.h"

namespace MyGame {
	//-----------------------常量------------------------
	//模型数据
	const float CUBE_LENGTH = 100.0f;
	const float PLAYER_WIDTH = 40.0f;
	const float PLAYER_DEPTH = 40.0f;
	const float PLAYER_HEIGHT = 40.0f;
	const float PLAYER_STEP = 25.0f;
    //相机最远处
	const float FAR_AWAY = 500.0f;
	const int MAXN = 8192;
	const float MAX_V = -40.0f;
	const float MIN_V = -10.0f;
	//时间单位
	const int HARDEST_SECOND = 100;
	const int FRAME_PER_SECOND = 50;
	//游戏状态切换
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
	//相机，渲染器，方块移动速度
	extern VEC3 CAMERA_POS;
	extern VEC3 LOOK_AT;
	extern float vel_sudu;
	extern int rootState;
	extern ICamera myCamera;
	extern IRenderer myRender;

};
