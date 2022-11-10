/************************************************************************		
关于YawPitchRoll的正角定义：（左手系）
			Yaw:俯视图顺时针转
			Pitch:y往负的方向转（低头）
			Roll:逆时针转
************************************************************************/

#pragma once

#include "MyConsoleEngine.h"

using namespace Math;

ICamera::ICamera()
{
	//旋转角
	mRotateX_Pitch=0;
	mRotateY_Yaw=0;
	mRotateZ_Roll=0;
	//透视投影变换矩阵参数
	mViewAngleY = 60.0f/180.0f * PAI_CIRCLE;
	mAspectRatio = 1.5;
	mFarPlane= 1500.0f;
	mNearPlane=1.0f;
	//相机位置，相机视点，相机看向的方向
	mPosition = VEC3(0,0,0);
	mLookat = VEC3(1,0,0);
	mDirection = VEC3(1,0,0);

	//初始化 投影透视变换矩阵
	mMatrixProjection= Matrix_PerspectiveProjection(mViewAngleY,mAspectRatio,mNearPlane,mFarPlane);
	//初始化 视图矩阵
	mMatrixView.Identity();
};

void ICamera::Destroy()
{
};

void	ICamera::SetLookAt(VEC3 vLookat)
{
	mLookat=vLookat;
	mFunction_UpdateRotation();
};

void	ICamera::SetLookAt(float x,float y,float z)
{
	VEC3 tmpLookat(x,y,z);
	mLookat=tmpLookat;
	mFunction_UpdateRotation();
};

VEC3	ICamera::GetLookAt()
{
	return  mLookat;
};

void	ICamera::SetPosition(VEC3 vPos)
{
	//lookat和位置不能重合啊
		mPosition=vPos;
		mFunction_UpdateRotation();
}

void	ICamera::SetPosition(float x,float y,float z)
{
	VEC3 tmpPos(x,y,z);
	mPosition=tmpPos;
	mFunction_UpdateRotation();
};

VEC3 ICamera::GetPosition()
{
	return mPosition;
}
VEC3 ICamera::GetDirection()
{
	return mLookat - mPosition;
};

void	ICamera::Move(VEC3 vRelativePos)
{
	mPosition+=vRelativePos;
	mLookat+=vRelativePos;

};

void	ICamera::Move(float relativeX,float relativeY,float relativeZ)
{
	mPosition += VEC3(relativeX, relativeY, relativeZ);
};

void	ICamera::SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll)//要更新Lookat
{
	SetRotationX_Pitch(RX_Pitch);
	SetRotationY_Yaw(RY_Yaw);
	SetRotationZ_Roll(RZ_Roll);
	mFunction_UpdateDirection();
};

void	ICamera::SetRotationY_Yaw(float angleY)
{
	mRotateY_Yaw = angleY;
	mFunction_UpdateDirection();
};

void	ICamera::SetRotationX_Pitch(float AngleX)
{
	//Limited to [-pi/2,pi/2]
	mRotateX_Pitch = Limited(AngleX, -(PAI_CIRCLE / 2), (PAI_CIRCLE / 2));

	mFunction_UpdateDirection();
};

void	ICamera::SetRotationZ_Roll(float AngleZ)
{
	//roll翻滚不需要更新lookat
	mRotateZ_Roll = AngleZ;
}

float ICamera::GetRotationY_Yaw()
{
	return mRotateY_Yaw;
}

float ICamera::GetRotationX_Pitch()
{
	return mRotateX_Pitch;
}

float ICamera::GetRotationZ_Roll()
{
	return mRotateZ_Roll;
}

void ICamera::RotateY_Yaw(float angleY)
{
	SetRotationY_Yaw(mRotateY_Yaw + angleY);
	mFunction_UpdateDirection();
};

void ICamera::RotateX_Pitch(float angleX)
{
	float newAngle = mRotateX_Pitch + angleX;
	SetRotationX_Pitch(newAngle);
	mFunction_UpdateDirection();
};

void ICamera::RotateZ_Roll(float angleZ)
{
	SetRotationZ_Roll(mRotateZ_Roll + angleZ);
	mFunction_UpdateDirection();
};


void ICamera::GetViewMatrix(MATRIX4 & outMat)
{
	mFunction_UpdateViewMatrix();
	outMat = mMatrixView;
}

void ICamera::GetProjMatrix(MATRIX4 & outMat)
{
	mFunction_UpdateProjMatrix();
	outMat = mMatrixProjection;
};




void	ICamera::SetViewFrustumPlane(float iNearPlaneZ,float iFarPlaneZ)
{
	if ( (iNearPlaneZ >0) && (iFarPlaneZ>iNearPlaneZ))
	{
		mNearPlane	= iNearPlaneZ;
		mFarPlane	=	iFarPlaneZ;
	}

};

void ICamera::SetViewAngle(float iViewAngleY,float iAspectRatio)
{
	if(iViewAngleY>0 && (mViewAngleY <(PAI_CIRCLE/2.0f))){mViewAngleY	=	iViewAngleY;	}
	if(iAspectRatio>0){mAspectRatio	= iAspectRatio;}
};


//更新投影透视变换矩阵
void	ICamera::mFunction_UpdateProjMatrix()
{
	mMatrixProjection=Matrix_PerspectiveProjection(
		mViewAngleY,
		mAspectRatio*c_ConsoleCharAspectRatio,
		mNearPlane,
		mFarPlane);

};
//更新视图矩阵
void	ICamera::mFunction_UpdateViewMatrix()
{

	MATRIX4	tmpMatrixTranslation;
	MATRIX4	tmpMatrixRotation;
	//先对齐原点
	
	tmpMatrixTranslation=Matrix_Translation(-mPosition.x, -mPosition.y, -mPosition.z);
	//然后用 yawpitchroll的逆阵 转到view空间
	tmpMatrixRotation = Matrix_YawPitchRoll(mRotateY_Yaw,mRotateX_Pitch, mRotateZ_Roll);
	//正交矩阵的转置是逆
	tmpMatrixRotation=Matrix_Transpose(tmpMatrixRotation);
	//先平移，再旋转 (column vector)
	mMatrixView= Matrix_Multiply(tmpMatrixRotation, tmpMatrixTranslation);
};

void	ICamera::mFunction_UpdateRotation()
{
	//主要功能：这个函数主要是为了处理Direction改变带来的姿态角变化

	//更新direction
	VEC3	tmpDirection;
	//检查新direction是否为0
	tmpDirection=mLookat-mPosition;
	float mLength = VEC3_Length(tmpDirection);
	//注意浮点数误差，视点和位置不能重合
	if (mLength<0.001f)
	{
		//视点和位置重合，重置摄像机啊
		mRotateX_Pitch = 0;
		mRotateY_Yaw = 0;
		mRotateZ_Roll = 0;
		mDirection = VEC3(1.0f, 0, 0);
		mLookat = mPosition + mDirection;
		return;
	}
	else
	//视点和位置不重合，再赋值
	{ mDirection = tmpDirection; }
	;

	//临时变量，储存的比值用来arctan获得Pitch角度
	float tmpRatio;
	//pitch角： tan = y/sqr(x^2+z^2))
	/*	注意：	atan取值范围是 [-pi/2,pi/2]  
					atan2取值范围是 [-pi,pi] 		*/
	if((mDirection.x==0) && (mDirection.z==0))
	{
		//是否在原点的正上下方
		if(mDirection.y>=0)
		{mRotateX_Pitch=-PAI_CIRCLE/2;}
		else
		{mRotateX_Pitch= PAI_CIRCLE /2;}
	}
	else
	{
		//y负的时候是正角————DX用左手系，左手系旋转正方向用左手决定
		//用大拇指指着旋转轴正向，四指指向正旋转方向
		tmpRatio =-mDirection.y /  sqrt(pow(mDirection.x,2.0f)+pow(mDirection.z,2.0f));
		mRotateX_Pitch = atan(tmpRatio);
		//返回[-0.5pi,0.5pi]
	}

	//yaw角： tan = -x/z
		mRotateY_Yaw = atan2(mDirection.x,mDirection.z);//俯视图yaw是顺时针正角

	//roll角：更新direction不会改变roll角 
	//roll逆时针转是正角
};

void	ICamera::mFunction_UpdateDirection()
{
	//主要功能：这个函数主要是为了处理姿态角改变带来的视线Direction变化

	//要更新Lookat
	float tmpDirectionLength = VEC3_Length(mDirection);
	mDirection.x = tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	mDirection.z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	mDirection.y =-tmpDirectionLength* sin(mRotateX_Pitch);
	mLookat=mPosition+mDirection;
};
