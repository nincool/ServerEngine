///--------------------------------------------------------------------
/// �ļ���:		NFResModule.h
/// ��  ��:		�����ļ�����
/// ˵  ��:		����proto��������ʽ�������ļ�
/// ��������:	2019��8��15��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_RES_MODULE_H__
#define __NF_RES_MODULE_H__

#include "NFComm/NFPluginModule/NFIResModule.h"
#include "../NFPluginModule/NFIElementModule.h"
#include "../NFPluginModule/KConstDefine.h"

class NFResModule : public NFIResModule
{
private:
	NFResModule() {};
public:
	NFResModule(NFIPluginManager* p);
	virtual ~NFResModule();

	virtual bool Init();
	virtual bool AfterInit();

	/// \breif   Load ���������ļ�
	virtual const google::protobuf::Message* Load(const char* szPath, ResReloadFunction pfReload = nullptr);

	/// \breif   Reload �����Ѿ����ع�������
	virtual bool Reload(const char* szPath);

	/// \breif   ReloadAll ���������Ѿ����ع�������
	virtual bool ReloadAll();

	/// \breif   ConvertMapData �� pb ����ת��Ϊmap
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName);
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName);

protected:

	/// \breif   Get ��ȡ�����ļ�
	virtual google::protobuf::Message* InnerGet(const char* szPath);

	/// \breif   Load ���������ļ�
	virtual bool InnerLoad(google::protobuf::Message* pRes, const char* szPath, bool bReload = false);
 
	/// \breif   InnerAddReloadHook ������ػص�
	virtual bool InnerAddReloadHook(const char* szPath, const google::protobuf::Message* pRes, ResReloadFunction pfReload);

	/// \breif   GetMapRowData ���һ������
	virtual bool GetMapRowData(const google::protobuf::Message& msg, const std::string& strKeyName, NFData& xKeyData, MapRowData& mapRow);
private:
	NFIElementModule* m_pElementModule = nullptr;

	typedef std::map <std::string, google::protobuf::Message*> MapRes;
	MapRes m_mapRes;

	typedef std::map <std::string, std::vector<ResReloadFunction>> MapHook;
	MapHook m_mapHook;
};

#endif // __NF_RES_MODULE_H__
