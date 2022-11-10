

#include "MyConsoleEngine.h"

IPicture::IPicture()
{
	m_pColorBuffer = new std::vector<COLOR3>;
}

IPicture::~IPicture()
{
	delete m_pColorBuffer;
}

BOOL IPicture::LoadPPM(std::string ppmFilePath)
{
	BOOL yes=IFileManager::ImportPPM(ppmFilePath, mWidth, mHeight, *m_pColorBuffer);//把PPM中的像素值“搬运”到图片类中
	if (yes == TRUE)
	{
		return TRUE;
	}
	else
	{
		mWidth = 0;
		mHeight = 0;
		DEBUG_MSG1("IPicture: Load PPM failed!!");
		return FALSE;
	}
}

UINT IPicture::GetWidth()
{
	return mWidth;
}

UINT IPicture::GetHeight()
{
	return mHeight;
}


COLOR3 IPicture::GetPixel(UINT x, UINT y)
{
	if (x<mWidth && y<mHeight)
	{
		UINT index = y*mWidth + x;
		return m_pColorBuffer->at(index);
	}
	return COLOR3(255, 0, 0);
}
