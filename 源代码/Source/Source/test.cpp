#include "test.h"
#include<iostream>
test::test()
{
	tRenderer.Init(1252, 758);
	tRenderer.SetWindowTitle(" ");
	tRenderer.SetCamera(tCamera);

	//------------------CAMERA--------------------
	tCamera.SetPosition(0, 100.0f, -300.0f);
	tCamera.SetLookAt(0, 0, 0);
	tCamera.SetViewAngle(Math::PAI_CIRCLE / 2.0f, 2.5f);
	tCamera.SetViewFrustumPlane(1.0f, 500.0f);

	//------------------Init Art Resources------------------
	 tDefaultMaterial.ambient = { 0.5f,0.5f,0.5f };
	 tDefaultMaterial.diffuse = { 1.0f,1.0f,1.0f };
	 tDefaultMaterial.specular = { 1.0f,1.0f,1.0f };
	 tDefaultMaterial.specularSmoothLevel = 20;
	 tTestPic.LoadPPM("Media/GameRule.pbm");
	 tModel1.LoadFile_OBJ("Media/cube.obj");
	 tModel1.SetPosition(-50, 0, 0);
	 tModel1.SetMaterial( tDefaultMaterial);
	 tModel1.SetTexture(& tTestPic);


	 tDefaultMaterial.ambient = { 0.5f,0.5f,0.5f };
	 tDefaultMaterial.diffuse = { 1.0f,1.0f,1.0f };
	 tDefaultMaterial.specular = { 1.0f,1.0f,1.0f };
	 tDefaultMaterial.specularSmoothLevel = 20;
	 tTestPic1.LoadPPM("Media/blue.pbm");
	 tModel2.LoadFile_OBJ("Media/player.obj");
	 tModel2.SetPosition(50, 0, 0);
	 tModel2.SetMaterial(tDefaultMaterial);
	 tModel2.SetTexture(&tTestPic1);

	//-----------------Lights----------------------
	 tLight1.mAmbientColor = { 1.0f,1.0f,1.0f };
	 tLight1.mDiffuseColor = { 1.0f,1.0f,1.0f };
	 tLight1.mDiffuseIntensity = 1.0f;
	 tLight1.mDirection = { -1.0f,-1.0f,-1.0f };
	 tLight1.mIsEnabled = TRUE;
	 tLight1.mSpecularColor = { 1.0f,1.0f,1.0f };
	 tLight1.mSpecularIntensity = 1.2f;
	tRenderer.SetLight(0,  tLight1);

	//-----------------Init Cursor--------------------
	::SetCursorPos(0, 0);
}
void test::init()
{
    
    tRenderer.Clear(COLOR3(0, 0, 0), TRUE);
    //tRenderer.RenderMesh(tModel1);
   // tRenderer.RenderMesh(tModel2);
    tRenderer.DrawPicture( tTestPic, 0, 0, 250, 100);
    tRenderer.Present();
    tRenderer.Clear(COLOR3(0, 0, 0), TRUE);
    
        

}

