///--------------------------------------------------------------------
// �ļ���:		IniFile.h 
// ��  ��:		��ʼ���ļ�������
/// ˵  ��:		
/// ��������:	2019��10��24��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
	
	// �ͷ�
	void Release();
	// �����ļ���
	void SetFileName(const char* filename);
	// ����ļ���
	const char* GetFileName() const;
	// �����ļ�
	bool LoadFromFile();
	// �����ļ�
	bool SaveToFile() const;
	// �����Ƿ���سɹ�
	bool Loaded() const;
	// ��Ӷ�
	bool AddSection(const char* section);
	// ���Ҷ�
	int FindSection(const char* section) const;
	// ���Ҷ��µļ�
	bool FindItem(const char* section, const char* key) const;
	// ��ü�ֵ
	bool FindValue(const char* section, const char* key, std::string& value) const;
	// ɾ����
	bool DeleteItem(const char* section, const char* key);
	// ������м�������
	size_t GetItemCount() const;
	// ��ü�������
	bool GetItem(size_t index, const char*& section, const char*& key, const char*& value) const;
	// ��öε�����
	size_t GetSectCount() const;
	// ��ö�����
	const char* GetSectName(size_t index) const;
	// ���ָ�����µļ�����
	size_t GetSectItemCount(size_t index) const;
	// ���ָ�����µ�keyֵ
	std::string& GetSectionDataKey(size_t sect, size_t index);
	// ���ָ�����µ�valueֵ
	std::string& GetSectionDataValue(size_t sect, size_t index);
	// ���ָ�����µļ�����
	bool GetSectItemData(size_t sect, size_t index, const char*& key, const char*& value) const;

	// ��ȡ�����ݣ����������ڵ�ʱ�򷵻�ȱʡֵ(def)
	int ReadInteger(const char* section, const char* key, int def = 0) const;
	std::string ReadString(const char* section, const char* key, const char* def = "") const;
	bool ReadBool(const char* section, const char* key, bool def = false) const;
	float ReadFloat(const char* section, const char* key, float def = 0.0f) const;

	// ��д�����ݣ�����λ������������ӣ�
	bool WriteInteger(const char* section, const char* key, int value);
	bool WriteString(const char* section, const char* key, const char* value);
	bool WriteBool(const char* section, const char* key, bool value);
	bool WriteFloat(const char* section, const char* key, float value);

	// �ϲ�����һ���ļ�
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
