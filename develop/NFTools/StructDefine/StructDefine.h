#pragma once
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "Dependencies/common/lexical_cast.hpp"
#include "MiniExcelReader.h"
#include <map>

class Property
{
public:
	std::string name = "";
	std::string type = "";
	std::string desc = "";
	bool bPrivate = false;
	bool bPublic = false;
	bool bSave = false;
};

class ColData
{
public:
	std::string tag = "";
	std::string type = "";
	std::string desc = "";
	int index = 0;
};

class Record
{
public:
	std::string name = "";
	std::string row = "";
	std::string col = "";
	bool bPrivate = false;
	bool bPublic = false;
	std::string desc = "";
	std::vector<ColData> vecCol;
};

class ClassData
{
public:
	std::string name = "";
	std::string type = "";
	std::string desc = "";
	std::map<std::string, Property> mapProp;
	std::map<std::string, Record> mapRecord;
	std::vector<NF_SHARE_PTR<ClassData>> vecChild;

	NF_SHARE_PTR<ClassData> parent = nullptr;
	std::vector<std::string> vecIncludes;

	bool FindInclude(std::string& strPath, std::string& strParent)
	{
		auto it = std::find(vecIncludes.begin(), vecIncludes.end(), strPath);
		if (it != vecIncludes.end())
		{
			strParent = this->name;
			return true;
		}

		if (parent != nullptr)
		{
			return parent->FindInclude(strPath, strParent);
		}

		return false;
	}

	bool FindProperty(Property& other, std::string& strParent)
	{
		auto it = mapProp.find(other.name);
		if (it != mapProp.end())
		{
			if (it->second.bPrivate == other.bPrivate 
				&& it->second.bPublic == other.bPublic)
			{
				strParent = this->name;
				return true;
			}
		}

		if (parent != nullptr)
		{
			return parent->FindProperty(other, strParent);
		}

		return false;
	}

	bool FindRecord(Record& other, std::string& strParent)
	{
		auto it = mapRecord.find(other.name);
		if (it != mapRecord.end())
		{
			if (it->second.bPrivate == other.bPrivate
				&& it->second.bPublic == other.bPublic)
			{
				strParent = this->name;
				return true;
			}
		}

		if (parent != nullptr)
		{
			return parent->FindRecord(other, strParent);
		}

		return false;
	}
};

class StructDefine
{
public:
	StructDefine();
	virtual ~StructDefine();

	bool ParseClass(std::string filepath, std::string strPath);
	bool Save(std::string nameFile, std::string nameSpace);
private:
	bool ReadFile(std::string& strPath, std::string& strContent);
	bool LoadClass(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<ClassData> pData);
	bool ParseData(std::string strPath, ClassData& data);
	bool LoadPropertys(rapidxml::xml_node<>* node, ClassData& data);
	bool LoadRecords(rapidxml::xml_node<>* node, ClassData& data);

	void SaveCpp();
	void SaveClassCpp(FILE* hppWriter, const ClassData& data);

	void SaveCs();
	void SaveClassCs(FILE* hppWriter, const ClassData& data);
private:
	std::vector<NF_SHARE_PTR<ClassData>> vecClassData;

	// 文件目录
	std::string strFilePath = "";

	// 保存的文件名
	std::string strSaveName = "";
	// 命名空间名称
	std::string strNameSpace = "";
};
