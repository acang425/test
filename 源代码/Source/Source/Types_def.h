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
	VEC3	ambient;   //��������(Ambient Lighting)
	VEC3	diffuse;//���������(Diffuse Lighting)
	VEC3	specular;// �������(Specular Lighting)
	UINT			specularSmoothLevel;//�����
};

struct Vertex
{
	VEC3 pos;//λ��
	VEC4 color;//��ɫ
	VEC3 normal;//����
	VEC2 texcoord;//��������
};

struct VertexShaderOutput_Vertex
{
	VEC4 posH;//�������
	VEC4 color;//��ɫ
	VEC2 texcoord;//��������
};

struct RasterizedFragment
{
	UINT pixelX;//����X����
	UINT pixelY;//����Y����
	VEC4 color;//��ɫ
	VEC2 texcoord;//��������
};

struct RenderPipeline_InitData
{
	UINT bufferWidth;//���������
	UINT bufferHeight;

	std::vector<float>*		pZBuffer;//��Ȼ���
	std::vector<VEC3>*	pOutColorBuffer;//�������ɫ����
};

struct RenderPipeline_DrawCallData
{
	std::vector<Vertex>*		pVertexBuffer;//���㻺��
	std::vector<UINT>*		pIndexBuffer;//������Ż���
	UINT offset;//ƫ����
	UINT VertexCount;//������Ŀ
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

	UINT vertexID;//�������
	UINT texcoordID;//�����������
	UINT vertexNormalID;//���㷨�����
};

struct Renderer_Color3ToConsolePixel
{
	COLOR3	color;//��ɫ
	BYTE			asciiChar;//����������ַ�
	WORD		textAttr;//�����������ɫ
};


//��������
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

	VEC3 mAmbientColor;	 //������
	float		mSpecularIntensity;//���淴��ǿ��
	VEC3 mDiffuseColor;		//�����
	float			mDiffuseIntensity;//������ǿ��
	VEC3 mSpecularColor;	 //�����
	BOOL		mIsEnabled;//�費��Ҫ����
	VEC3 mDirection;//����
};