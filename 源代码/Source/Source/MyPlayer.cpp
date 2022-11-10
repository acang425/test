#include "MyGame.h"
using namespace MyGame;


MyPlayer::MyPlayer() {
	//材质，贴图，模型初始化
	playerMat.ambient = { 0.5f,0.5f,0.5f };
	playerMat.diffuse = { 1.0f,1.0f,1.0f };
	playerMat.specular = { 1.0f,1.0f,1.0f };
	playerMat.specularSmoothLevel = 20;
	playerTex.LoadPPM("Media/keli.pbm");
	playerMesh.LoadFile_OBJ("Media/player.obj");
	playerMesh.SetPosition(-25, 25, 0);
	playerMesh.SetMaterial(playerMat);
	playerMesh.SetTexture(&playerTex);
	//碰撞判定点
	VEC3 topPoint = VEC3(-3*CUBE_LENGTH/2, 3 * CUBE_LENGTH / 2, 0);
	VEC3 bottomPoint = VEC3(-3 * CUBE_LENGTH / 2, CUBE_LENGTH / 2, 0);
	enableMove = 1;
	playerPos = VEC3(-3 * CUBE_LENGTH / 2,  CUBE_LENGTH, 0);
}

void MyPlayer::Render()
{
	myRender.RenderMesh(playerMesh);
}
void MyPlayer::playerMove() 
{
	//按上下左右键移动，且不能超出活动范围，并更新判定点
	if (IS_KEY_DOWN(VK_UP))
	{
		if (enableMove)
		{
			if (playerPos.y + PLAYER_HEIGHT / 2 <= 2 * CUBE_LENGTH)
			{
				playerPos.y += PLAYER_STEP;
			}
			if (playerPos.y - PLAYER_HEIGHT / 2 >= CUBE_LENGTH)
			{
				bottomPoint = VEC3(playerPos.x, CUBE_LENGTH * 4, playerPos.z);
				topPoint = VEC3(playerPos.x, 3 * CUBE_LENGTH / 2, playerPos.z);
			}
		}
		
	}
	if (IS_KEY_DOWN(VK_DOWN))
	{
		if (enableMove)
		{
			if (playerPos.y - PLAYER_HEIGHT / 2 >= 0)
			{
				playerPos.y -= PLAYER_STEP;
			}
			if (playerPos.y + PLAYER_HEIGHT / 2 <= CUBE_LENGTH)
			{
				bottomPoint = VEC3(playerPos.x, CUBE_LENGTH / 2, playerPos.z);
				topPoint = VEC3(playerPos.x, CUBE_LENGTH * 4, playerPos.z);
			}
		}
	}
	if (IS_KEY_DOWN(VK_LEFT))
	{
		if (enableMove)
		{
			if (playerPos.x -PLAYER_WIDTH/2 +  CUBE_LENGTH * 2 >= 0)
			{
				playerPos.x -= PLAYER_STEP;
			}
			if (1)
			{
				bottomPoint = VEC3(playerPos.x, CUBE_LENGTH / 2, playerPos.z);
				topPoint = VEC3(playerPos.x, 3 * CUBE_LENGTH / 2, playerPos.z);
			}
		}
		
	}
	if (IS_KEY_DOWN(VK_RIGHT))
	{
		if (enableMove)
		{
			if (playerPos.x + PLAYER_WIDTH / 2 - CUBE_LENGTH * 2 <= 0)
			{
				playerPos.x += PLAYER_STEP;
			}
			if (1)
			{
				bottomPoint = VEC3(playerPos.x, CUBE_LENGTH / 2, playerPos.z);
				topPoint = VEC3(playerPos.x, 3 * CUBE_LENGTH / 2, playerPos.z);
			}
		}
	}
	SetPosition(playerPos);
}
void MyPlayer::SetPosition(VEC3 Vpos)
{
	playerMesh.SetPosition(Vpos);
}