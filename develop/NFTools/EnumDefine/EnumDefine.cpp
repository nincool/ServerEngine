#include "EnumDefine.h"
#include "Utf8ToGbk.h"
#include <iostream>
#if NF_PLATFORM == NF_PLATFORM_WIN
#include <io.h>
#include <windows.h>
#include <conio.h>
#else
#include <iconv.h>
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#endif
#include "NFComm/NFCore/NFDateTime.hpp"
#include <direct.h>


EnumDefine::EnumDefine()
{

}

EnumDefine::~EnumDefine()
{

}

bool EnumDefine::LoadDataFromExcel(std::string & strFile)
{
	std::cout << "load " << strFile << std::endl;

	MiniExcelReader::ExcelFile* xExcel = new MiniExcelReader::ExcelFile();
	if (!xExcel->open(strFile.c_str()))
	{
		std::cout << "can't open" << strFile << std::endl;
		return false;
	}

	std::string::size_type pos = 0;
	while((pos = strFile.find("\\")) != std::string::npos)
	{
		strFile.replace(pos, 1, "/");
	}
	
	int nBegin = strFile.rfind("/") + 1;
	int nEnd = strFile.rfind(".");
	m_strFileName = strFile.substr(nBegin, nEnd - nBegin);
	m_strFilePath = strFile.substr(0, nBegin);

	std::vector<MiniExcelReader::Sheet>& sheets = xExcel->sheets();
	for (MiniExcelReader::Sheet& sh : sheets)
	{
		LoadDataFromExcel(sh);
	}
	return true;
}

bool EnumDefine::LoadDataFromExcel(MiniExcelReader::Sheet & sheet)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();

	std::string strSheetName = sheet.getName();
	if (strSheetName.empty())
	{
		std::cout << "sheet name empty" << std::endl;
		return false;
	}

	for (int i = dim.firstRow + 1; i <= dim.lastRow; ++i)
	{
		EnumData data;
		for (int j = dim.firstCol; j <= dim.lastCol; ++j)
		{
			MiniExcelReader::Cell* pCell = sheet.getCell(i, j);
			if (pCell == NULL)
			{
				if (j == COL_TYPE_ID || j == COL_TYPE_NAME)
				{
					std::cout << "sheet:" << strSheetName << " row:" << i << " col:" << j << " empty" << std::endl;
					return false;
				}
				
				continue;
			}

			switch (j)
			{
			case COL_TYPE_ID:
				data.strId = pCell->value;
				break;
			case COL_TYPE_NAME:
				data.strName = pCell->value;
				break;
			case COL_TYPE_MOUDEL:
				data.strMoudle = pCell->value;
				break;
			case COL_TYPE_PROTO:
				data.strProto = pCell->value;
				break;
			case COL_TYPE_DESC:
				data.strDesc = pCell->value;
				break;
			case COL_TYPE_TO_CS:
				data.bNotToCS = pCell->value == "1";
				break;
			}
		}

		auto iterId = m_mapId[strSheetName.c_str()].find(data.strId.c_str());
		if (iterId != m_mapId[strSheetName.c_str()].end())
		{
			std::cout << "sheet:" << strSheetName << " id:" << data.strId << " exist" << std::endl;
			return false;
		}
		m_mapId[strSheetName.c_str()][data.strId] = 0;

		std::map<std::string, int>::iterator iterName = m_mapName.find(data.strName.c_str());
		if (iterName != m_mapName.end())
		{
			std::cout << "sheet:" << strSheetName << " Name:" << data.strName << " exist" << std::endl;
			return false;
		}
		m_mapName[data.strName.c_str()] = 0;

		m_mapData[strSheetName.c_str()][data.strMoudle].push_back(data);
	}
	
	return true;
}

bool EnumDefine::SaveForCPP(std::string file, std::string strSpace)
{
	NFDateTime now = NFDateTime::Now();
	std::string strUpperName = m_strFileName;
	transform(strUpperName.begin(), strUpperName.end(), strUpperName.begin(), ::toupper);
	std::string strFilePath = m_strFilePath + (file.empty() ? "" : file + "/");
	_mkdir(strFilePath.c_str());
	strFilePath += m_strFileName + ".h";
	std::cout << "create cpp file: " << strFilePath << std::endl;

	std::string strFileHead;
	strFileHead = strFileHead
		+ "// -------------------------------------------------------------------------\n"
		+ "//    @FileName\t:\t" + m_strFileName + ".h\n"
		+ "//    @Author  \t:\tEnumDefine Tool\n"
		+ "// -------------------------------------------------------------------------\n\n"
		+ "#ifndef __" + strUpperName + "_H__\n"
		+ "#define __" + strUpperName + "_H__\n"
		+ "namespace " + strSpace + "\n{\n";
	std::string strFileEnd = "}\n#endif // __" + strUpperName + "_H__";

	std::string strFileBody;
	for (auto it:m_mapData)
	{
		strFileBody += "\nenum " + it.first + "\n{\n";

		for (auto itModule:it.second)
		{
			strFileBody += "\n\t// " + itModule.first + "\n";

			for (auto itVec:itModule.second)
			{
				strFileBody = strFileBody
					+ "\t" + itVec.strName + " = " + itVec.strId + ",\t// " + itVec.strProto + "\t" + itVec.strDesc + "\n";
			}
		}

		strFileBody += "};\n\n";
	}
		
	FILE* hWriter = fopen(strFilePath.c_str(), "w+");
	fwrite(strFileHead.c_str(), strFileHead.length(), 1, hWriter);
	fwrite(strFileBody.c_str(), strFileBody.length(), 1, hWriter);
	fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, hWriter);
	fclose(hWriter);

	return false;
}

bool EnumDefine::SaveForCS(std::string file, std::string strSpace)
{
	NFDateTime now = NFDateTime::Now();
	std::string strUpperName = m_strFileName;
	transform(strUpperName.begin(), strUpperName.end(), strUpperName.begin(), ::toupper);
	std::string strFilePath = m_strFilePath + (file.empty() ? "" : file + "/");
	_mkdir(strFilePath.c_str());
	strFilePath += m_strFileName + ".cs";
	std::cout << "create cs file: " << strFilePath << std::endl;

	std::string strFileHead;
	strFileHead = strFileHead
		+ "// -------------------------------------------------------------------------\n"
		+ "//    @FileName\t:\t" + m_strFileName + ".cs\n"
		+ "//    @Author  \t:\tEnumDefine Tool\n"
		+ "// -------------------------------------------------------------------------\n\n"
		+ "namespace " + strSpace + "\n{\n";
	std::string strFileEnd = "}\n";

	std::string strFileBody;
	for (auto it : m_mapData)
	{
		strFileBody += "\npublic enum " + it.first + "\n{\n";

		for (auto itModule : it.second)
		{
			bool bFirst = true;
			for (auto itVec : itModule.second)
			{
				if (itVec.bNotToCS)
				{
					continue;
				}

				if (bFirst)
				{
					strFileBody += "\n\t// " + itModule.first + "\n";
					bFirst = false;
				}

				strFileBody = strFileBody
					+ "\t" + itVec.strName + " = " + itVec.strId + ",\t// " + itVec.strProto + "\t" + itVec.strDesc + "\n";
			}
		}

		strFileBody += "}\n\n";
	}

	FILE* hWriter = fopen(strFilePath.c_str(), "w+");
	fwrite(strFileHead.c_str(), strFileHead.length(), 1, hWriter);
	fwrite(strFileBody.c_str(), strFileBody.length(), 1, hWriter);
	fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, hWriter);
	fclose(hWriter);

	return false;
}
