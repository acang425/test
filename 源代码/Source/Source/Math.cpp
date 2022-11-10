
#include "MyConsoleEngine.h"

//----------------------------向量运算------------------------

float Math::Vec2_Dot(const VEC2& vec1, const VEC2& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
};

float Math::Vec2_Length(const VEC2& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

float Math::VEC3_Dot(const VEC3& vec1, const VEC3& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
};

VEC3 Math::VEC3_Cross(const VEC3& vec1, const VEC3& vec2)
{
	return VEC3(
		vec1.y * vec2.z - vec1.z * vec2.x,
		vec1.z * vec2.x - vec1.x * vec2.z,
		vec1.x * vec2.y - vec1.y * vec2.x);
};

float Math::VEC3_Length(const VEC3& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

VEC3 Math::VEC3_Reflect(const VEC3& vec, const VEC3& axis)
{
	VEC3 In = vec;
	VEC3 Out = (-1) * In + 2 * (In - VEC3_Dot(In, axis) / In.Length() * axis);
	return Out;
}

float Math::Vec4_Dot(const VEC4& vec1, const VEC4& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
};

float Math::Vec4_Length(const VEC4& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}


//----------------------------矩阵运算------------------------

inline MATRIX4 Math::Matrix_Multiply(const MATRIX4& leftMat, const MATRIX4& rightMat)
{
	MATRIX4 outMat;
	for (UINT i = 0; i < 4; ++i)
	{
		for (UINT j = 0; j < 4; ++j)
		{
			VEC4 row_i = leftMat.m[i];
			VEC4 column_j = { rightMat.m[0][j],rightMat.m[1][j],rightMat.m[2][j],rightMat.m[3][j] };
			outMat.m[i][j] = Vec4_Dot(row_i, column_j);
		}
	}
	return outMat;
};

inline VEC4	Math::Matrix_Multiply(const MATRIX4 leftMat, const VEC4& rightColumnVec)
{
	VEC4 outVec;
	outVec.x = leftMat.m[0][0] * rightColumnVec.x + leftMat.m[0][1] * rightColumnVec.y +
		+leftMat.m[0][2] * rightColumnVec.z + leftMat.m[0][3] * rightColumnVec.w;
	outVec.y = leftMat.m[1][0] * rightColumnVec.x + leftMat.m[1][1] * rightColumnVec.y +
		+leftMat.m[1][2] * rightColumnVec.z + leftMat.m[1][3] * rightColumnVec.w;
	outVec.z = leftMat.m[2][0] * rightColumnVec.x + leftMat.m[2][1] * rightColumnVec.y +
		+leftMat.m[2][2] * rightColumnVec.z + leftMat.m[2][3] * rightColumnVec.w;
	outVec.w = leftMat.m[3][0] * rightColumnVec.x + leftMat.m[3][1] * rightColumnVec.y +
		+leftMat.m[3][2] * rightColumnVec.z + leftMat.m[3][3] * rightColumnVec.w;
	return outVec;
};


MATRIX4	Math::Matrix_Translation(float dx, float dy, float dz)
{
	MATRIX4 outMatrix;
	outMatrix.SetMatrix(
		{
			{ 1,0,0,dx },
			{ 0,1,0,dy },
			{ 0,0,1,dz },
			{ 0,0,0,1 }
		});
	return outMatrix;
}

MATRIX4	Math::Matrix_RotationX(float angle)
{
	MATRIX4 outMatrix;
	outMatrix.SetMatrix({
		{ 1,			0,					0,					0 },
		{ 0,			cos(angle),	-sin(angle),	0 },
		{ 0,			sin(angle),	cos(angle),	0 },
		{ 0,			0,					0,					1 }
		});
	return outMatrix;
};

MATRIX4	Math::Matrix_RotationY(float angle)
{
	MATRIX4 outMatrix;
	outMatrix.SetMatrix({
	{ cos(angle),	0,			sin(angle),	0 },
	{ 0,				1,			0,					0 },
	{ -sin(angle),0,			cos(angle),	0 },
	{ 0,				0,			0,					1 }
		});
	return outMatrix;
};

MATRIX4	Math::Matrix_RotationZ(float angle)
{
	MATRIX4 outMatrix;
	outMatrix.SetMatrix({
	{ cos(angle),	-sin(angle),	0,		0 },
	{ sin(angle),	cos(angle),	0,		0 },
	{ 0,					0,				1,		0 },
	{ 0,				0,			0,					1 }
		});
	return outMatrix;
};

MATRIX4	Math::Matrix_YawPitchRoll(float yaw_Y, float pitch_X, float roll_Z)
{
	MATRIX4 matRotateX;
	MATRIX4 matRotateY;
	MATRIX4 matRotateZ;

	matRotateX = Matrix_RotationX(pitch_X);
	matRotateY = Matrix_RotationY(yaw_Y);
	matRotateZ = Matrix_RotationZ(roll_Z);

	//把三轴的旋转矩阵乘在一起
	MATRIX4 outMatrix;
	outMatrix = Matrix_Multiply(matRotateY, Matrix_Multiply(matRotateX, matRotateZ));
	return outMatrix;
}

MATRIX4	Math::Matrix_PerspectiveProjection(float ViewAngleY, float aspectRatio, float NearPlaneZ, float FarPlaneZ)
{
	//投影透视矩阵，套公式就完事了
	float term11 = 1.0f / (aspectRatio * tanf(ViewAngleY / 2.0f));
	float term22 = 1.0f / tanf(ViewAngleY / 2.0f);
	float term33 = 1.0f / (FarPlaneZ - NearPlaneZ);
	float term34 = -NearPlaneZ / (FarPlaneZ - NearPlaneZ);

	MATRIX4 outMatrix;
	outMatrix.SetRow(0, { term11,	0,				0,				0 });
	outMatrix.SetRow(1, { 0,			term22,		0,				0 });
	outMatrix.SetRow(2, { 0,			0,				term33,		term34 });
	outMatrix.SetRow(3, { 0,			0,				1,				0 });

	return outMatrix;
};

//转置矩阵
inline MATRIX4 Math::Matrix_Transpose(const MATRIX4& inMatrix)
{
	MATRIX4 outMatrix = inMatrix;
	std::swap(outMatrix.m[0][1], outMatrix.m[1][0]);
	std::swap(outMatrix.m[0][2], outMatrix.m[2][0]);
	std::swap(outMatrix.m[0][3], outMatrix.m[3][0]);
	std::swap(outMatrix.m[2][1], outMatrix.m[1][2]);
	std::swap(outMatrix.m[3][1], outMatrix.m[1][3]);
	std::swap(outMatrix.m[3][2], outMatrix.m[2][3]);
	return outMatrix;
}
//----------------------------限定范围------------------------
UINT Math::Limited(UINT val, UINT min, UINT max)
{
	if (val > max)val = max;
	if (val < min)val = min;
	return val;
}

inline float Math::Limited(float val, float min, float max)
{
	if (val > max)val = max;
	if (val < min)val = min;
	return val;
}

inline VEC2 Math::Limited(const VEC2& val, const VEC2& min, const VEC2& max)
{
	return VEC2(Limited(val.x, min.x, max.x), Limited(val.y, min.y, max.y));
}

inline VEC3 Math::Limited(const VEC3& val, const VEC3& min, const VEC3& max)
{
	return VEC3(Limited(val.x, min.x, max.x), Limited(val.y, min.y, max.y), Limited(val.z, min.z, max.z));
}
//----------------------------插值------------------------
inline BYTE Math::Lerp(BYTE start, BYTE end, float t)
{
	return char(start * (1.0f - t) + end * t);
}

inline float Math::Lerp(float start, float end, float t)
{
	return start * (1.0f - t) + end * t;
}

inline VEC2 Math::Lerp(const VEC2& start, const VEC2& end, float t)
{
	return VEC2(Lerp(start.x, end.x, t), Lerp(start.y, end.y, t));
}

inline VEC3 Math::Lerp(const VEC3& start, const VEC3& end, float t)
{
	return VEC3(Lerp(start.x, end.x, t), Lerp(start.y, end.y, t), Lerp(start.z, end.z, t));
}


