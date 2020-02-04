#pragma once
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "Dependencies/common/lexical_cast.hpp"
#include "MiniExcelReader.h"
#include <map>

enum ColType
{
	COL_TYPE_ID = 1,
	COL_TYPE_NAME = 2,
	COL_TYPE_MOUDEL = 3,
	COL_TYPE_PROTO = 4,
	COL_TYPE_DESC = 5,
	COL_TYPE_TO_CS = 6,
};
struct EnumData
{
	std::string strId = "";
	std::string strName = "";
	std::string strMoudle = "";
	std::string strProto = "";
	std::string strDesc = "";
	bool bNotToCS = false;
};

class EnumDefine
{
public:
	EnumDefine();
	virtual ~EnumDefine();

	void SetUTF8(const bool b);

	bool LoadDataFromExcel(std::string& strFile);

	bool SaveForCPP(std::string file, std::string strSpace);
	bool SaveForCS(std::string file, std::string strSpace);
private:
	
	bool LoadDataFromExcel(MiniExcelReader::Sheet& sheet);

private:
	typedef std::map<std::string, std::vector<EnumData>> MapEnumData;
	typedef std::map<std::string, MapEnumData> MapData;
	MapData m_mapData;
	std::map<std::string, std::map<std::string, int>> m_mapId;
	std::map<std::string, int> m_mapName;

	std::string m_strFileName = "";
	std::string m_strFilePath = "";
};
