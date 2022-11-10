
#include "MyConsoleEngine.h"

IRenderer::IRenderer()
{
	for (UINT i = 0;i < 8;i++)m_pPalette[i] = new std::vector<Renderer_Color3ToConsolePixel>;
	mFunction_GeneratePalette();
	m_pConsoleWindowTitle = new std::string;
}

IRenderer::~IRenderer()
{
//	CloseHandle(m_hScreenBuffer);
//	CloseHandle(m_hScreenOutput);
//	delete m_pCharBuffer;
//	delete m_pZBuffer;
//	for (UINT i = 0;i < 8;i++)
//	{
//		m_pPalette[i]->clear();
//		delete m_pPalette[i];
//	}
//
}

void IRenderer::Init(UINT bufferWidth, UINT bufferHeight)
{
	//�õ���׼������
	m_hScreenOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);

	//-----------------��������-----------------
	//�������������ҿ�������
	CONSOLE_FONT_INFOEX fontInfo = { 0 };//����̨�������Ϣ
	::GetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);//��ȡָ������̨�����������ĳߴ�
	fontInfo.cbSize = sizeof(fontInfo);//�˽ṹ�Ĵ�С�����ֽ�Ϊ��λ),���뽫�˳�Ա����Ϊsizeof(CONSOLE_FONT_INFOEX)
	fontInfo.dwFontSize = { c_ConsoleCharSizeX, c_ConsoleCharSizeY };//����������ÿ���ַ��Ŀ�Ⱥ͸߶�
	fontInfo.FontWeight = 700;		//�����ϸ�� Ȩ�صķ�ΧΪ100��1000����100Ϊ������ ���磬����Ȩ��Ϊ400����700Ϊ���塣
	::SetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);//����������Ϣ


	//��������̨��Ļ������
	m_hScreenBuffer = ::CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
		);

	//���ع��
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;//�α�Ŀɼ��ԡ� ������ɼ�����˳�ԱΪ TRUE��
	cci.dwSize = 1;//�ɹ�������ַ���Ԫ��İٷֱȡ� ��ֵ����1��100֮�䡣 �����ۻ�仯����Χ����ȫ��䵥Ԫ�񵽵�Ԫ�ײ���ˮƽ������
	::SetConsoleCursorInfo(m_hScreenOutput, &cci);//����ָ������̨��Ļ�������Ĺ��Ĵ�С�Ϳɼ���
	::SetConsoleCursorInfo(m_hScreenBuffer, &cci);


	//----------�������ĳ�ʼ��-----------------
	BOOL isSuceeded = FALSE;

	COORD maxWindowSize = ::GetLargestConsoleWindowSize(m_hScreenOutput);//����̨������ (X ��Ա) �� (Y ��Ա) �е��ַ���Ԫ������
	mBufferWidth = SHORT(bufferWidth) > maxWindowSize.X ? maxWindowSize.X : bufferWidth;
	mBufferHeight = SHORT(bufferHeight) > maxWindowSize.Y ? maxWindowSize.Y : bufferHeight;


	COORD dwBuffSize;//����̨��Ļ���������ַ���Ԫ������ꡣ
	dwBuffSize.X = SHORT(bufferWidth);
	dwBuffSize.Y = SHORT(bufferHeight);
	isSuceeded = ::SetConsoleScreenBufferSize(m_hScreenBuffer, dwBuffSize);//����ָ���Ŀ���̨��Ļ�������Ĵ�С��


	//��ʼ�������Ϣ����
	m_pCharBuffer = new std::vector<char>(mBufferHeight*mBufferWidth);
	m_pZBuffer = new std::vector<float>(mBufferHeight*mBufferWidth);
	m_pColorBuffer = new std::vector<COLOR3>(mBufferHeight*mBufferWidth);
	m_pTextAttrBuffer = new std::vector<WORD>(mBufferHeight*mBufferWidth);

	//��ָ������Ļ����������Ϊ��ǰ��ʾ�Ŀ���̨��Ļ��������
	::SetConsoleActiveScreenBuffer(m_hScreenBuffer);

	//----------------------------��ʼ����Ⱦ����-----------------------
	RenderPipeline_InitData initData;
	initData.bufferWidth = mBufferWidth;
	initData.bufferHeight = mBufferHeight;
	initData.pOutColorBuffer = m_pColorBuffer;
	initData.pZBuffer = m_pZBuffer;
	IRenderPipeline3D::Init(initData);

}

void IRenderer::Clear(COLOR3 clearColor,BOOL clearZBuff)
{
	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		//����ɫ������Ϊͬһ��ɫ
		m_pColorBuffer->at(i) = clearColor;
	}

	//ͳһ��Ȼ���
	if (clearZBuff == TRUE)
	{
		for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
		{
			m_pZBuffer->at(i) = 1.0f;
		}
	}
}

void IRenderer::SetCamera(ICamera & cam)
{
	m_pCamera = &cam;
}

void IRenderer::SetLight(UINT index, const DirectionalLight & light)
{
	IRenderPipeline3D::SetLight(index, light);
}

BOOL IRenderer::DrawPicture(IPicture & pic, UINT x1, UINT y1, UINT x2, UINT y2)
{
	//�޶����귶Χ
	x1 = Limited(x1, 0, mBufferWidth-1);
	x2 = Limited(x2, 0, mBufferWidth-1);
	y1 = Limited(y1, 0, mBufferHeight-1);
	y2 = Limited(y2, 0, mBufferHeight-1);


	if (x1 >= x2 || y1 >= y2)
	{
		DEBUG_MSG1("Render Picture: region info error!");
		return FALSE;
	}

	UINT drawRegionWidth = x2 - x1+1;
	UINT drawRegionHeight = y2 - y1+1;

	//����x��y������ֱ�ߵı�����ӳ�䵽��ͼ
	for (UINT i = x1;i <= x2;++i)
	{
		float x_ratio = float(i - x1) / drawRegionWidth;
		for (UINT j = y1;j <= y2;++j)
		{
			float y_ratio = float(j - y1) / drawRegionHeight;
			UINT picCoordX = UINT(pic.mWidth*x_ratio);
			UINT picCoordY = UINT(pic.mHeight*y_ratio);
			//���øõ�����ֵ
			mFunction_SetPixel(i, j, pic.GetPixel(picCoordX, picCoordY));
		}
	}
	return TRUE;
}


void IRenderer::DrawTriangle(COLOR3 color, const VEC2 & v1_pixel, const VEC2 & v2_pixel, const VEC2 & v3_pixel)
{
	//��ʼ����������Ϊ��λ����
	MATRIX4 matIdentity;
	matIdentity.Identity();
	IRenderPipeline3D::SetWorldMatrix(matIdentity);
	IRenderPipeline3D::SetProjMatrix(matIdentity);
	IRenderPipeline3D::SetViewMatrix(matIdentity);

	//ת�����������
	auto convertToHomoSpace = [&](const VEC2& v, VEC2& outV)
	{
		outV.x = (v.x / float(mBufferWidth) *2.0f) - 1.0f;
		outV.y = (-v.y / float(mBufferHeight) *2.0f) + 1.0f;
	};
	VEC2 v1, v2, v3;
	convertToHomoSpace(v1_pixel, v1);
	convertToHomoSpace(v2_pixel, v2);
	convertToHomoSpace(v3_pixel, v3);

	//��Ⱦ���߻Ὣ��βü��ռ�ת�������ؿռ�
	//��ʼ����������
	std::vector<Vertex> vertexArray(3);
	vertexArray[0].color = VEC4(color.x, color.y, color.z, 1.0f);
	vertexArray[0].pos = VEC3(v1.x, v1.y, 0.0f);
	vertexArray[0].normal = VEC3(0.0f, 1.0f, 0.0f);
	vertexArray[0].texcoord = VEC2(0.0f, 1.0f);

	vertexArray[1].color = VEC4(color.x, color.y, color.z, 1.0f);
	vertexArray[1].pos = VEC3(v2.x, v2.y, 0.0f);
	vertexArray[1].normal = VEC3(0.0f, 1.0f, 0.0f);
	vertexArray[1].texcoord = VEC2(1.0f, 0.0f);

	vertexArray[2].color = VEC4(color.x, color.y, color.z, 1.0f);
	vertexArray[2].pos = VEC3(v3.x, v3.y, 0.0f);
	vertexArray[2].normal = VEC3(0.0f, 1.0f, 0.0f);
	vertexArray[2].texcoord = VEC2(1.0f, 1.0f);

	//��ʼ���������
	std::vector<UINT> indexArray = { 0,1,2 };

	//��ʼ����Ⱦ����
	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = &indexArray;
	drawCallData.pVertexBuffer = &vertexArray;
	drawCallData.VertexCount = 3;

	//��Ⱦ���߿�����
	IRenderPipeline3D::DrawTriangles(drawCallData);

}



void IRenderer::RenderMesh(IMesh& mesh)
{
	if (m_pCamera == nullptr)return;

	//�������������ͼ����͸��ͶӰ�任����
	MATRIX4 matW, matV, matP;
	mesh.GetWorldMatrix(matW);
	m_pCamera->GetViewMatrix(matV);
	m_pCamera->GetProjMatrix(matP);

	IRenderPipeline3D::SetWorldMatrix(matW);
	IRenderPipeline3D::SetProjMatrix(matP);
	IRenderPipeline3D::SetViewMatrix(matV);

	//������������ʣ���ͼ������
	IRenderPipeline3D::SetCameraPos(m_pCamera->GetPosition());
	IRenderPipeline3D::SetMaterial(mesh.mMaterial);
	IRenderPipeline3D::SetTexture(mesh.m_pTexture);
	IRenderPipeline3D::SetLightingEnabled(TRUE);

	//������Ⱦ����
	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = mesh.m_pIB_Mem;
	drawCallData.pVertexBuffer = mesh.m_pVB_Mem;
	drawCallData.VertexCount = mesh.GetVertexCount();

	//��Ⱦ����
	IRenderPipeline3D::DrawTriangles(drawCallData);

}



void IRenderer::Present()
{
	//��������������
	mFunction_AdjustWindowSize();
	//������ɫ�븲����
	mFunction_UpdateCharAndTextAttrBuffer();

	COORD topLeftPos = { 0,0 };
	DWORD tmpBytesWritten = 0;

//��һ��������ǰ��������ɫ���Կ����������Ŀ���̨��Ļ�������� 
	::WriteConsoleOutputAttribute(
		m_hScreenBuffer,
		&m_pTextAttrBuffer->at(0),
		mBufferWidth*mBufferHeight,
		topLeftPos,
		&tmpBytesWritten);

	//��һ���������ַ������������Ŀ���̨��Ļ��������
	::WriteConsoleOutputCharacterA(
		m_hScreenBuffer,						
		&m_pCharBuffer->at(0),							
		mBufferWidth*mBufferHeight,
		topLeftPos,								
		&tmpBytesWritten);					
	
}

void IRenderer::SetWindowTitle(const char * titleStr)
{
	::SetConsoleTitleA(titleStr);
	*m_pConsoleWindowTitle = titleStr;
}

UINT IRenderer::GetBufferWidth()
{
	return mBufferWidth;
}

UINT IRenderer::GetBufferHeight()
{
	return mBufferHeight;
}


void IRenderer::mFunction_GeneratePalette()
{
	constexpr UINT c_ConsoleTotalColorCount = 8;
	/*
	FOREGROUND_BLUE ��ɫ
	FOREGROUND_GREEN ��ɫ
	FOREGROUND_RED ��ɫ
	FOREGROUND_INTENSITY ��ǿ
	BACKGROUND_BLUE ��ɫ����
	BACKGROUND_GREEN ��ɫ����
	BACKGROUND_RED ��ɫ����
	BACKGROUND_INTENSITY ����ɫ��ǿ
	COMMON_LVB_REVERSE_VIDEO ��ɫ
	*/

	//ǰ��ɫ��ɫ��
	std::pair<COLOR3, WORD>  fgrTextAttr[c_ConsoleTotalColorCount] =
	{
		{ COLOR3(0,0,0),				NULL },
		{ COLOR3(1.0f,0,0),			FOREGROUND_RED | FOREGROUND_INTENSITY },
		{ COLOR3(0,1.0f,0),			FOREGROUND_GREEN | FOREGROUND_INTENSITY },
		{ COLOR3(0,0,1.0f),			FOREGROUND_BLUE | FOREGROUND_INTENSITY },
		{ COLOR3(0,1.0f,1.0f),		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
		{ COLOR3(1.0f,0,1.0f),		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,0),		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,1.0f),	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
	};

	//����ɫ��ɫ��
	 std::pair<COLOR3, WORD>  bgrTextAttr[c_ConsoleTotalColorCount] =
	{
		{ COLOR3(0,0,0),				NULL },
		{ COLOR3(1.0f,0,0),			BACKGROUND_RED | BACKGROUND_INTENSITY },
		{ COLOR3(0,1.0f,0),			BACKGROUND_GREEN | BACKGROUND_INTENSITY },
		{ COLOR3(0,0,1.0f),			BACKGROUND_BLUE | BACKGROUND_INTENSITY },
		{ COLOR3(0,1.0f,1.0f),		BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
		{ COLOR3(1.0f,0,1.0f),		BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,0),		BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,1.0f),	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
	};

	constexpr UINT c_steps = 6;
	constexpr BYTE c_LerpChar[c_steps] = { ' ',	  '.' ,  '='  ,'&'  ,'#'	,'#'};//ͨ���ı��ַ��ı�ǰ��ɫ�ĸ��Ƕ�

	for (UINT i = 0;i < c_ConsoleTotalColorCount;++i)
	{
		for (UINT j = i+1;j < c_ConsoleTotalColorCount;++j)
		{
			COLOR3 colorVec = fgrTextAttr[i].first - bgrTextAttr[j].first;//���ǰ����ɫ����С����ִ�в�ֵ
			if (colorVec.Length() < 0.9f)goto label_nextLerp;

			//��������ɫ��ִ�����Բ�ֵ��Ԥ����ɫ֮�䣬��ʹ���㹻�� ascii �ַ���ȡ�������ظ����ʣ���Ϊ��ֵ����ϵ��

			for (UINT k =0;k < c_steps-1;k++)
			{
				COLOR3 newColor = Lerp(bgrTextAttr[i].first, fgrTextAttr[j].first, float(k) *1.0f / float(c_steps-1));
				
				//����ɫ������Ӧ�ĵ�ɫ���У��������ڰ˽��ƵĿ���з��ࣩ
				//�� 3 λȷ����ÿ��λΪ x / y / z
				UINT paletteID = 0;
				if (newColor.x >= 0.5)paletteID += 0x0004;
				if (newColor.y >= 0.5)paletteID += 0x0002;
				if (newColor.z >= 0.5)paletteID += 0x0001;
				
				//�����ֵ�õ�������ɫ
				Renderer_Color3ToConsolePixel tmpObj;
				tmpObj.asciiChar = c_LerpChar[k];
				tmpObj.color = newColor;
				tmpObj.textAttr = bgrTextAttr[i].second | fgrTextAttr[j].second;
				m_pPalette[paletteID]->push_back(tmpObj);
			}

		label_nextLerp:;
		}
	}
}

void IRenderer::mFunction_BlendPixel(UINT x, UINT y, float blendFactor, const COLOR3 & newColor)
{
	//�������ֵ
	if (x < mBufferWidth && y < mBufferHeight)
	{
		COLOR3& c = m_pColorBuffer->at(y*mBufferWidth + x);
		c.x = Lerp(c.x, newColor.x, blendFactor);
		c.y = Lerp(c.y, newColor.y, blendFactor);
		c.z= Lerp(c.z, newColor.z, blendFactor);
	}

}

inline void IRenderer::mFunction_SetPixel(UINT x, UINT y, const COLOR3 & color)
{
	if(x<mBufferWidth && y <mBufferHeight)
		m_pColorBuffer->at(y*mBufferWidth + x) = color;
}

inline UINT IRenderer::mFunction_GetIndex(UINT x, UINT y)
{
	return y*mBufferWidth+x;
}

void IRenderer::mFunction_AdjustWindowSize()
{

#ifdef CUBE_GAME_WIN10
	int frameX_Width = GetSystemMetrics(SM_CXBORDER);//���ڱ߿�Ŀ�ȣ�������Ϊ��λ��
	int frameY_Height = GetSystemMetrics(SM_CYBORDER);//���ڱ߿�Ŀ�ȣ�������Ϊ��λ��
	int frameY_Caption = GetSystemMetrics(SM_CYCAPTION);//�����ؼ������ͨ���ڱ���ĸ߶�
	int scrWidth = GetSystemMetrics(SM_CXSCREEN);//������Ϊ��λ�������Ļ�ߴ�
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = mBufferWidth*c_ConsoleCharSizeX + 2 * frameX_Width + WINDOWSSIZE;
	int windowHeight = int(mBufferHeight* c_ConsoleCharSizeY) + frameY_Caption + frameY_Height +20;
#endif


	//�������ڴ�С����Ӧ��������С��ֱ�����ô������ش�С��
	HWND hwnd = ::FindWindowA(NULL, m_pConsoleWindowTitle->c_str());

	//����ָ�����ڵı߿���εĴ�С
	RECT prevRect;
	::GetWindowRect(hwnd, &prevRect);

	//���ڵľ��������������λ�á����ڶ�������λ�á����ڵ��¿�ȡ����ڵ��¸߶ȡ�ָʾ�Ƿ�Ҫ���»��ƴ��ڡ�
	::MoveWindow(
		hwnd,
		prevRect.left,
		prevRect.top,
		windowWidth,
		windowHeight,
		true);

}

inline void IRenderer::mFunction_UpdateCharAndTextAttrBuffer()
{

	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		COLOR3& color = m_pColorBuffer->at(i);
//�õ���ɫ���ж�Ӧ����ɫ
		UINT paletteID = 0;
		if (color.x >= 0.5)paletteID += 0x0004;
		if (color.y >= 0.5)paletteID += 0x0002;
		if (color.z >= 0.5)paletteID += 0x0001;

		float minLength = 1.0f;
		UINT paletteColorID = 0;
		for (UINT j = 0;j < m_pPalette[paletteID]->size();++j)
		{
			//���������Ԥ����ɫ��ŷ����þ��룩
			VEC3 deltaVec = color - m_pPalette[paletteID]->at(j).color;
			float len = deltaVec.Length();
			if (len < minLength)
			{
				minLength = len;
				paletteColorID = j;
			}
		}
		
		//���ת��Ϊ����̨�ַ�
		m_pCharBuffer->at(i) = m_pPalette[paletteID]->at(paletteColorID).asciiChar;
		m_pTextAttrBuffer->at(i) = m_pPalette[paletteID]->at(paletteColorID).textAttr;
	}
}
