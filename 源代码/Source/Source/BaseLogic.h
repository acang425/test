#pragma once
#include"MyConsoleEngine.h"
#include"MyGame.h"
#include<random>
#include<time.h>
namespace MyGame {
	//-------------------------������------------------------------
	class Score
	{
	public:
		UINT myScore;

		Score();

	};
	//-------------------------������------------------------------
	class MyCube
	{
	public:
		IMesh MyCubeMesh;//ģ��
		UINT MyCubeLength;//����
		VEC3 moveDirection;//�ƶ�����
		VEC3 pos;//λ��
		Material MyCubemat;//����
		static VEC3 moveV;//�ƶ��ٶ�
		IPicture MyCubeTex;//��ͼ
		
		MyCube();
		void SetCubePosition(VEC3);//���÷���λ��
		void Render();//��Ⱦ����
		void MyCubeMove();//�����ƶ�
	};
	//-------------------------��Ϸ�߼���------------------------------
	class BaseLogic
	{
	public:
		BaseLogic();
		~BaseLogic();
		UINT gameFrameCounter;


	//-------------------------��������------------------------------
		float spawnMyCubeV;
		float lastGameFrameCounter;
		float baseSpawnMyCubeV;
		float maxSpawnMyCubeV;
		MyCube MyCube_Queue[1024];
		int front;
		int back;

	//-------------------------����------------------------------
		DirectionalLight highLight;
		DirectionalLight lowLight;
		DirectionalLight nowLight;

	//--------------------------���-----------------------------
		MyPlayer Player;

	//----------------------------����---------------------------
		Score playerScore;


	//-------------------------ͼƬ------------------------------
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


	//------------------------�������------------------------------
		void pushMyCube(float, float);
		void pop_frontMyCube();
		void spawnMyCube();
		bool collid(VEC3, MyCube);

	//------------------------��Ϸ�߼�------------------------------
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

