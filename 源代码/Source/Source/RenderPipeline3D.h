#pragma once



class IRenderPipeline3D
{
public:
	IRenderPipeline3D();

	BOOL	Init(RenderPipeline_InitData& initData);
	//-----------------------------------画三角形-----------------------------------------

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

	//-----------------------缓存区----------------------------------
	UINT mBufferWidth;
	UINT mBufferHeight;
	std::vector<float>*		m_pZBuffer;//depth buffer


	//----------------------渲染管线-----------------
	//----------------------矩阵-----------------
	MATRIX4			mWorldMatrix;
	MATRIX4			mViewMatrix;
	MATRIX4			mProjMatrix;

	//----------------------纹理坐标-----------------
	float						mTexCoord_offsetX;
	float						mTexCoord_offsetY;
	float						mTexCoord_scale;
	//----------------------光线-----------------
	static const UINT	c_maxLightCount = 8;
	BOOL					mLightEnabled;
	DirectionalLight	mDirLight[c_maxLightCount];
	//---------------------材质 贴图 相机-----------------
	Material				mMaterial;
	IPicture*				m_pTexture;
	VEC3				mCameraPos;

	//------------------渲染管线函数-----------------
	// 
	//------------------顶点着色-----------------

	void VertexShader(Vertex& inVertex);//顶点着色：转换为齐次坐标，赋值纹理坐标和光照颜色

	std::vector<VertexShaderOutput_Vertex>*		m_pVB_HomoSpace;//上述函数输出顶点的容器

	//------------------裁剪-----------------
	void	HomoSpaceClipping_Triangles(std::vector<UINT>* const pIB);//裁剪三角形

	std::vector<VertexShaderOutput_Vertex>*	 m_pVB_HomoSpace_Clipped;//上述函数输出的顶点

	std::vector<UINT>*										m_pIB_HomoSpace_Clipped;

	//------------------光栅化-----------------
	void RasterizeTriangles();//光栅化三角形以获取离散像素

	std::vector<RasterizedFragment>*		m_pVB_Rasterized;//光栅化之后的片元容器

	//----------------像素着色---------------------

	void	PixelShader_DrawTriangles(RasterizedFragment& inVertex);//计算每个光栅化“像素”的颜色

	std::vector<COLOR3>*		m_pOutColorBuffer;//输出颜色缓存

	//----------------------辅助函数-----------------
	BOOL		mFunction_HorizontalIntersect(float y,const VEC2& v1, const VEC2& v2, const VEC2& v3, UINT& outX1,UINT& outX2);//计算相交点

	void			mFunction_SetZ(UINT x, UINT y,float z);//设置深度

	float			mFunction_GetZ(UINT x, UINT y);

	BOOL		mFunction_DepthTest(UINT x,UINT y,float testZ);//深度测试

	VEC4	mFunction_VertexLighting(const VEC3& vPosW,const VEC3& vNormalW);

	VEC3	mFunction_SampleTexture(float x, float y);

};