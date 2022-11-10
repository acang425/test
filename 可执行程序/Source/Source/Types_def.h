#pragma once

using namespace Math;

typedef VEC3 COLOR3;

struct Material
{
	Material() 
	{
		ambient = { 0,0,0 };
		diffuse = { 1.0f,0,0 };
		specular = { 1.0f,1.0f,1.0f };
		specularSmoothLevel = 10;
	};
	VEC3	ambient;   //环境光照(Ambient Lighting)
	VEC3	diffuse;//漫反射光照(Diffuse Lighting)
	VEC3	specular;// 镜面光照(Specular Lighting)
	UINT			specularSmoothLevel;//反光度
};

struct Vertex
{
	VEC3 pos;//位置
	VEC4 color;//颜色
	VEC3 normal;//法线
	VEC2 texcoord;//纹理坐标
};

struct VertexShaderOutput_Vertex
{
	VEC4 posH;//齐次坐标
	VEC4 color;//颜色
	VEC2 texcoord;//纹理坐标
};

struct RasterizedFragment
{
	UINT pixelX;//像素X坐标
	UINT pixelY;//像素Y坐标
	VEC4 color;//颜色
	VEC2 texcoord;//纹理坐标
};

struct RenderPipeline_InitData
{
	UINT bufferWidth;//缓存区宽高
	UINT bufferHeight;

	std::vector<float>*		pZBuffer;//深度缓存
	std::vector<VEC3>*	pOutColorBuffer;//输出的颜色缓存
};

struct RenderPipeline_DrawCallData
{
	std::vector<Vertex>*		pVertexBuffer;//顶点缓存
	std::vector<UINT>*		pIndexBuffer;//顶点序号缓存
	UINT offset;//偏移量
	UINT VertexCount;//顶点数目
};

struct OBJ_vertexInfoIndex
{
	OBJ_vertexInfoIndex()
	{
		vertexID = texcoordID = vertexNormalID = 0;
	};

	OBJ_vertexInfoIndex(int vID, int texcID, int vnID)
	{
		vertexID = vID;
		texcoordID = texcID;
		vertexNormalID = vnID;
	}

	inline BOOL operator==(OBJ_vertexInfoIndex const& v)const
	{
		if (vertexID == v.vertexID && texcoordID == v.texcoordID && vertexNormalID == v.vertexNormalID)
		{
			return TRUE;
		}
		return FALSE;
	}

	UINT vertexID;//顶点序号
	UINT texcoordID;//纹理坐标序号
	UINT vertexNormalID;//顶点法线序号
};

struct Renderer_Color3ToConsolePixel
{
	COLOR3	color;//颜色
	BYTE			asciiChar;//输出的像素字符
	WORD		textAttr;//输出的像素颜色
};


//光照类型
struct DirectionalLight
{
public:
	DirectionalLight()
	{
		ZeroMemory(this, sizeof(*this));
		mSpecularIntensity = 1.0f;
		mDirection = VEC3(1.0f, 0, 0);
		mDiffuseIntensity = 0.5;
		mIsEnabled = TRUE;
	}

	BOOL operator==(DirectionalLight& Light)
	{
		if (memcmp(this, &Light, sizeof(Light)) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	VEC3 mAmbientColor;	 //环境光
	float		mSpecularIntensity;//镜面反射强度
	VEC3 mDiffuseColor;		//漫射光
	float			mDiffuseIntensity;//漫反射强度
	VEC3 mSpecularColor;	 //镜面光
	BOOL		mIsEnabled;//需不需要光照
	VEC3 mDirection;//方向
};