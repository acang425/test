#include "MyConsoleEngine.h"

//---------------------------�ļ�������-----------------------------


BOOL IFileManager::ImportOBJ(std::string pFilePath, std::vector<Vertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer)
{
	//�����ļ������������ļ�
	std::fstream fileIn(pFilePath);
	if (!fileIn.good())
	{
		DEBUG_MSG1("Import OBJ : Open File failed!!");
		return FALSE;
	}

	std::vector<VEC3> pointList;//���㻺��
	std::vector<VEC2> texcoordList;//�������껺��
	std::vector<VEC3> VNormalList;//���㷨�߻���
	std::vector<OBJ_vertexInfoIndex> vertexInfoList;//������Ż���


//һ���еض���obj�ļ��е�string
	std::string currString;
	while (!fileIn.eof())
	{
		fileIn >> currString;

		//�����ʽ: "v 1.0000000 0.52524242 5.12312345"
		if (currString == "v")
		{
			VEC3 currPoint(0, 0, 0);
			fileIn >> currPoint.x >> currPoint.y >> currPoint.z;
			pointList.push_back(currPoint);
		}

		//���㷨�߸�ʽ�� "vn 1.0000000 0.52524242 5.12312345"
		if (currString == "vn")
		{
			VEC3 currNormal(0, 0, 0);
			fileIn >> currNormal.x >> currNormal.y >> currNormal.z;
			VNormalList.push_back(currNormal);
		}

		//���������ʽ�� "vt 1.0000000 0.0000000"
		if (currString == "vt")
		{
			VEC2 currTexCoord(0, 0);
			fileIn >> currTexCoord.x >> currTexCoord.y;
			texcoordList.push_back(currTexCoord);
		}

		//���ڰ���������Ϊƽ���OBJ�ļ�
			//��ĸ�ʽΪ ��ÿ�������г���������� /�����������/������� 
			// ���ӣ�"1/3/4" "5/6/7"
		if (currString == "f")
		{
			for (UINT i = 0;i < 3;++i)
			{
				OBJ_vertexInfoIndex currVertex = { 0,0,0 };
				std::stringstream tmpSStream;
				std::string tmpString;

				fileIn >> tmpString;

				//��/�ÿո����������stringstream�����Զ������ַ���
				for (auto& c : tmpString) { if (c == '/')c = ' '; }

				tmpSStream << tmpString;
				tmpSStream >> currVertex.vertexID >> currVertex.texcoordID >> currVertex.vertexNormalID;
				--currVertex.texcoordID;
				--currVertex.vertexID;
				--currVertex.vertexNormalID;
				//Vector�����ԡ�����ʼ�±�Ϊ0

				//Ϊ�˽�������ʱ�临�Ӷȣ���������ѱ������������ٴα�pushback�����㻺����
				BOOL IsVertexExist = FALSE;
				UINT  existedVertexIndex = 0;
				//�������㻺�棬ȷ�϶����Ƿ��ѱ�����
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
					refIndexBuffer.push_back(vertexInfoList.size() - 1);//�½��������
				}
				else
				{
					refIndexBuffer.push_back(existedVertexIndex);
				}
			}
		}
	}

	fileIn.close();

	//��������ϵĶ��㻺�����������浼��
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

	//��PPM�ļ���ͷ��������ĸ����
	fileIn.seekg(3);
	std::streampos filePos = fileIn.tellg();
	//����ͼ��Ŀ�Ⱥ͸߶ȣ�ͨ��ASCII��ʾ�������������ֵ��0 - 255�ֽڱ�ʾ
	fileIn >> outWidth>> outHeight>> maxColorIntensity;

	//�����س�
	fileIn.seekg(1, std::ios::cur);

	//PPM��24bits����ÿһ�����أ��������ֱ�ռ��8bits
	unsigned char* byteBuff=new unsigned char[outWidth*outHeight*3];
	fileIn.read((char*)byteBuff, 3 * outWidth*outHeight);
	outColorBuffer.resize(outWidth*outHeight);
	fileIn.close();

	//��������ϵ�����ֵ���浼��
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
		fileOut.open(filePath.c_str(), std::ios::trunc);//���������ļ�
	}
	else
	{
		fileOut.open(filePath.c_str(), std::ios::_Noreplace);//����ļ��������������ļ�,����ļ��Դ��������ʧ��,noplace����˼�ǲ��������ļ�
	};

	if(fileOut.good()==FALSE)
	{
		DEBUG_MSG1("Save PPM : File Open Failed!!");
		return FALSE;
	}

	//��д�ļ�ͷ������ImportPPM�еĸ�ʽ���PPM
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

