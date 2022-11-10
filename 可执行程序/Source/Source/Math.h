#pragma once

namespace Math
{
	//************************数学类型与常量*******************************
	const float PAI_CIRCLE = 3.1415926f;

		//----------------------二维向量-------------------------
	struct VEC2
	{
		VEC2() { };
		VEC2(float X, float Y) { x = X; y = Y; };
		VEC2(const float column[2])
		{
			x = column[0];
			y = column[1];
		}

		float	Length()//长度
		{
			return sqrt(x*x + y*y);
		};
		VEC2&	GuiYiHua()//单位化
		{
			float len = Length();
			if (len != 0)
			{
				x /= len;
				y /= len;
			}
			else
			{
				x = 0.0f;
				y = 0.0f;
			}
			return *this;
		}
		//----------------------重载运算符--------------------------
		BOOL	operator==(const VEC2& vec)
		{
			if (vec.x == x&&vec.y == y)
				return TRUE;
			else
				return FALSE;
		}

		VEC2 operator+(const VEC2& vec)
		{
			return VEC2(x + vec.x, y + vec.y);
		}

		VEC2 operator-(const VEC2& vec)
		{
			return VEC2(x - vec.x, y - vec.y);
		}

		VEC2& operator+=(const VEC2& vec)
		{
			x += vec.x;
			y += vec.y;
			return *this;
		}

		VEC2& operator-=(const VEC2& vec)
		{
			x -= vec.x;
			y -= vec.y;
			return *this;
		}

		VEC2 operator*(const float scaleFactor)
		{
			return VEC2(scaleFactor*x, scaleFactor* y);
		}

		friend VEC2 operator*(float scaleFactor, const VEC2& vec)
		{
			return VEC2(scaleFactor*vec.x, scaleFactor* vec.y);
		}


		VEC2& operator*=(const float scaleFactor)
		{
			x *= scaleFactor;
			y *= scaleFactor;
			return *this;
		}

		float x;
		float y;
	};
	//----------------------三维向量--------------------------
	struct VEC3
	{
		VEC3() { };
		VEC3(float X, float Y, float Z) { x = X; y = Y; z = Z; };
		VEC3(const float column[3])
		{
			x = column[0];
			y = column[1];
			z = column[2];
		}

		float	Length() const//长度
		{
			return sqrtf(x*x + y*y + z*z );
		};

		VEC3&	GuiYiHua()//单位化
		{
			float len = Length();
			if (len != 0)
			{
				x /= len;
				y /= len;
				z /= len;
			}
			else
			{
				x = y = z = 0.0f;
			}
			return *this;
		}

		//-----------------------重载运算符-------------------------
		BOOL	operator==(const VEC3& vec) 
		{
			if (vec.x == x&&vec.y == y&&vec.z == z)
				return TRUE;
			else
				return FALSE;
		}

		VEC3 operator+(const VEC3& vec)
		{
			return VEC3(x + vec.x, y + vec.y, z + vec.z);
		}

		VEC3 operator-(const VEC3& vec)
		{
			return VEC3(x - vec.x, y - vec.y, z - vec.z);
		}

		friend VEC3 operator-(const VEC3& v1, const VEC3& v2)
		{
			return VEC3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
		};

		VEC3& operator+=(const VEC3& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}

		VEC3& operator-=(const VEC3& vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			return *this;
		}

		VEC3 operator*(const float scaleFactor)
		{
			return VEC3(scaleFactor*x, scaleFactor* y, scaleFactor* z);
		}

		VEC3 operator*(const VEC3& vec)
		{
			return VEC3(vec.x*x, vec.y*y, vec.z*z);
		}

		friend VEC3 operator*(float scaleFactor,const VEC3& vec)
		{
			return VEC3(scaleFactor*vec.x, scaleFactor* vec.y, scaleFactor* vec.z);
		}

		VEC3& operator*=(const float scaleFactor)
		{
			x *=scaleFactor;
			y *= scaleFactor;
			z *=scaleFactor;
			return *this;
		}

		float x;
		float y;
		float z;
	};
	//----------------------四维向量--------------------------
	struct VEC4
	{
		VEC4() { };
		VEC4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; };
		VEC4(const float column[4]) 
		{
			x = column[0];
			y = column[1];
			z = column[2];
			w = column[3];
		}

		float	Length()//长度
		{
			return sqrt(x*x + y*y + z*z + w*w);
		};

		VEC4&	GuiYiHua()//单位化
		{
			float len = Length();
			if (len != 0.0f)
			{
				x /= len;
				y /= len;
				z /= len;
				w /= len;
			}
			else
			{
				x = y = z = w = 0.0f;
			}
			return *this;
		}

		BOOL	operator==(const VEC4& vec)
		{
			if (vec.x == x&&vec.y == y&&vec.z == z&&vec.w==w)
				return TRUE;
			else
				return FALSE;
		}

		VEC4 operator+(const VEC4& vec)
		{
			return VEC4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
		}

		VEC4 operator-(const VEC4& vec)
		{
			return VEC4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
		}

		VEC4& operator+=(const VEC4& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
			w += vec.w;
			return *this;
		}

		VEC4& operator-=(const VEC4& vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			w -= vec.w;
			return *this;
		}

		VEC4 operator*(const float scaleFactor)
		{
			return VEC4(scaleFactor*x, scaleFactor* y, scaleFactor* z, scaleFactor* w);
		}

		friend VEC4 operator*(float scaleFactor, const VEC4& vec)
		{
			return VEC4(scaleFactor*vec.x, scaleFactor* vec.y, scaleFactor* vec.z,scaleFactor*vec.w);
		}

		VEC4& operator*=(const float scaleFactor)
		{
			x *= scaleFactor;
			y *= scaleFactor;
			z *= scaleFactor;
			w *= scaleFactor;
			return *this;
		}

		float x;
		float y;
		float z;
		float w;
	};
	//----------------------四维矩阵--------------------------
	struct MATRIX4
	{
		MATRIX4()
		{
			Identity();
		}

		inline void	SetRow(UINT row, float newRow[4])
		{
			if (row < 4)
			{
				for (UINT i = 0;i < 4;i++)
					m[row][i] = newRow[i];
			}
		};

		inline void	SetRow(UINT row, float(&&newRow)[4])
		{
			if (row < 4)
			{
				for (UINT i = 0;i < 4;i++)
					m[row][i] = newRow[i];
			}
		};

		inline void	SetColumn(UINT col, float newCol[4])
		{
			if (col < 4)
			{
				for (UINT i = 0;i < 4;++i)
					m[i][col] = newCol[i];
			}
		};

		inline void	SetColumn(UINT col, float(&&newCol)[4])
		{
			if (col < 4)
			{
				for (UINT i = 0;i < 4;++i)
					m[i][col] = newCol[i];
			}
		};
		
		inline void SetMatrix(VEC4 (&&row)[4])
		{
			for (UINT i = 0;i < 4;++i)
			{
				m[i][0] = row[i].x;
				m[i][1] = row[i].y;
				m[i][2] = row[i].z;
				m[i][3] = row[i].w;
			}
		};

		void Identity()
		{
			SetRow(0, { 1,0,0,0 });
			SetRow(1, { 0,1,0,0 });
			SetRow(2, { 0,0,1,0 });
			SetRow(3, { 0,0,0,1 });
		};

		float m[4][4];
	};
	//----------------------包围盒--------------------------
	struct BOUNDINGBOX
	{
		BOUNDINGBOX()
		{
			max = { 1,1,1 };
			min = { 0,0,0 };
		}

		VEC3 max;
		VEC3 min;
	};

	//***************************************数学运算*******************************

	//----------------------------向量运算------------------------
	extern float Vec2_Dot(const VEC2& vec1, const VEC2& vec2);

	extern float Vec2_Length(const VEC2& vec);

	extern float VEC3_Dot(const VEC3& vec1, const VEC3& vec2);

	extern VEC3 VEC3_Cross(const VEC3& vec1, const VEC3& vec2);

	extern float VEC3_Length(const VEC3& vec);

	extern VEC3 VEC3_Reflect(const VEC3& vec, const VEC3& axis);

	extern float Vec4_Dot(const VEC4& vec1, const VEC4& vec2);

	extern float Vec4_Length(const VEC4& vec);
	

	//----------------------------矩阵运算------------------------
	extern MATRIX4 Matrix_Multiply(const MATRIX4& leftMat, const MATRIX4& rightMat);

	extern VEC4	Matrix_Multiply(const MATRIX4 leftMat, const VEC4& rightColumnVec);


	extern MATRIX4	Matrix_Translation(float dx, float dy, float dz);//平移

	extern MATRIX4	Matrix_RotationX(float angle);//旋转

	extern MATRIX4	Matrix_RotationY(float angle);

	extern MATRIX4	Matrix_RotationZ(float angle);

	extern MATRIX4	Matrix_YawPitchRoll(float yaw_Y, float pitch_X, float roll_Z);

	extern MATRIX4	Matrix_PerspectiveProjection(float ViewAngleY, float aspectRatio, float NearPlaneZ, float FarPlaneZ);//透视投影矩阵

	extern MATRIX4	Matrix_Transpose(const MATRIX4& inMatrix);//转置

	//----------------------------限定范围------------------------
	extern UINT Limited(UINT val, UINT min, UINT max);

	extern float Limited(float val, float min, float max);

	extern VEC2 Limited(const VEC2& val,const VEC2& min, const VEC2& max);

	extern VEC3 Limited(const VEC3& val, const VEC3& min, const VEC3& max);

	//----------------------------插值------------------------
	extern BYTE Lerp(BYTE start, BYTE end, float t);

	extern float Lerp(float start, float end, float t);

	extern VEC2 Lerp(const VEC2& start, const VEC2& end, float t);

	extern VEC3 Lerp(const VEC3& start, const VEC3& end, float t);


}
