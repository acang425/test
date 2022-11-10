
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
	//得到标准输出句柄
	m_hScreenOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);

	//-----------------字体设置-----------------
	//用字体来代表并且控制像素
	CONSOLE_FONT_INFOEX fontInfo = { 0 };//控制台字体的信息
	::GetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);//获取指定控制台输出窗口字体的尺寸
	fontInfo.cbSize = sizeof(fontInfo);//此结构的大小（以字节为单位),必须将此成员设置为sizeof(CONSOLE_FONT_INFOEX)
	fontInfo.dwFontSize = { c_ConsoleCharSizeX, c_ConsoleCharSizeY };//包含字体中每个字符的宽度和高度
	fontInfo.FontWeight = 700;		//字体粗细。 权重的范围为100到1000，以100为倍数。 例如，正常权重为400，而700为粗体。
	::SetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);//设置字体信息


	//创建控制台屏幕缓冲区
	m_hScreenBuffer = ::CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
		);

	//隐藏光标
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;//游标的可见性。 如果光标可见，则此成员为 TRUE。
	cci.dwSize = 1;//由光标填充的字符单元格的百分比。 此值介于1到100之间。 光标外观会变化，范围从完全填充单元格到单元底部的水平线条。
	::SetConsoleCursorInfo(m_hScreenOutput, &cci);//设置指定控制台屏幕缓冲区的光标的大小和可见性
	::SetConsoleCursorInfo(m_hScreenBuffer, &cci);


	//----------缓冲区的初始化-----------------
	BOOL isSuceeded = FALSE;

	COORD maxWindowSize = ::GetLargestConsoleWindowSize(m_hScreenOutput);//控制台窗口中 (X 成员) 和 (Y 成员) 行的字符单元列数。
	mBufferWidth = SHORT(bufferWidth) > maxWindowSize.X ? maxWindowSize.X : bufferWidth;
	mBufferHeight = SHORT(bufferHeight) > maxWindowSize.Y ? maxWindowSize.Y : bufferHeight;


	COORD dwBuffSize;//控制台屏幕缓冲区中字符单元格的坐标。
	dwBuffSize.X = SHORT(bufferWidth);
	dwBuffSize.Y = SHORT(bufferHeight);
	isSuceeded = ::SetConsoleScreenBufferSize(m_hScreenBuffer, dwBuffSize);//更改指定的控制台屏幕缓冲区的大小。


	//初始化输出信息缓存
	m_pCharBuffer = new std::vector<char>(mBufferHeight*mBufferWidth);
	m_pZBuffer = new std::vector<float>(mBufferHeight*mBufferWidth);
	m_pColorBuffer = new std::vector<COLOR3>(mBufferHeight*mBufferWidth);
	m_pTextAttrBuffer = new std::vector<WORD>(mBufferHeight*mBufferWidth);

	//将指定的屏幕缓冲区设置为当前显示的控制台屏幕缓冲区。
	::SetConsoleActiveScreenBuffer(m_hScreenBuffer);

	//----------------------------初始化渲染管线-----------------------
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
		//将颜色缓存设为同一颜色
		m_pColorBuffer->at(i) = clearColor;
	}

	//统一深度缓存
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
	//限定坐标范围
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

	//计算x和y在所在直线的比例，映射到贴图
	for (UINT i = x1;i <= x2;++i)
	{
		float x_ratio = float(i - x1) / drawRegionWidth;
		for (UINT j = y1;j <= y2;++j)
		{
			float y_ratio = float(j - y1) / drawRegionHeight;
			UINT picCoordX = UINT(pic.mWidth*x_ratio);
			UINT picCoordY = UINT(pic.mHeight*y_ratio);
			//设置该点像素值
			mFunction_SetPixel(i, j, pic.GetPixel(picCoordX, picCoordY));
		}
	}
	return TRUE;
}


void IRenderer::DrawTriangle(COLOR3 color, const VEC2 & v1_pixel, const VEC2 & v2_pixel, const VEC2 & v3_pixel)
{
	//初始化三个矩阵为单位矩阵
	MATRIX4 matIdentity;
	matIdentity.Identity();
	IRenderPipeline3D::SetWorldMatrix(matIdentity);
	IRenderPipeline3D::SetProjMatrix(matIdentity);
	IRenderPipeline3D::SetViewMatrix(matIdentity);

	//转换到齐次坐标
	auto convertToHomoSpace = [&](const VEC2& v, VEC2& outV)
	{
		outV.x = (v.x / float(mBufferWidth) *2.0f) - 1.0f;
		outV.y = (-v.y / float(mBufferHeight) *2.0f) + 1.0f;
	};
	VEC2 v1, v2, v3;
	convertToHomoSpace(v1_pixel, v1);
	convertToHomoSpace(v2_pixel, v2);
	convertToHomoSpace(v3_pixel, v3);

	//渲染管线会将齐次裁剪空间转化到像素空间
	//初始化顶点坐标
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

	//初始化顶点序号
	std::vector<UINT> indexArray = { 0,1,2 };

	//初始化渲染数据
	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = &indexArray;
	drawCallData.pVertexBuffer = &vertexArray;
	drawCallData.VertexCount = 3;

	//渲染管线开画！
	IRenderPipeline3D::DrawTriangles(drawCallData);

}



void IRenderer::RenderMesh(IMesh& mesh)
{
	if (m_pCamera == nullptr)return;

	//设置世界矩阵，视图矩阵，透视投影变换矩阵
	MATRIX4 matW, matV, matP;
	mesh.GetWorldMatrix(matW);
	m_pCamera->GetViewMatrix(matV);
	m_pCamera->GetProjMatrix(matP);

	IRenderPipeline3D::SetWorldMatrix(matW);
	IRenderPipeline3D::SetProjMatrix(matP);
	IRenderPipeline3D::SetViewMatrix(matV);

	//设置相机，材质，贴图，光照
	IRenderPipeline3D::SetCameraPos(m_pCamera->GetPosition());
	IRenderPipeline3D::SetMaterial(mesh.mMaterial);
	IRenderPipeline3D::SetTexture(mesh.m_pTexture);
	IRenderPipeline3D::SetLightingEnabled(TRUE);

	//设置渲染数据
	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = mesh.m_pIB_Mem;
	drawCallData.pVertexBuffer = mesh.m_pVB_Mem;
	drawCallData.VertexCount = mesh.GetVertexCount();

	//渲染管线
	IRenderPipeline3D::DrawTriangles(drawCallData);

}



void IRenderer::Present()
{
	//呈现整个缓冲区
	mFunction_AdjustWindowSize();
	//更新颜色与覆盖率
	mFunction_UpdateCharAndTextAttrBuffer();

	COORD topLeftPos = { 0,0 };
	DWORD tmpBytesWritten = 0;

//将一定数量的前景背景颜色属性拷贝到连续的控制台屏幕缓冲区中 
	::WriteConsoleOutputAttribute(
		m_hScreenBuffer,
		&m_pTextAttrBuffer->at(0),
		mBufferWidth*mBufferHeight,
		topLeftPos,
		&tmpBytesWritten);

	//将一定数量的字符拷贝到连续的控制台屏幕缓冲区中
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
	FOREGROUND_BLUE 蓝色
	FOREGROUND_GREEN 绿色
	FOREGROUND_RED 红色
	FOREGROUND_INTENSITY 加强
	BACKGROUND_BLUE 蓝色背景
	BACKGROUND_GREEN 绿色背景
	BACKGROUND_RED 红色背景
	BACKGROUND_INTENSITY 背景色加强
	COMMON_LVB_REVERSE_VIDEO 反色
	*/

	//前景色调色盘
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

	//背景色调色盘
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
	constexpr BYTE c_LerpChar[c_steps] = { ' ',	  '.' ,  '='  ,'&'  ,'#'	,'#'};//通过改变字符改变前景色的覆盖度

	for (UINT i = 0;i < c_ConsoleTotalColorCount;++i)
	{
		for (UINT j = i+1;j < c_ConsoleTotalColorCount;++j)
		{
			COLOR3 colorVec = fgrTextAttr[i].first - bgrTextAttr[j].first;//如果前后颜色差别较小，则不执行插值
			if (colorVec.Length() < 0.9f)goto label_nextLerp;

			//生成新颜色，执行线性插值在预设颜色之间，并使用足够的 ascii 字符（取决于像素覆盖率）作为插值比例系数

			for (UINT k =0;k < c_steps-1;k++)
			{
				COLOR3 newColor = Lerp(bgrTextAttr[i].first, fgrTextAttr[j].first, float(k) *1.0f / float(c_steps-1));
				
				//将颜色放入相应的调色板中（按类似于八进制的块进行分类）
				//由 3 位确定，每个位为 x / y / z
				UINT paletteID = 0;
				if (newColor.x >= 0.5)paletteID += 0x0004;
				if (newColor.y >= 0.5)paletteID += 0x0002;
				if (newColor.z >= 0.5)paletteID += 0x0001;
				
				//储存插值得到的新颜色
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
	//混合像素值
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
	int frameX_Width = GetSystemMetrics(SM_CXBORDER);//窗口边框的宽度，以像素为单位。
	int frameY_Height = GetSystemMetrics(SM_CYBORDER);//窗口边框的宽度，以像素为单位。
	int frameY_Caption = GetSystemMetrics(SM_CYCAPTION);//以像素计算的普通窗口标题的高度
	int scrWidth = GetSystemMetrics(SM_CXSCREEN);//以像素为单位计算的屏幕尺寸
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = mBufferWidth*c_ConsoleCharSizeX + 2 * frameX_Width + WINDOWSSIZE;
	int windowHeight = int(mBufferHeight* c_ConsoleCharSizeY) + frameY_Caption + frameY_Height +20;
#endif


	//调整窗口大小以适应缓冲区大小（直接设置窗口像素大小）
	HWND hwnd = ::FindWindowA(NULL, m_pConsoleWindowTitle->c_str());

	//返回指定窗口的边框矩形的大小
	RECT prevRect;
	::GetWindowRect(hwnd, &prevRect);

	//窗口的句柄。窗口左侧的新位置。窗口顶部的新位置。窗口的新宽度。窗口的新高度。指示是否要重新绘制窗口。
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
//得到调色盘中对应的颜色
		UINT paletteID = 0;
		if (color.x >= 0.5)paletteID += 0x0004;
		if (color.y >= 0.5)paletteID += 0x0002;
		if (color.z >= 0.5)paletteID += 0x0001;

		float minLength = 1.0f;
		UINT paletteColorID = 0;
		for (UINT j = 0;j < m_pPalette[paletteID]->size();++j)
		{
			//查找最近的预设颜色（欧几里得距离）
			VEC3 deltaVec = color - m_pPalette[paletteID]->at(j).color;
			float len = deltaVec.Length();
			if (len < minLength)
			{
				minLength = len;
				paletteColorID = j;
			}
		}
		
		//最后转化为控制台字符
		m_pCharBuffer->at(i) = m_pPalette[paletteID]->at(paletteColorID).asciiChar;
		m_pTextAttrBuffer->at(i) = m_pPalette[paletteID]->at(paletteColorID).textAttr;
	}
}
