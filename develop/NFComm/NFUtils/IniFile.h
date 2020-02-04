///--------------------------------------------------------------------
// 文件名:		IniFile.h 
// 内  容:		初始化文件类声明
/// 说  明:		
/// 创建日期:	2019年10月24日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _INIFILE_H
#define _INIFILE_H

#include <vector>
#include <string>
#include "../NFPublic/Debug.h"
#include "../NFCore/NFDataList.hpp"

// CIniFile 
class CIniFile
{
private:
	struct ITEM
	{
		std::string strName;
		int nHash;
		std::string strValue;
	};

	struct SECTION
	{
		std::string	strName;
		int	nHash;
		std::vector<ITEM> items;
	};

public:
	CIniFile();
	CIniFile(const char* filename);
	~CIniFile();
	
	// 释放
	void Release();
	// 设置文件名
	void SetFileName(const char* filename);
	// 获得文件名
	const char* GetFileName() const;
	// 加载文件
	bool LoadFromFile();
	// 保存文件
	bool SaveToFile() const;
	// 测试是否加载成功
	bool Loaded() const;
	// 添加段
	bool AddSection(const char* section);
	// 查找段
	int FindSection(const char* section) const;
	// 查找段下的键
	bool FindItem(const char* section, const char* key) const;
	// 获得键值
	bool FindValue(const char* section, const char* key, std::string& value) const;
	// 删除键
	bool DeleteItem(const char* section, const char* key);
	// 获得所有键的数量
	size_t GetItemCount() const;
	// 获得键的数据
	bool GetItem(size_t index, const char*& section, const char*& key, const char*& value) const;
	// 获得段的数量
	size_t GetSectCount() const;
	// 获得段名称
	const char* GetSectName(size_t index) const;
	// 获得指定段下的键数量
	size_t GetSectItemCount(size_t index) const;
	// 获得指定段下的key值
	std::string& GetSectionDataKey(size_t sect, size_t index);
	// 获得指定段下的value值
	std::string& GetSectionDataValue(size_t sect, size_t index);
	// 获得指定段下的键数据
	bool GetSectItemData(size_t sect, size_t index, const char*& key, const char*& value) const;

	// 读取键数据，当键不存在的时候返回缺省值(def)
	int ReadInteger(const char* section, const char* key, int def = 0) const;
	std::string ReadString(const char* section, const char* key, const char* def = "") const;
	bool ReadBool(const char* section, const char* key, bool def = false) const;
	float ReadFloat(const char* section, const char* key, float def = 0.0f) const;

	// 改写键数据（如果段或键不存在则添加）
	bool WriteInteger(const char* section, const char* key, int value);
	bool WriteString(const char* section, const char* key, const char* value);
	bool WriteBool(const char* section, const char* key, bool value);
	bool WriteFloat(const char* section, const char* key, float value);

	// 合并另外一个文件
	void Append(const CIniFile& other);

private:
	bool SetData(const char* section, const char* key, const char* data);
	const char* GetData(const char* section, const char* key) const;
	const CIniFile::SECTION* GetSection(const char* name) const;
	const CIniFile::ITEM* GetItem(const SECTION* pSection, const char* name) const;

private:
	std::vector<SECTION> m_Sections;
	std::string m_strFileName;
	bool m_bLoad;
};

#endif // _INIFILE_H
