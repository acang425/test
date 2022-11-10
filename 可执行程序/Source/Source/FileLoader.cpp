#include "MyConsoleEngine.h"

//---------------------------文件导入类-----------------------------


BOOL IFileManager::ImportOBJ(std::string pFilePath, std::vector<Vertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer)
{
	//利用文件流函数读入文件
	std::fstream fileIn(pFilePath);
	if (!fileIn.good())
	{
		DEBUG_MSG1("Import OBJ : Open File failed!!");
		return FALSE;
	}

	std::vector<VEC3> pointList;//顶点缓存
	std::vector<VEC2> texcoordList;//纹理坐标缓存
	std::vector<VEC3> VNormalList;//顶点法线缓存
	std::vector<OBJ_vertexInfoIndex> vertexInfoList;//顶点序号缓存


//一行行地读入obj文件中的string
	std::string currString;
	while (!fileIn.eof())
	{
		fileIn >> currString;

		//顶点格式: "v 1.0000000 0.52524242 5.12312345"
		if (currString == "v")
		{
			VEC3 currPoint(0, 0, 0);
			fileIn >> currPoint.x >> currPoint.y >> currPoint.z;
			pointList.push_back(currPoint);
		}

		//顶点法线格式： "vn 1.0000000 0.52524242 5.12312345"
		if (currString == "vn")
		{
			VEC3 currNormal(0, 0, 0);
			fileIn >> currNormal.x >> currNormal.y >> currNormal.z;
			VNormalList.push_back(currNormal);
		}

		//纹理坐标格式： "vt 1.0000000 0.0000000"
		if (currString == "vt")
		{
			VEC2 currTexCoord(0, 0);
			fileIn >> currTexCoord.x >> currTexCoord.y;
			texcoordList.push_back(currTexCoord);
		}

		//对于把三角形作为平面的OBJ文件
			//面的格式为 ：每个顶点列出：顶点序号 /纹理坐标序号/法线序号 
			// 例子："1/3/4" "5/6/7"
		if (currString == "f")
		{
			for (UINT i = 0;i < 3;++i)
			{
				OBJ_vertexInfoIndex currVertex = { 0,0,0 };
				std::stringstream tmpSStream;
				std::string tmpString;

				fileIn >> tmpString;

				//将/用空格替代，这样stringstream才能自动分离字符串
				for (auto& c : tmpString) { if (c == '/')c = ' '; }

				tmpSStream << tmpString;
				tmpSStream >> currVertex.vertexID >> currVertex.texcoordID >> currVertex.vertexNormalID;
				--currVertex.texcoordID;
				--currVertex.vertexID;
				--currVertex.vertexNormalID;
				//Vector的特性——起始下标为0

				//为了降低搜索时间复杂度，如果顶点已被创建，不会再次被pushback进顶点缓存中
				BOOL IsVertexExist = FALSE;
				UINT  existedVertexIndex = 0;
				//遍历顶点缓存，确认顶点是否已被创建
				for (UINT j = 0;j<vertexInfoList.size();j++)
				{
					if (vertexInfoList[j] == currVertex)
					{
						IsVertexExist = TRUE;
						existedVertexIndex =j;
						break;
					}
				}
				if (!IsVertexExist)
				{
					vertexInfoList.push_back(currVertex);
					refIndexBuffer.push_back(vertexInfoList.size() - 1);//新建顶点序号
				}
				else
				{
					refIndexBuffer.push_back(existedVertexIndex);
				}
			}
		}
	}

	fileIn.close();

	//将处理完毕的顶点缓存与索引缓存导出
	refVertexBuffer.resize(vertexInfoList.size());
	for (UINT i = 0;i < refVertexBuffer.size();++i)
	{
		Vertex tmpVertex = {};

		OBJ_vertexInfoIndex& indicesCombination = vertexInfoList.at(i);
		tmpVertex.pos = pointList.at(indicesCombination.vertexID);
		tmpVertex.normal = VNormalList.at(indicesCombination.vertexNormalID);
		tmpVertex.texcoord = texcoordList.at(indicesCombination.texcoordID);
		tmpVertex.color = VEC4(1.0f, 1.0f, 1.0f,1.0f);

		refVertexBuffer.at(i) = (tmpVertex);
	}

	return TRUE;
}


BOOL IFileManager::ImportPPM(std::string filePath, UINT & outWidth, UINT & outHeight, std::vector<COLOR3>& outColorBuffer)
{
	std::ifstream fileIn(filePath,std::ios::binary);
	if (fileIn.good() == FALSE)
	{
		DEBUG_MSG1("Load PPM : File Open Failed!!");
		return FALSE;
	}
	UINT maxColorIntensity;

	//将PPM文件开头的两个字母跳过
	fileIn.seekg(3);
	std::streampos filePos = fileIn.tellg();
	//载入图像的宽度和高度，通过ASCII表示，载入最大像素值，0 - 255字节表示
	fileIn >> outWidth>> outHeight>> maxColorIntensity;

	//跳过回车
	fileIn.seekg(1, std::ios::cur);

	//PPM用24bits代表每一个像素，红绿蓝分别占用8bits
	unsigned char* byteBuff=new unsigned char[outWidth*outHeight*3];
	fileIn.read((char*)byteBuff, 3 * outWidth*outHeight);
	outColorBuffer.resize(outWidth*outHeight);
	fileIn.close();

	//将处理完毕的像素值缓存导出
	for (UINT i = 0;i < outWidth*outHeight;++i)
	{
		COLOR3 tmpColor;
		tmpColor = { byteBuff[3 * i]/255.0f,byteBuff[3 * i+1] / 255.0f,byteBuff[3 * i+2] / 255.0f };
		outColorBuffer.at(i) = tmpColor;
	}

	return TRUE;
}

BOOL IFileManager::ExportPPM(std::string filePath, UINT width, UINT height, const std::vector<COLOR3>& colorBuffer, BOOL bOverrideExistedFile)
{
	std::ofstream fileOut;
	if (bOverrideExistedFile == TRUE)
	{
		fileOut.open(filePath.c_str(), std::ios::trunc);//覆盖已有文件
	}
	else
	{
		fileOut.open(filePath.c_str(), std::ios::_Noreplace);//如果文件不存在则建立新文件,如果文件以存在则操作失败,noplace的意思是不建立新文件
	};

	if(fileOut.good()==FALSE)
	{
		DEBUG_MSG1("Save PPM : File Open Failed!!");
		return FALSE;
	}

	//手写文件头，按照ImportPPM中的格式输出PPM
	fileOut << "P6" << std::endl;
	fileOut << width << std::endl;
	fileOut << height << std::endl;
	fileOut << 255 << std::endl;
	for (auto& c : colorBuffer)
	{
		unsigned char tmpByteColor[3] = { BYTE(c.x * 255),BYTE(c.y * 255), BYTE(c.z * 255) };
		fileOut.write((char*)&tmpByteColor[0], 3);
	}
	fileOut.close();

	return TRUE;
}

