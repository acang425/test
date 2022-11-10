#pragma once
#define IsKeyDown(VK_NAME) ((GetAsyncKeyState(VK_NAME) & 0x8000) ? true:false)   
#include "MyConsoleEngine.h"


class test
{
public:
	IRenderer			tRenderer;

	ICamera				tCamera;
	Material tDefaultMaterial;
	IPicture tTestPic;
	IPicture tTestPic1;
	IMesh	  tModel1;
	IMesh	  tModel2;
	DirectionalLight tLight1;
	test();
	void init();
	
private:
};

