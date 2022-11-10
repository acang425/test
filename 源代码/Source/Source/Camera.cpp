/************************************************************************		
����YawPitchRoll�����Ƕ��壺������ϵ��
			Yaw:����ͼ˳ʱ��ת
			Pitch:y�����ķ���ת����ͷ��
			Roll:��ʱ��ת
************************************************************************/

#pragma once

#include "MyConsoleEngine.h"

using namespace Math;

ICamera::ICamera()
{
	//��ת��
	mRotateX_Pitch=0;
	mRotateY_Yaw=0;
	mRotateZ_Roll=0;
	//͸��ͶӰ�任�������
	mViewAngleY = 60.0f/180.0f * PAI_CIRCLE;
	mAspectRatio = 1.5;
	mFarPlane= 1500.0f;
	mNearPlane=1.0f;
	//���λ�ã�����ӵ㣬�������ķ���
	mPosition = VEC3(0,0,0);
	mLookat = VEC3(1,0,0);
	mDirection = VEC3(1,0,0);

	//��ʼ�� ͶӰ͸�ӱ任����
	mMatrixProjection= Matrix_PerspectiveProjection(mViewAngleY,mAspectRatio,mNearPlane,mFarPlane);
	//��ʼ�� ��ͼ����
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
	//lookat��λ�ò����غϰ�
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

void	ICamera::SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll)//Ҫ����Lookat
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
	//roll��������Ҫ����lookat
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


//����ͶӰ͸�ӱ任����
void	ICamera::mFunction_UpdateProjMatrix()
{
	mMatrixProjection=Matrix_PerspectiveProjection(
		mViewAngleY,
		mAspectRatio*c_ConsoleCharAspectRatio,
		mNearPlane,
		mFarPlane);

};
//������ͼ����
void	ICamera::mFunction_UpdateViewMatrix()
{

	MATRIX4	tmpMatrixTranslation;
	MATRIX4	tmpMatrixRotation;
	//�ȶ���ԭ��
	
	tmpMatrixTranslation=Matrix_Translation(-mPosition.x, -mPosition.y, -mPosition.z);
	//Ȼ���� yawpitchroll������ ת��view�ռ�
	tmpMatrixRotation = Matrix_YawPitchRoll(mRotateY_Yaw,mRotateX_Pitch, mRotateZ_Roll);
	//���������ת������
	tmpMatrixRotation=Matrix_Transpose(tmpMatrixRotation);
	//��ƽ�ƣ�����ת (column vector)
	mMatrixView= Matrix_Multiply(tmpMatrixRotation, tmpMatrixTranslation);
};

void	ICamera::mFunction_UpdateRotation()
{
	//��Ҫ���ܣ����������Ҫ��Ϊ�˴���Direction�ı��������̬�Ǳ仯

	//����direction
	VEC3	tmpDirection;
	//�����direction�Ƿ�Ϊ0
	tmpDirection=mLookat-mPosition;
	float mLength = VEC3_Length(tmpDirection);
	//ע�⸡�������ӵ��λ�ò����غ�
	if (mLength<0.001f)
	{
		//�ӵ��λ���غϣ������������
		mRotateX_Pitch = 0;
		mRotateY_Yaw = 0;
		mRotateZ_Roll = 0;
		mDirection = VEC3(1.0f, 0, 0);
		mLookat = mPosition + mDirection;
		return;
	}
	else
	//�ӵ��λ�ò��غϣ��ٸ�ֵ
	{ mDirection = tmpDirection; }
	;

	//��ʱ����������ı�ֵ����arctan���Pitch�Ƕ�
	float tmpRatio;
	//pitch�ǣ� tan = y/sqr(x^2+z^2))
	/*	ע�⣺	atanȡֵ��Χ�� [-pi/2,pi/2]  
					atan2ȡֵ��Χ�� [-pi,pi] 		*/
	if((mDirection.x==0) && (mDirection.z==0))
	{
		//�Ƿ���ԭ��������·�
		if(mDirection.y>=0)
		{mRotateX_Pitch=-PAI_CIRCLE/2;}
		else
		{mRotateX_Pitch= PAI_CIRCLE /2;}
	}
	else
	{
		//y����ʱ�������ǡ�������DX������ϵ������ϵ��ת�����������־���
		//�ô�Ĵָָ����ת��������ָָ������ת����
		tmpRatio =-mDirection.y /  sqrt(pow(mDirection.x,2.0f)+pow(mDirection.z,2.0f));
		mRotateX_Pitch = atan(tmpRatio);
		//����[-0.5pi,0.5pi]
	}

	//yaw�ǣ� tan = -x/z
		mRotateY_Yaw = atan2(mDirection.x,mDirection.z);//����ͼyaw��˳ʱ������

	//roll�ǣ�����direction����ı�roll�� 
	//roll��ʱ��ת������
};

void	ICamera::mFunction_UpdateDirection()
{
	//��Ҫ���ܣ����������Ҫ��Ϊ�˴�����̬�Ǹı����������Direction�仯

	//Ҫ����Lookat
	float tmpDirectionLength = VEC3_Length(mDirection);
	mDirection.x = tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	mDirection.z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	mDirection.y =-tmpDirectionLength* sin(mRotateX_Pitch);
	mLookat=mPosition+mDirection;
};
