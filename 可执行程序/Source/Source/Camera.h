
#pragma once

using namespace Math;

	class ICamera 
	{
	public:

		ICamera();

		void				Destroy();

		void				SetLookAt(VEC3 vLookat);//要更新旋转角

		void				SetLookAt(float x, float y, float z);//要更新旋转角

		VEC3		GetLookAt();

		void				SetPosition(VEC3 vPos);

		void				SetPosition(float x, float y, float z);

		VEC3		GetPosition();

		VEC3		GetDirection();

		void				Move(VEC3 vRelativePos);

		void				Move(float relativeX, float relativeY, float relativeZ);

		void				SetViewFrustumPlane(float iNearPlaneZ, float iFarPlaneZ);

		void				SetViewAngle(float iViewAngleY, float iAspectRatio);

		void				SetRotation(float RX_Pitch, float RY_Yaw, float RZ_Roll);//要更新Lookat

		void				SetRotationY_Yaw(float AngleX);//用setRotation呗

		void				SetRotationX_Pitch(float AngleY);

		void				SetRotationZ_Roll(float AngleZ);

		float				GetRotationY_Yaw();

		float				GetRotationX_Pitch();

		float				GetRotationZ_Roll();

		void				RotateY_Yaw(float angleY);

		void				RotateX_Pitch(float angleX);

		void				RotateZ_Roll(float angleZ);

		void				GetViewMatrix(MATRIX4& outMat);

		void				GetProjMatrix(MATRIX4& outMat);

	private:

		void		mFunction_UpdateProjMatrix();
		void		mFunction_UpdateViewMatrix();
		void		mFunction_UpdateRotation();
		void		mFunction_UpdateDirection();
		float		mViewAngleY;
		float		mAspectRatio;
		float		mNearPlane;
		float		mFarPlane;

		VEC3		mPosition;
		VEC3		mLookat;
		VEC3		mDirection;

		MATRIX4	mMatrixView;
		MATRIX4	mMatrixProjection;

		float			mRotateX_Pitch;
		float			mRotateY_Yaw;
		float			mRotateZ_Roll;

	};
