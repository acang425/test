#include "MyConsoleEngine.h"

IRenderPipeline3D::IRenderPipeline3D()
{
	m_pVB_HomoSpace = new std::vector<VertexShaderOutput_Vertex>;
	m_pVB_HomoSpace_Clipped = new std::vector<VertexShaderOutput_Vertex>;
	m_pIB_HomoSpace_Clipped = new std::vector<UINT>;
	m_pVB_Rasterized = new std::vector<RasterizedFragment>;
	m_pTexture = nullptr;
	mWorldMatrix.Identity();
	mViewMatrix.Identity();
	mProjMatrix.Identity();
	mTexCoord_offsetX=0.0f;
	mTexCoord_offsetY=0.0f;
	mTexCoord_scale=1.0f;
	mCameraPos = { 0,0,0 };
	mLightEnabled = TRUE;
	for (UINT i = 0;i < c_maxLightCount;++i)memset(&mDirLight[i],0,sizeof(DirectionalLight));
}

BOOL IRenderPipeline3D::Init(RenderPipeline_InitData & initData)
{
	mBufferWidth = initData.bufferWidth;
	mBufferHeight = initData.bufferHeight;
	if (initData.pOutColorBuffer != nullptr)
	{
		m_pOutColorBuffer = initData.pOutColorBuffer;
	}
	else
	{
		DEBUG_MSG1("Render PipeLine: color buffer ptr invalid!!");
		return FALSE;
	}


	if (initData.pZBuffer != nullptr)
	{
		m_pZBuffer = initData.pZBuffer;
	}
	else
	{
		DEBUG_MSG1("Render PipeLine: Z buffer ptr invalid!!");
		return FALSE;

	}
	return TRUE;
}

void IRenderPipeline3D::DrawTriangles(RenderPipeline_DrawCallData & drawCallData)
{
	//�������
	m_pVB_HomoSpace->clear();
	m_pVB_HomoSpace_Clipped->clear();
	m_pIB_HomoSpace_Clipped->clear();
	m_pVB_Rasterized->clear();

	//��ʼ��һ��
	UINT offset = drawCallData.offset;
	UINT vCount = drawCallData.VertexCount;
	auto const pVB = drawCallData.pVertexBuffer;
	auto const pIB = drawCallData.pIndexBuffer;

	//�����ռ�
	m_pVB_HomoSpace->reserve(vCount);
	m_pVB_Rasterized->reserve(mBufferWidth*mBufferHeight / 3);// /3 �Ǵ�Ź���

	//------------------------------������ɫ-----------------------------
	for (UINT i = offset;i < (offset+vCount>pVB->size()?pVB->size():offset+vCount);++i)
	{

		Vertex& currVertex = drawCallData.pVertexBuffer->at(offset + i);

		//ʹ�ö�����ɫ������ÿ������
		VertexShader(currVertex);
	}

	//--------------------��βü��ռ�-------------------
	HomoSpaceClipping_Triangles(pIB);

	//-----------------------------��դ��------------------------------
	RasterizeTriangles();

	//------------------------------������ɫ----------------------------
	for (auto& rasterizedVertex: *m_pVB_Rasterized)
	{
		PixelShader_DrawTriangles(rasterizedVertex);
	}

}


void IRenderPipeline3D::SetWorldMatrix(const MATRIX4 & mat)
{
	mWorldMatrix = mat;
}

void IRenderPipeline3D::SetViewMatrix(const MATRIX4 & mat)
{
	mViewMatrix = mat;
}

void IRenderPipeline3D::SetProjMatrix(const MATRIX4 & mat)
{
	mProjMatrix = mat;
}

void IRenderPipeline3D::SetCameraPos(const VEC3 & vec)
{
	mCameraPos = vec;
}

void IRenderPipeline3D::SetLightingEnabled(BOOL enabled)
{
	mLightEnabled = enabled;
}

void IRenderPipeline3D::SetLight(UINT index, const DirectionalLight & light)
{
	if (index < c_maxLightCount)
	{
		mDirLight[index] = light;
	}
}

void IRenderPipeline3D::SetTexcoordTransform(float dx, float dy, float scale)
{
	mTexCoord_offsetX = dx;
	mTexCoord_offsetY = dy;
	mTexCoord_scale = scale;
}

void IRenderPipeline3D::SetMaterial(const Material & mat)
{
	mMaterial = mat;
}

void IRenderPipeline3D::SetTexture(IPicture * pTex)
{
	m_pTexture = pTex;
}


void IRenderPipeline3D::VertexShader(Vertex& inVertex)
{
	VertexShaderOutput_Vertex outVertex;

	//----------------------ת��Ϊ�������-------------------
	//����λ��
	VEC4 pos(inVertex.pos.x, inVertex.pos.y, inVertex.pos.z, 1.0f);
	pos = Matrix_Multiply(mWorldMatrix, pos);
	pos = Matrix_Multiply(mViewMatrix, pos);

	//��Z����
	float Z_ViewSpace = pos.z;
	pos = Matrix_Multiply(mProjMatrix, pos);
	if (Z_ViewSpace >= 0)
	{
		pos.x /= (Z_ViewSpace);
		pos.y /= (Z_ViewSpace);
	}

	outVertex.posH = pos;

//���߱任�������ų������з�����������˿�����ת�þ�����������
	MATRIX4 WorldMat_NoTrans = mWorldMatrix;
	WorldMat_NoTrans.SetColumn(3, { 0,0,0,1.0f });
	VEC4 normW(inVertex.normal.x, inVertex.normal.y, inVertex.normal.z, 1.0f);
	normW = Matrix_Multiply(WorldMat_NoTrans, normW);


//----------------------------��������----------------------------------
	outVertex.texcoord = VEC2(
		inVertex.texcoord.x*mTexCoord_scale + mTexCoord_offsetX, 
		inVertex.texcoord.y*mTexCoord_scale + mTexCoord_offsetY);


//----------------------------����or�޹���----------------------------------
	if (mLightEnabled)
	{
		outVertex.color = mFunction_VertexLighting(inVertex.pos, VEC3(normW.x,normW.y,normW.z));
	}
	else
	{
		outVertex.color = inVertex.color;
	}

	m_pVB_HomoSpace->push_back(outVertex);
}

//�ü�
void IRenderPipeline3D::HomoSpaceClipping_Triangles(std::vector<UINT>* const pIB)
{
	//�����㻺��or��Ż��渴�Ƶ��µ������Ȼ��ɾ��
	*m_pIB_HomoSpace_Clipped = (*pIB);
	*m_pVB_HomoSpace_Clipped = std::move(*m_pVB_HomoSpace);

	UINT i = 0;
	while(i < m_pIB_HomoSpace_Clipped->size()-3)
	{
		//����Ż������ҵ�����
		UINT idx1 = m_pIB_HomoSpace_Clipped->at(i);
		UINT idx2 = m_pIB_HomoSpace_Clipped->at(i + 1);
		UINT idx3 = m_pIB_HomoSpace_Clipped->at(i + 2);

		auto const v1 = m_pVB_HomoSpace_Clipped->at(idx1);
		auto const v2 = m_pVB_HomoSpace_Clipped->at(idx2);
		auto const v3 = m_pVB_HomoSpace_Clipped->at(idx3);

		//�ж϶����Ƿ�����βü��ռ���
		bool b1 = v1.posH.x <= -1.0f || v1.posH.x >= 1.0f ||
			v1.posH.y <= -1.0f || v1.posH.y >= 1.0f ||
			v1.posH.z <= 0.0f || v1.posH.z >= 1.0f;

		bool b2 = v2.posH.x <= -1.0f || v2.posH.x >= 1.0f ||
			v2.posH.y <= -1.0f || v2.posH.y >= 1.0f ||
			v2.posH.z <= 0.0f || v2.posH.z >= 1.0f;

		bool b3 = v3.posH.x <= -1.0f || v3.posH.x >= 1.0f ||
			v3.posH.y <= -1.0f || v3.posH.y >= 1.0f ||
			v3.posH.z <= 0.0f || v3.posH.z >= 1.0f;

		//����������ȫ���ڷ�Χ����������θ��޳���
		if (b1 && b2 && b3==true)
		{
			//��back�Ķ��㽻��Ȼ��pop��
			UINT rubbishFragmentStartIndex = m_pIB_HomoSpace_Clipped->size() - 3;
			std::swap(m_pIB_HomoSpace_Clipped->at(i), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex));
			std::swap(m_pIB_HomoSpace_Clipped->at(i+1), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex+1));
			std::swap(m_pIB_HomoSpace_Clipped->at(i+2), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex+2));
		
			for (UINT j = 0;j < 3;j++)m_pIB_HomoSpace_Clipped->pop_back();
		}
		else
		{
			//ָ����ǰ�ߣ����������pop����Ҫ����Ϊ����ᡰ�Զ����䡱��
			i += 3;
		}
	}
}

void IRenderPipeline3D::RasterizeTriangles()
{
	for (UINT tri = 0;tri < m_pIB_HomoSpace_Clipped->size() - 2;tri += 3)
	{
		UINT idx1 = m_pIB_HomoSpace_Clipped->at(tri);
		UINT idx2 = m_pIB_HomoSpace_Clipped->at(tri + 1);
		UINT idx3 = m_pIB_HomoSpace_Clipped->at(tri + 2);

		RasterizedFragment outVertex;

		//����Ż������ҵ�����
		const auto& v1 = m_pVB_HomoSpace_Clipped->at(idx1);
		const auto& v2 = m_pVB_HomoSpace_Clipped->at(idx2);
		const auto& v3 = m_pVB_HomoSpace_Clipped->at(idx3);


		//ת�����������ꡪ��>[0,1]
		auto convertToPixelSpace = [&](const VertexShaderOutput_Vertex& v, VEC2& outV)
		{
			outV.x = float(mBufferWidth) * (v.posH.x + 1.0f) / 2.0f;
			outV.y = float(mBufferHeight) * (-v.posH.y + 1.0f) / 2.0f;
		};

		VEC2 v1_pixel, v2_pixel, v3_pixel;
		convertToPixelSpace(v1, v1_pixel);
		convertToPixelSpace(v2, v2_pixel);
		convertToPixelSpace(v3, v3_pixel);

		//��������˫���Բ�ֵ�Ļ�����
		VEC2 basisVector1 = v2_pixel - v1_pixel;
		VEC2 basisVEC2 = v3_pixel - v1_pixel;

		float D = basisVector1.x*basisVEC2.y - basisVEC2.x*basisVector1.y;

		//�����ε������߲�����ͬһ������
		if (D == 0)return;

		//�ҵ���������������͵�
		float minY = Limited(min(min(v1_pixel.y, v2_pixel.y), v3_pixel.y), 0, float(mBufferHeight - 1));
		float maxY = Limited(max(max(v1_pixel.y, v2_pixel.y), v3_pixel.y), 0, float(mBufferHeight - 1));

		//
		for (int j = int(minY);j < int(maxY) + 1;++j)
		{
			BOOL intersectSucceeded = FALSE;
			UINT x1 = 0, x2 = 0;
			intersectSucceeded =
				mFunction_HorizontalIntersect(float(j), v1_pixel, v2_pixel, v3_pixel, x1, x2);//�Ը���������ˮƽ�ߵ����ཻ��

			if (intersectSucceeded == TRUE)
			{
				//----------------����ÿ��ƬԪ----------------
				for (UINT i = x1;i <= x2;++i)
				{
					if (i >= mBufferWidth || j >= mBufferHeight)
					{
						break;
					//Խ����break
					}
					//���㵱ǰ��������ص���������
					VEC2 currentPoint_pixel = VEC2(float(i) + 0.5f, float(j) + 0.5f);

					//����õ������v1������
					VEC2 currentPointLocal_pixel = currentPoint_pixel - v1_pixel;

					//����˫���Բ�ֵ����ϵ��(s,t)
					float s = (currentPointLocal_pixel.x*basisVEC2.y - currentPointLocal_pixel.y*basisVEC2.x) / D;
					float t = (basisVector1.x*currentPointLocal_pixel.y - basisVector1.y*currentPointLocal_pixel.x) / D;

					//�������
					float depth = 1.0f / (s / v2.posH.z + t / v3.posH.z + (1 - s - t) / v1.posH.z);
					if (mFunction_DepthTest(i, j, depth) == FALSE)goto label_nextPixel;//��Ȳ��ԣ����ڸ���ÿ�������ǳ���
					//---------------------�����Ҫ�������-----------------------
					//����z
					mFunction_SetZ(i, j, depth);

					outVertex.pixelX = i;
					outVertex.pixelY = j;

					//͸�ӽ�����ֵ
					outVertex.color =
						(s / v2.posH.z* v2.color +
							t / v3.posH.z *v3.color +
							(1 - s - t) / v1.posH.z *v1.color)*depth;

					outVertex.texcoord =
						(s / v2.posH.z * v2.texcoord +
							t / v3.posH.z * v3.texcoord +
							(1 - s - t) / v1.posH.z * v1.texcoord)*depth;


					m_pVB_Rasterized->push_back(outVertex);
					//----------------------------------------------------------------
				label_nextPixel:;
				}//����ÿ��x
			}//������ཻ��

		}//����ÿ��y
	}//����ÿ����βü��ռ��е�������ƬԪ
}

void IRenderPipeline3D::PixelShader_DrawTriangles(RasterizedFragment& inVertex)
{
	COLOR3 outColor;
	//�����ͼ����ֵ
	COLOR3 texSampleColor = mFunction_SampleTexture(inVertex.texcoord.x, inVertex.texcoord.y);
	//��ɫ
	outColor =COLOR3(inVertex.color.x, inVertex.color.y, inVertex.color.z) * texSampleColor;
	//������ɫ����
	m_pOutColorBuffer->at(inVertex.pixelY*mBufferWidth + inVertex.pixelX) = outColor;
}




BOOL IRenderPipeline3D::mFunction_HorizontalIntersect(float y, const VEC2 & v1, const VEC2 & v2, const VEC2 & v3, UINT & outX1, UINT & outX2)
{
	const VEC2 triangleVertices[3] = { v1,v2,v3 };

	//�ж�ˮƽ�����������ཻ�˼�������
	std::vector<UINT> indexList_sameYCoord;
	if (v1.y == y) indexList_sameYCoord.push_back(0); 
	if (v2.y == y) indexList_sameYCoord.push_back(1); 
	if (v3.y == y) indexList_sameYCoord.push_back(2); 


	//�ж������Ƿ��ཻ
	const auto func_intersect = [](float y, const VEC2& v1, const VEC2& v2, VEC2& outVec)->BOOL
	{
		bool b1 = v1.y > y && v2.y < y;
		bool b2 = v1.y < y && v2.y > y;
		//����ཻ��
		if (b1||b2)
		{
			outVec = Lerp(v1, v2, (y - v1.y) / (v2.y - v1.y));
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	};


	//����ˮƽ�����������ཻ�˼������㣬��������ཻ��x1,x2
	switch (indexList_sameYCoord.size())
	{
		//------------0 :û�н�������-----------------
		case 0:
		{
	
			BOOL canIntersect = FALSE;
			std::vector<VEC2> intersectPointList;

			VEC2 tmpVec;
			canIntersect = func_intersect(y, v1, v2, tmpVec);
			if (canIntersect)intersectPointList.push_back(tmpVec);

			canIntersect = func_intersect(y, v2, v3, tmpVec);
			if (canIntersect)intersectPointList.push_back(tmpVec);

			canIntersect = func_intersect(y, v1, v3, tmpVec);
			if (canIntersect)intersectPointList.push_back(tmpVec);
			//output vertex
			if (intersectPointList.size() == 2) 
			{
				outX1 = UINT(Limited(intersectPointList.at(0).x,0,float(mBufferWidth)));
				outX2 = UINT(Limited(intersectPointList.at(1).x,0,float(mBufferWidth)));
				if (outX1 > outX2)std::swap(outX1, outX2);
				return TRUE;
			}
			else
			{
				return FALSE;
			}

			break;
		}


		//------------1 :��һ�������� Y=y��-----------------
		case 1:
		{
			VEC2 tmpVec;
			BOOL canOppositeLineSegmentIntersect = FALSE;
			//�ж��������ε��ĸ������y�ཻ
			switch (indexList_sameYCoord.at(0))
			{
			case 0:
				canOppositeLineSegmentIntersect = func_intersect(y, v2, v3, tmpVec);
				break;
			case 1:
				canOppositeLineSegmentIntersect = func_intersect(y, v1, v3, tmpVec);
				break;
			case 2:
				canOppositeLineSegmentIntersect = func_intersect(y, v1, v2, tmpVec);
				break;
			}

			if (canOppositeLineSegmentIntersect==TRUE)
			{
				outX1 = UINT(Limited(tmpVec.x,0,float(mBufferWidth)));
				outX2 = UINT(Limited(triangleVertices[indexList_sameYCoord.at(0)].x,0,float(mBufferWidth)));
				if (outX1 > outX2)std::swap(outX1, outX2);
				return TRUE;
			}
			else
			{
				return FALSE;
			}

			break;
		}

		//------------2 :������������ Y=y��-----------------

		case 2:
		{//ֱ�������������
			outX1 = UINT(Limited(triangleVertices[indexList_sameYCoord.at(0)].x,0,float(mBufferWidth)));//new intersect point
			outX2 = UINT(Limited(triangleVertices[indexList_sameYCoord.at(1)].x,0,float(mBufferWidth)));//the one on the tested-y line
			if (outX1 > outX2)std::swap(outX1, outX2);
			return TRUE;
			break;
		}

		default:
			return FALSE;
	}

}

void IRenderPipeline3D::mFunction_SetZ(UINT x, UINT y, float z)
{
	m_pZBuffer->at(y*mBufferWidth + x)=z;
}

inline float IRenderPipeline3D::mFunction_GetZ(UINT x, UINT y)
{
	return m_pZBuffer->at(y*mBufferWidth + x);
}

inline BOOL IRenderPipeline3D::mFunction_DepthTest(UINT x, UINT y, float testZ)
{
	float& targetZ = m_pZBuffer->at(y*mBufferWidth + x);
	if (testZ <= targetZ&&testZ>0.0f)
	{
		targetZ = testZ;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VEC4 IRenderPipeline3D::mFunction_VertexLighting(const VEC3& vPosW, const VEC3& vNormalW)
{
	//---------����ÿ�����㣬ִ�� Gouraud ��ɫ------------

	VEC4 outColor = { 0.0f,0.0f,0.0f,1.0f };

	//����ÿ������
	for (UINT i = 0;i < c_maxLightCount;++i)
	{
		if (mDirLight[i].mIsEnabled == TRUE)
		{
			//��λ��ÿ����ķ���
			VEC3 unitIncomingLightVec = mDirLight[i].mDirection;
			unitIncomingLightVec.GuiYiHua();

			//�ӵ�ǰ���㵽�����ʸ�������ڼ��㾵�淴��
			VEC3 toEye = mCameraPos - vPosW;
			toEye.GuiYiHua();

			//��λ�����㷨��
			VEC3 unitNormal = vNormalW;
			unitNormal.GuiYiHua();

			//������
			VEC3 currentAmbient = mMaterial.ambient* mDirLight[i].mAmbientColor * mMaterial.diffuse;

			//���������ӣ�ȷ���������ļн�С��PI/ 2
			VEC3 currentDiffuse = { 0,0,0 };
			VEC3 currentSpecular = { 0,0,0 };

			float diffuseFactor = mDirLight[i].mDiffuseIntensity*Math::VEC3_Dot((-1)*unitIncomingLightVec, unitNormal);
			if (diffuseFactor > 0.0f)
			{
				//��������ɫ
				currentDiffuse = diffuseFactor * mDirLight[i].mDiffuseColor;

				//���û��������ͼ����ʹ�ò��ʵ���������ɫ
				if (m_pTexture == nullptr)
				{
					currentDiffuse = currentDiffuse* mMaterial.diffuse;
				}
			}
			//���㶥����մ�������ɫ
			VEC3 outColor3 = currentAmbient+currentDiffuse+currentSpecular;
			outColor += VEC4(outColor3.x, outColor3.y, outColor3.z, 0.0f);
		}
	}

	return outColor;
}

inline VEC3 IRenderPipeline3D::mFunction_SampleTexture(float x, float y)
{
	//û��������ͼ�����Ӳ����л�ȡ��������ɫ
	if (m_pTexture == nullptr)return VEC3(1.0f, 1.0f, 1.0f);

	//�������
	UINT width = m_pTexture->GetWidth();
	UINT height = m_pTexture->GetHeight();
	float pixelX= abs(width * (float(x - UINT(x))));
	float pixelY = abs(height * (float(y - UINT(y))));
	return m_pTexture->GetPixel(UINT(pixelX), UINT(pixelY));
}
