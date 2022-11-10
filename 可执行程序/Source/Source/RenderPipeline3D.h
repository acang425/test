#pragma once



class IRenderPipeline3D
{
public:
	IRenderPipeline3D();

	BOOL	Init(RenderPipeline_InitData& initData);
	//-----------------------------------��������-----------------------------------------

	void		DrawTriangles(RenderPipeline_DrawCallData& drawCallData);

	//-----------------------------------SET-----------------------------------------
	//-----------------------MATRIX-------------------------
	void		SetWorldMatrix(const MATRIX4& mat);

	void		SetViewMatrix(const MATRIX4& mat);

	void		SetProjMatrix(const MATRIX4& mat);
	//---------------------SCENE-----------------------------------

	void		SetCameraPos(const VEC3& vec);

	void		SetLightingEnabled(BOOL enabled=TRUE);

	void		SetLight(UINT index,const DirectionalLight& light);

	void		SetTexcoordTransform(float dx, float dy, float scale);

	void		SetMaterial(const Material& mat);

	void		SetTexture(IPicture* pTex);
	//------------------------------------------------------------
private:

	//-----------------------������----------------------------------
	UINT mBufferWidth;
	UINT mBufferHeight;
	std::vector<float>*		m_pZBuffer;//depth buffer


	//----------------------��Ⱦ����-----------------
	//----------------------����-----------------
	MATRIX4			mWorldMatrix;
	MATRIX4			mViewMatrix;
	MATRIX4			mProjMatrix;

	//----------------------��������-----------------
	float						mTexCoord_offsetX;
	float						mTexCoord_offsetY;
	float						mTexCoord_scale;
	//----------------------����-----------------
	static const UINT	c_maxLightCount = 8;
	BOOL					mLightEnabled;
	DirectionalLight	mDirLight[c_maxLightCount];
	//---------------------���� ��ͼ ���-----------------
	Material				mMaterial;
	IPicture*				m_pTexture;
	VEC3				mCameraPos;

	//------------------��Ⱦ���ߺ���-----------------
	// 
	//------------------������ɫ-----------------

	void VertexShader(Vertex& inVertex);//������ɫ��ת��Ϊ������꣬��ֵ��������͹�����ɫ

	std::vector<VertexShaderOutput_Vertex>*		m_pVB_HomoSpace;//��������������������

	//------------------�ü�-----------------
	void	HomoSpaceClipping_Triangles(std::vector<UINT>* const pIB);//�ü�������

	std::vector<VertexShaderOutput_Vertex>*	 m_pVB_HomoSpace_Clipped;//������������Ķ���

	std::vector<UINT>*										m_pIB_HomoSpace_Clipped;

	//------------------��դ��-----------------
	void RasterizeTriangles();//��դ���������Ի�ȡ��ɢ����

	std::vector<RasterizedFragment>*		m_pVB_Rasterized;//��դ��֮���ƬԪ����

	//----------------������ɫ---------------------

	void	PixelShader_DrawTriangles(RasterizedFragment& inVertex);//����ÿ����դ�������ء�����ɫ

	std::vector<COLOR3>*		m_pOutColorBuffer;//�����ɫ����

	//----------------------��������-----------------
	BOOL		mFunction_HorizontalIntersect(float y,const VEC2& v1, const VEC2& v2, const VEC2& v3, UINT& outX1,UINT& outX2);//�����ཻ��

	void			mFunction_SetZ(UINT x, UINT y,float z);//�������

	float			mFunction_GetZ(UINT x, UINT y);

	BOOL		mFunction_DepthTest(UINT x,UINT y,float testZ);//��Ȳ���

	VEC4	mFunction_VertexLighting(const VEC3& vPosW,const VEC3& vNormalW);

	VEC3	mFunction_SampleTexture(float x, float y);

};