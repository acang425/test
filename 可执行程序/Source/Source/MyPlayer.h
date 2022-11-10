#pragma once
#include"MyConsoleEngine.h"
namespace MyGame {


	class MyPlayer
	{
	public:


		IMesh playerMesh;//模型
		VEC3 topPoint;//碰撞判定点
		VEC3 bottomPoint;
		bool enableMove;//允不允许移动
		Material playerMat;//材质
		IPicture playerTex;//贴图
		VEC3 playerPos;//位置

		MyPlayer();
		void Render();
		void playerMove();
		void SetPosition(VEC3);
	};
};

