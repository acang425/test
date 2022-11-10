#pragma once
#include"MyConsoleEngine.h"
namespace MyGame {


	class MyPlayer
	{
	public:


		IMesh playerMesh;//ģ��
		VEC3 topPoint;//��ײ�ж���
		VEC3 bottomPoint;
		bool enableMove;//�ʲ������ƶ�
		Material playerMat;//����
		IPicture playerTex;//��ͼ
		VEC3 playerPos;//λ��

		MyPlayer();
		void Render();
		void playerMove();
		void SetPosition(VEC3);
	};
};

