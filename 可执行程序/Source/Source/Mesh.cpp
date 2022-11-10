#include "MyConsoleEngine.h"

using namespace Math;

static UINT c_VBstride_Default = sizeof(Vertex);		//VertexBuffer��ÿ��Ԫ�ص��ֽڿ��
static UINT c_VBoffset = 0;				//VertexBuffer�������ƫ�� ��Ϊ��ͷ��ʼ����offset��0

IMesh::IMesh()
{
	mRotationX_Pitch = 0.0f;
	mRotationY_Yaw = 0.0f;
	mRotationZ_Roll = 0.0f;//YPR��

	m_pVB_Mem		= new std::vector<Vertex>;
	m_pIB_Mem			= new std::vector<UINT>;//��������

	mMatrixWorld.Identity();//��ʼ���������

	Material defaultMat;//��ʼ�����ʲ���
	defaultMat.ambient = { 0.2f,0.2f,0.2f };
	defaultMat.diffuse	= {0.8f,0.8f,0.8f };
	defaultMat.specular = { 1.0f,1.0f,1.0f };
	defaultMat.specularSmoothLevel = 20;
	IMesh::SetMaterial(defaultMat);

	m_pTexture = nullptr;//��ʼ����ͼ
}

IMesh::~IMesh()
{
};

void IMesh::Destroy()
{
	delete m_pVB_Mem;
	delete m_pIB_Mem;
};


BOOL IMesh::LoadFile_OBJ(std::string pFilePath)
{
	
	m_pVB_Mem->clear();
	m_pIB_Mem->clear();


	std::vector<Vertex> tmpCompleteVertexList;
	std::vector<UINT> tmpNormalList;

	//��OBJ�еĶ����붥����š����ˡ���mesh��
	BOOL fileLoadSucceeded = FALSE;
	fileLoadSucceeded = IFileManager::ImportOBJ(pFilePath, *m_pVB_Mem, *m_pIB_Mem);
	if (!fileLoadSucceeded)
	{
		DEBUG_MSG1("Noise Mesh : Load OBJ failed!");
		return FALSE;
	}

	return TRUE;
}

void IMesh::SetColor(const VEC4& color)
{
	for (auto& v : *m_pVB_Mem)
	{
		v.color = color;
	}
}

void IMesh::SetMaterial(const Material & mat)
{
	mMaterial = mat;
}

void IMesh::SetTexture(IPicture * pTex)
{
	m_pTexture = pTex;
}

void IMesh::SetPosition(float x,float y,float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void IMesh::SetPosition(const VEC3 & pos)
{
	mPosition = pos;
}

void IMesh::SetRotation(float angleX, float angleY, float angleZ)
{
	mRotationX_Pitch	= angleX;
	mRotationY_Yaw		= angleY;
	mRotationZ_Roll		= angleZ;
}

void IMesh::SetRotationX_Pitch(float angleX)
{
	mRotationX_Pitch = angleX;
};

void IMesh::SetRotationY_Yaw(float angleY)
{
	mRotationY_Yaw = angleY;
};

void IMesh::SetRotationZ_Roll(float angleZ)
{
	mRotationZ_Roll = angleZ;
}

VEC3 IMesh::GetPosition()
{
	return mPosition;
}

void IMesh::RotateX_Pitch(float angle)
{
	SetRotationX_Pitch(mRotationX_Pitch + angle);
}

void IMesh::RotateY_Yaw(float angle)
{
	SetRotationY_Yaw(mRotationY_Yaw + angle);
}

void IMesh::RotateZ_Roll(float angle)
{
	SetRotationZ_Roll(mRotationZ_Roll + angle);
}

float IMesh::GetRotationY_Yaw()
{
	return mRotationY_Yaw;
}

float IMesh::GetRotationX_Pitch()
{
	return mRotationX_Pitch;
}

float IMesh::GetRotationZ_Roll()
{
	return mRotationZ_Roll;
;
}

void IMesh::GetWorldMatrix(MATRIX4 & outMat) 
{
	mFunction_UpdateWorldMatrix();
	outMat = mMatrixWorld;
}

UINT IMesh::GetVertexCount()
{
	return m_pVB_Mem->size();
}

void IMesh::GetVertex(UINT iIndex, Vertex& outVertex)
{
	if (iIndex < m_pVB_Mem->size())
	{
		outVertex = m_pVB_Mem->at(iIndex);
	}
}

void IMesh::GetVertexBuffer(std::vector<Vertex>& outBuff)
{
	std::vector<Vertex>::iterator iterBegin, iterLast;
	iterBegin = m_pVB_Mem->begin();
	iterLast = m_pVB_Mem->end();
	outBuff.assign(iterBegin,iterLast);
}


void	IMesh::mFunction_UpdateWorldMatrix()
{
	MATRIX4	tmpMatrixScaling;
	MATRIX4	tmpMatrixTranslation;
	MATRIX4	tmpMatrixRotation;
	MATRIX4	tmpMatrix;

	//��ʼ���������
	tmpMatrix.Identity();
		
	//��ת����
	tmpMatrix=Matrix_YawPitchRoll(mRotationY_Yaw, mRotationX_Pitch, mRotationZ_Roll);
	
	//�޸�ƽ����
	tmpMatrix.m[0][3] = mPosition.x;
	tmpMatrix.m[1][3] = mPosition.y;
	tmpMatrix.m[2][3] = mPosition.z;

	mMatrixWorld = tmpMatrix;
}

