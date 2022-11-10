#pragma once

class IPicture :private IFileManager
{
public:
		
	friend class IRenderer;

	IPicture();

	~IPicture();

	BOOL	LoadPPM(std::string ppmFilePath);

	UINT		GetWidth();

	UINT		GetHeight();

	COLOR3	GetPixel(UINT x, UINT y);

private:
	std::vector<COLOR3>* m_pColorBuffer;
	UINT mWidth;
	UINT mHeight;

};