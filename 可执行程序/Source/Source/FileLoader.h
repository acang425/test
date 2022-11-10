#pragma once

class IFileManager
{
public:

	BOOL ImportOBJ(std::string pFilePath, std::vector<Vertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer);//载入obj文件

	BOOL ImportPPM(std::string filePath, UINT& outWidth,UINT& outHeight,std::vector<COLOR3>& outColorBuffer);//载入ppm文件

	BOOL ExportPPM(std::string filePath,UINT width,UINT height,const std::vector<COLOR3>& colorBuffer, BOOL bOverrideExistedFile);//输出ppm文件

};