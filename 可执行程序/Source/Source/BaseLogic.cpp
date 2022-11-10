#include "MyGame.h"
#include<string>

#pragma comment(lib,"Winmm.lib")

using namespace MyGame;
//********************方块类************************
Score::Score() {
	myScore = 0;
}
MyCube::MyCube() {
	//位置初始化
	pos.z = FAR_AWAY;
	MyCubeLength = 100;
	moveDirection = VEC3(0, 0, 1);	
	//材质，贴图，模型初始化
	MyCubemat.ambient = { 0.5f,0.5f,0.5f };
	MyCubemat.diffuse = { 1.0f,1.0f,1.0f };
	MyCubemat.specular = { 1.0f,1.0f,1.0f };
	MyCubemat.specularSmoothLevel = 20;
	MyCubeTex.LoadPPM("Media/qin.pbm");
	MyCubeMesh.LoadFile_OBJ("Media/cube.obj");
	MyCubeMesh.SetPosition(0, 0, FAR_AWAY);
	MyCubeMesh.SetMaterial(MyCubemat);
	MyCubeMesh.SetTexture(&MyCubeTex);
}
VEC3 MyCube::moveV = VEC3(0, 0, vel_sudu);

void MyCube::Render()
{
	myRender.RenderMesh(MyCubeMesh);
}
void MyCube::MyCubeMove()
{
	pos += MyCube::moveV;
	SetCubePosition(pos);
}
void MyCube::SetCubePosition(VEC3 Vpos)
{
	MyCubeMesh.SetPosition(Vpos);
}

//********************************************




//**********************基础逻辑类**********************
BaseLogic::BaseLogic()
{

	//-----------------方块属性----------------------
	gameFrameCounter = 80;
	spawnMyCubeV = 150;
	lastGameFrameCounter = -100.0f;
	baseSpawnMyCubeV = 80.0f;
	maxSpawnMyCubeV = 40.0f;
	front = 0;
	back = -1;

	playerScore.myScore = 0;

	//-----------------光照----------------------
	highLight.mAmbientColor = { 1.0f,1.0f,1.0f };
	highLight.mDiffuseColor = { 1.0f,1.0f,1.0f };
	highLight.mDiffuseIntensity = 1.0f;
	highLight.mDirection = { -1.0f,-1.0f,-1.0f };
	highLight.mIsEnabled = TRUE;
	highLight.mSpecularColor = { 1.0f,1.0f,1.0f };
	highLight.mSpecularIntensity = 1.2f;


	lowLight.mAmbientColor = { 1.0f,1.0f,1.0f };
	lowLight.mDiffuseColor = { 1.0f,1.0f,1.0f };
	lowLight.mDiffuseIntensity = 1.0f;
	lowLight.mDirection = { -1.0f,-1.0f,-1.0f };
	lowLight.mIsEnabled = TRUE;
	lowLight.mSpecularColor = { 1.0f,1.0f,1.0f };
	lowLight.mSpecularIntensity = 1.2f;

	nowLight = highLight;
	myRender.SetLight(0, nowLight);

	//-----------------相机--------------------
	CAMERA_POS = VEC3(0, 300.0f, -250.0f);
	LOOK_AT = VEC3(0,0,50.0f);
	myCamera.SetPosition(CAMERA_POS);
	myCamera.SetLookAt(LOOK_AT);
	myCamera.SetViewAngle(Math::PAI_CIRCLE / 2.0f, 2.5f);
	myCamera.SetViewFrustumPlane(1.0f, 1500.0f);


	myRender.Init(250, 100);
	myRender.SetWindowTitle("Cube Game");
	myRender.SetCamera(myCamera);
	//--------------------光标-------------
	::SetCursorPos(0, 0);

	//----------------菜单图片----------------
	PauseMenu_Continue.LoadPPM("Media/PauseMenu_Continue.pbm");
	PauseMenu_Exit.LoadPPM("Media/PauseMenu_Exit.pbm");
	StartMenu_Start.LoadPPM("Media/StartMenu_StartGame.pbm");
	StartMenu_Rule.LoadPPM("Media/StartMenu_GameRule.pbm");
	StartMenu_Exit.LoadPPM("Media/StartMenu_ExitGame.pbm");
	GameRule1.LoadPPM("Media/GameRule1.pbm");
	GameRule2.LoadPPM("Media/GameRule2.pbm");
	GameRule3.LoadPPM("Media/GameRule3.pbm");
	BackGround.LoadPPM("Media/Background.pbm");

	//----------------游戏状态----------------
	rootState = GAMESTATE_STARTMENU;
}
BaseLogic::~BaseLogic() {

}
void BaseLogic::pushMyCube(float _x,float _y) {
	back++;//后指针后移
	if (back < 1024)
	{
		MyCube_Queue[back].pos.x = _x;
		MyCube_Queue[back].pos.y = _y;
		MyCube_Queue[back].pos.z = FAR_AWAY;
		MyCube_Queue[back].SetCubePosition(MyCube_Queue[back].pos);
	}
	
}
void BaseLogic::pop_frontMyCube() {
	if (front < back) {
		front++;//前指针后移
	}
}
void BaseLogic::spawnMyCube() {
	//随机生成方块：数量与位置
	srand(time(0));
	int r = rand();
	r = rand();
	int cnt = r % 3 + 5;
	for (int i = 1;i <= cnt;i++) {
		r = rand();
		int POS = (r % 8);
		float _x = ((POS) % 4 * 1.0f - 1.5f) * CUBE_LENGTH;
		float _y = (POS / 4 * 1.0f + 0.5f) * CUBE_LENGTH;
		pushMyCube(_x, _y);
	}
}

bool BaseLogic::collid(VEC3 point, MyCube C) 
{
	//通过切比雪夫距离判定是否碰撞
	VEC3 center = C.pos;
	float dis_ =max( fabs(point.x - center.x) , max(fabs(point.y - center.y) , fabs(point.z - center.z)));
	return  (dis_ <= (CUBE_LENGTH / 2));

}
void BaseLogic::gameContinue() {
	//模拟倒计时
	countPic.LoadPPM("Media/3.pbm");
	myRender.DrawPicture(countPic, 0, 0, 250, 100);
	nowLight = lowLight;
	myRender.Present();
	Player.enableMove = 0;
	Sleep(1000);

	countPic.LoadPPM("Media/2.pbm");
	myRender.DrawPicture(countPic, 0, 0, 250, 100);
	myRender.Present();
	Player.enableMove = 0;
	Sleep(1000);

	countPic.LoadPPM("Media/1.pbm");
	myRender.DrawPicture(countPic, 0, 0, 250, 100);
	myRender.Present();
	Player.enableMove = 1;
	Sleep(1000);
	myRender.Clear();
	nowLight = highLight;
	while (1) {
		nextFrame();
		if (IS_KEY_DOWN(VK_ESCAPE))
		{
			//暂停菜单
			rootState = GAMESTATE_PAUSEMENU;
			break;
		}
		if (rootState == GAMESTATE_STARTMENU)
		{
			break;
		}
	}
}
void BaseLogic::gameInit() {

	//-----------------方块属性----------------------
	gameFrameCounter = 0;
	lastGameFrameCounter = 0.0f;
	front = 0;
	back = -1;
	//-----------------光照属性----------------------
	highLight.mDiffuseIntensity = 0.5f;
	highLight.mSpecularIntensity = 1.0f;
	lowLight.mDiffuseIntensity = 0.2f;
	lowLight.mSpecularIntensity = 0.3f;
	nowLight = lowLight;
}
void BaseLogic::gameBegin() {
	gameInit();
	gameContinue();
}
bool BaseLogic::IsGameOver() {
	bool OK = 0;
	VEC3 center = Player.playerPos;
	//只用判定玩家立方体前方的四个点是否与敌人碰撞
	VEC3 zs = VEC3(center.x - PLAYER_WIDTH / 2, center.y + PLAYER_HEIGHT / 2, center.z + PLAYER_DEPTH / 2);
	VEC3 ys = VEC3(center.x + PLAYER_WIDTH / 2, center.y + PLAYER_HEIGHT / 2, center.z + PLAYER_DEPTH / 2);
	VEC3 zx = VEC3(center.x - PLAYER_WIDTH / 2, center.y - PLAYER_HEIGHT / 2, center.z + PLAYER_DEPTH / 2);
	VEC3 yx = VEC3(center.x + PLAYER_WIDTH / 2, center.y - PLAYER_HEIGHT / 2, center.z + PLAYER_DEPTH / 2);
	for (auto point : { zs,ys,zx,yx }) {
		for (int i = front; i <= front + 6 && i <= back; i++) {
			bool okok = 0;
			VEC3 center = MyCube_Queue[i].pos;
			float dis_ = max(fabs(point.x - center.x), max(fabs(point.y - center.y), fabs(point.z - center.z)));
			if(dis_ <= (CUBE_LENGTH / 2))	OK = 1;
		
		}
	}
	return OK;
}
void BaseLogic::gameOver() {
	Player.enableMove = 0;
	Sleep(1000);
	std::string s = "可莉被琴团长抓住啦！\n 分数为:";
	s += std::to_string(playerScore.myScore);
	LPCSTR str = s.c_str();
	MessageBox(0, str, 0, 0);
	//游戏结束返回开始菜单
	rootState=GAMESTATE_STARTMENU;
	playerScore.myScore = 0;
}
void BaseLogic::IsAddScore() {
	//经过一层就加一分
	bool OK = 0;
	for (int i = front; i <= front + 6 && i <= back; i++) {
		if (MyCube_Queue[i].pos.z + CUBE_LENGTH / 2 <= Player.playerPos.z) {
			pop_frontMyCube();
			OK = 1;
		}
	}
	if (OK)playerScore.myScore++;

}

void BaseLogic::nextFrame() {
	myRender.Clear();
	//-------------------------渲染------------------------------
	myRender.DrawPicture(BackGround, 0, 0, 250, 100);
	if (gameFrameCounter - lastGameFrameCounter >= spawnMyCubeV) {
		spawnMyCube();
		lastGameFrameCounter = gameFrameCounter;
	}
	Player.playerMove();

	//-----------------------穿梭效果：变换相机视点-----------------
	CAMERA_POS = VEC3(Player.playerPos.x, Player.playerPos.y+CUBE_LENGTH/2, -250.0f);
	LOOK_AT = VEC3(Player.playerPos.x*7/8, Player.playerPos.y*7/8+CUBE_LENGTH/2, 50.0f);
	myCamera.SetPosition(CAMERA_POS);
	myCamera.SetLookAt(LOOK_AT);

	for (int i = front; i <= back; i++) {
		MyCube_Queue[i].MyCubeMove();
	}
	if (IsGameOver())gameOver();
	IsAddScore();
	for (int i = front; i <= back; i++) {
		MyCube_Queue[i].Render();
	}
	Player.Render();
	myRender.SetLight(0, nowLight);
	//显示图像
	myRender.Present();
	gameFrameCounter++;

	//-------------------------改变方块速度------------------------------
	vel_sudu = Lerp(MIN_V, MAX_V, (double)gameFrameCounter / (HARDEST_SECOND * FRAME_PER_SECOND));
	MyCube::moveV.z = vel_sudu;
	spawnMyCubeV = -400 / vel_sudu;
	

	Sleep(int(1000/FRAME_PER_SECOND));
}


void BaseLogic::pauseMenu()
{

	static int startMenuState = GAMESTATE_PAUSEMENU_CONTINUE;

	UINT width = myRender.GetBufferWidth();
	UINT height = myRender.GetBufferHeight();

	myRender.Clear();

	switch (startMenuState)
	{
	case GAMESTATE_PAUSEMENU_CONTINUE:
	{
		
		if (IS_KEY_DOWN(VK_RETURN))
		{
			rootState = GAMESTATE_GAMECONTINUE;
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			
			startMenuState = GAMESTATE_PAUSEMENU_EXIT;
			Sleep(300);
		}

		myRender.DrawPicture(PauseMenu_Continue, 0, 0, width, height);

		break;
	}
	case GAMESTATE_PAUSEMENU_EXIT:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			exit(0);
			break;
		}
		if (IS_KEY_DOWN(VK_UP))
		{
			startMenuState = GAMESTATE_PAUSEMENU_CONTINUE;
			Sleep(300);
		}

		myRender.DrawPicture(PauseMenu_Exit, 0, 0, width, height);
		break;
	}

	default:
		DEBUG_MSG1("game state error!!");
		break;
	}


	myRender.Present();
}


void BaseLogic::startMenu()
{

	static int startMenuState = GAMESTATE_STARTMENU_START;

	UINT width = myRender.GetBufferWidth();
	UINT height = myRender.GetBufferHeight();

	myRender.Clear();

	switch (startMenuState)
	{
	case  GAMESTATE_STARTMENU_START:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			rootState = GAMESTATE_MAINGAME;
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			startMenuState = GAMESTATE_STARTMENU_RULE;
			Sleep(300);
		}

		myRender.DrawPicture(StartMenu_Start, 0, 0, width, height);

		break;
	}
	case GAMESTATE_STARTMENU_RULE:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			rootState = GAMESTATE_RULE;
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_UP))
		{
			startMenuState = GAMESTATE_STARTMENU_START;
			Sleep(300);

		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			startMenuState = GAMESTATE_STARTMENU_EXIT;
			Sleep(300);

		}
		myRender.DrawPicture(StartMenu_Rule, 0, 0, width, height);
		break;
	}
	case GAMESTATE_STARTMENU_EXIT:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			exit(0);
			break;
		}
		if (IS_KEY_DOWN(VK_UP))
		{
			startMenuState = GAMESTATE_STARTMENU_RULE;
			Sleep(300);
		}

		myRender.DrawPicture(StartMenu_Exit, 0, 0, width, height);
		break;
	}

	default:
		DEBUG_MSG1("game state error!!");
		break;
	}


	myRender.Present();
}

void BaseLogic::ruleShow()
{
	static int startMenuState = GAMESTATE_RULE_1;


	UINT width = myRender.GetBufferWidth();
	UINT height = myRender.GetBufferHeight();

	myRender.Clear();

	switch (startMenuState)
	{
	case GAMESTATE_RULE_1:
	{

		if (IS_KEY_DOWN(VK_RETURN))
		{
			startMenuState = GAMESTATE_RULE_2;
			Sleep(300);
		}

		myRender.DrawPicture(GameRule1, 0, 0, width, height);
		myRender.Present();

		break;
	}
	case GAMESTATE_RULE_2:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			startMenuState = GAMESTATE_RULE_3;
			Sleep(300);
		}

		myRender.DrawPicture(GameRule2, 0, 0, width, height);
		myRender.Present();

		break;
	}
	case GAMESTATE_RULE_3:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			rootState = GAMESTATE_STARTMENU;
			startMenuState = GAMESTATE_RULE_1;
			Sleep(300);
		}

		myRender.DrawPicture(GameRule3, 0, 0, width, height);
		myRender.Present();

		break;
	}

	default:
		DEBUG_MSG1("game state error!!");
		break;
	}


}

void BaseLogic::testgame()
{
	
	myRender.Clear(COLOR3(0, 0, 0), TRUE);
	myRender.Clear();
	Material playerMat;
	IPicture playerTex;
	IMesh playerMesh;
	playerMat.ambient = { 0.5f,0.5f,0.5f };
	playerMat.diffuse = { 1.0f,1.0f,1.0f };
	playerMat.specular = { 1.0f,1.0f,1.0f };
	playerMat.specularSmoothLevel = 20;
	playerTex.LoadPPM("Media/1.pbm");
	playerMesh.LoadFile_OBJ("Media/cube.obj");
	playerMesh.SetPosition(-25, 25, 0);
	playerMesh.SetMaterial(playerMat);
	playerMesh.SetTexture(&playerTex);
	myRender.RenderMesh(playerMesh);
	myRender.Present();

	

	//myRender.Clear();
	/*
	tmpMyCube[0].pos.x = -50.0f;
		tmpMyCube[0].pos.y = 50.0f;
		tmpMyCube[0].SetCubePosition(tmpMyCube[0].pos);
	tmpMyCube[0].Render();
		tmpMyCube[1].pos.x = -50.0f;
		tmpMyCube[1].pos.y = 150.0f;
	myRender.SetLight(0, nowLight);
		tmpMyCube[1].SetCubePosition(tmpMyCube[1].pos);
	tmpMyCube[1].Render();
			myRender.Present();
	*/
	//MyCube* tmpMyCube;
	//tmpMyCube = new MyCube[2];
	//spawnMyCube();
	//for (int i=front;i<=back;i++) {
	//	//MyCube_Queue[i].MyCubeMove();
	////MessageBox(0, "1", 0, 0);
	//	MyCube_Queue[i].MyCubeMove();
	//}
	//for (int i = front; i <= back; i++) {
	//	MyCube_Queue[i].Render();
	//}
	//	
	//	for (int i = 1; i <= 200; i++)
	//	{
	//		//MyCube_Queue[1].MyCubeMove();
	//		//MyCube_Queue[1].Render();
	//		myRender.Present();
	//		Sleep(100);
	////		tmpMyCube[1].MyCubeMove();
	////		tmpMyCube[1].Render();
	//////		myRender.Present();
	////		Sleep(100);
	//	}
	//	
	mciSendString("open \"C:\\Users\\阿仓\\Desktop\\Source\\Source\\Media\\music2.mp3\"", 0, 0, 0);
	mciSendString("play \"C:\\Users\\阿仓\\Desktop\\Source\\Source\\Media\\music2.mp3\"", NULL, 0, NULL);
	//mciSendString("close \"C:\\Users\\阿仓\\Desktop\\Source\\Source\\Media\\music2.mp3\"", NULL, 0, NULL);
	Sleep(50000);
}

void BaseLogic::wholeGame()
{
	while (1)
	{
		mciSendString("open \"Media\\music2.mp3\"", 0, 0, 0);
		mciSendString("play \"Media\\music2.mp3\" repeat", NULL, 0, NULL);
		/*
		mciSendString("open \"C:\\Users\\阿仓\\Desktop\\Source\\Source\\Media\\music2.mp3\"", 0, 0, 0);
		mciSendString("play \"C:\\Users\\阿仓\\Desktop\\Source\\Source\\Media\\music2.mp3\" repeat", NULL, 0, NULL);*/
		//mciSendString("close \"C:\\Users\\阿仓\\Desktop\\Source\\Source\\Media\\music2.mp3\"", NULL, 0, NULL);

		switch (UINT(rootState))
		{
		case GAMESTATE_STARTMENU:
			startMenu();
			break;

		case GAMESTATE_MAINGAME:
			gameBegin();
			break;

		case GAMESTATE_PAUSEMENU:
			pauseMenu();
			break;

		case GAMESTATE_RULE:
			ruleShow();
			break;

		case GAMESTATE_GAMECONTINUE:
			gameContinue();
			break;
		default:
			DEBUG_MSG1("game state error!!");
			exit(0);
			break;
		}
	}
	
}






