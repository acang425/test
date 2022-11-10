#pragma once

	class IMesh : private IFileManager
	{
		friend  class IRenderer;

	public:
		//构造函数
		IMesh();

		~IMesh();


		BOOL	LoadFile_OBJ(std::string pFilePath);

		void		SetColor(const VEC4& color);

		void		SetMaterial(const Material& mat);

		void		SetTexture(IPicture* pTex);

		void		SetPosition(float x, float y, float z);

		void		SetPosition(const VEC3& pos);

		void		SetRotation(float angleX, float angleY, float angleZ);

		void		SetRotationX_Pitch(float angleX);

		void		SetRotationY_Yaw(float angleY);

		void		SetRotationZ_Roll(float angleZ);

		VEC3		GetPosition();

		void		RotateX_Pitch(float angle);

		void		RotateY_Yaw(float angle);

		void		RotateZ_Roll(float angle);

		float		GetRotationY_Yaw();

		float		GetRotationX_Pitch();

		float		GetRotationZ_Roll();


		void		GetWorldMatrix(MATRIX4& outMat);

		UINT		GetVertexCount();

		void		GetVertex(UINT iIndex, Vertex& outVertex);

		void		GetVertexBuffer(std::vector<Vertex>& outBuff);


	private:

		void	Destroy();

		void		mFunction_UpdateWorldMatrix();//更新世界矩阵
	private:


		VEC3							mPosition;
		BOUNDINGBOX				mBoundingBox;

		float									mRotationX_Pitch;
		float									mRotationY_Yaw;
		float									mRotationZ_Roll;


		MATRIX4						mMatrixWorld;
		std::vector<Vertex>*			m_pVB_Mem;//顶点缓存
		std::vector<UINT>*			m_pIB_Mem;//顶点序号缓存
		
		Material							mMaterial;
		//graphic attribute
		IPicture*							m_pTexture;

};