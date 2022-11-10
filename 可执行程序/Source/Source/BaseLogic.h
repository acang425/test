#pragma once
#include"MyConsoleEngine.h"
#include"MyGame.h"
#include<random>
#include<time.h>
namespace MyGame {
	//-------------------------分数类------------------------------
	class Score
	{
	public:
		UINT myScore;

		Score();

	};
	//-------------------------方块类------------------------------
	class MyCube
	{
	public:
		IMesh MyCubeMesh;//模型
		UINT MyCubeLength;//长度
		VEC3 moveDirection;//移动方向
		VEC3 pos;//位置
		Material MyCubemat;//材质
		static VEC3 moveV;//移动速度
		IPicture MyCubeTex;//贴图
		
		MyCube();
		void SetCubePosition(VEC3);//设置方块位置
		void Render();//渲染方块
		void MyCubeMove();//方块移动
	};
	//-------------------------游戏逻辑类------------------------------
	class BaseLogic
	{
	public:
		BaseLogic();
		~BaseLogic();
		UINT gameFrameCounter;


	//-------------------------方块属性------------------------------
		float spawnMyCubeV;
		float lastGameFrameCounter;
		float baseSpawnMyCubeV;
		float maxSpawnMyCubeV;
		MyCube MyCube_Queue[1024];
		int front;
		int back;

	//-------------------------光照------------------------------
		DirectionalLight highLight;
		DirectionalLight lowLight;
		DirectionalLight nowLight;

	//--------------------------玩家-----------------------------
		MyPlayer Player;

	//----------------------------分数---------------------------
		Score playerScore;


	//-------------------------图片------------------------------
		IPicture countPic;
		IPicture PauseMenu_Continue;
		IPicture PauseMenu_Exit;
		IPicture StartMenu_Start;
		IPicture StartMenu_Rule;
		IPicture StartMenu_Exit;
		IPicture GameRule1;
		IPicture GameRule2;
		IPicture GameRule3;
		IPicture BackGround;


	//------------------------方块操作------------------------------
		void pushMyCube(float, float);
		void pop_frontMyCube();
		void spawnMyCube();
		bool collid(VEC3, MyCube);

	//------------------------游戏逻辑------------------------------
		void gameContinue();
		void gameInit();
		void gameBegin();
		bool IsGameOver();
		void gameOver();
		void IsAddScore();
		void nextFrame();


	//------------------------GUI------------------------------
		void pauseMenu();
		void startMenu();
		void ruleShow();

	//------------------------GAME------------------------------
		void testgame();
		void wholeGame();

	};

};

