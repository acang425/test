#pragma once

#define CUBE_GAME_WIN10
#ifdef CUBE_GAME_WIN10
const short c_ConsoleCharSizeX = 4;//一个字符的大小（以像素为单位）
const short c_ConsoleCharSizeY = 6;//一个字符的大小（以像素为单位）
const float c_ConsoleCharAspectRatio = 4.0f / 6.0f;//用来矫正像素的大小
#endif


class IRenderer : private IRenderPipeline3D
{
public:
	const int WINDOWSSIZE = 32;
	IRenderer();

	~IRenderer();

	void		Init(UINT bufferWidth,UINT bufferHeight);//初始化

	void		Clear(COLOR3 clearColor=COLOR3(0,0,0),BOOL clearZBuff=TRUE);//清除缓冲区

	//-------------------------------SET---------------------------

	void		SetCamera(ICamera& cam);

	void		SetLight(UINT index,const  DirectionalLight& light);

	void		SetWindowTitle(const char* titleStr);

	//----------------------------渲染----------------------

	void		RenderMesh(IMesh& mesh);

	BOOL	DrawPicture(IPicture& pic, UINT x1, UINT y1, UINT x2, UINT y2);

	void		DrawTriangle(COLOR3 color, const VEC2& v1_pixel, const VEC2& v2_pixel, const VEC2& v3_pixel);

	void		Present();

	//----------------------------GET----------------------

	UINT		GetBufferWidth();

	UINT		GetBufferHeight();

private:
	//----------------------------------调色盘-----------------------------
	void			mFunction_GeneratePalette();

	void			mFunction_BlendPixel(UINT x, UINT y,float blendFactor, const COLOR3& newColor);

	void			mFunction_SetPixel(UINT x, UINT y, const COLOR3& color);

	UINT			mFunction_GetIndex(UINT x, UINT y);

	void			mFunction_AdjustWindowSize();

	void			mFunction_UpdateCharAndTextAttrBuffer();


	//--------------------------屏幕信息---------------------------
	DWORD	mBufferWidth;
	DWORD	mBufferHeight;
	HANDLE	m_hScreenOutput;
	HANDLE	m_hScreenBuffer;
	std::string* m_pConsoleWindowTitle;

	//----------------------渲染信息-----------------------------
	ICamera*						m_pCamera;

	std::vector<float>*		m_pZBuffer;
	std::vector<char>*		m_pCharBuffer;
	std::vector<COLOR3>*	m_pColorBuffer;
	std::vector<WORD>*	m_pTextAttrBuffer;

	std::vector<Renderer_Color3ToConsolePixel>*		m_pPalette[8];
};