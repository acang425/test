#pragma once

class IFileManager
{
public:

	BOOL ImportOBJ(std::string pFilePath, std::vector<Vertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer);//����obj�ļ�

	BOOL ImportPPM(std::string filePath, UINT& outWidth,UINT& outHeight,std::vector<COLOR3>& outColorBuffer);//����ppm�ļ�

	BOOL ExportPPM(std::string filePath,UINT width,UINT height,const std::vector<COLOR3>& colorBuffer, BOOL bOverrideExistedFile);//���ppm�ļ�

};