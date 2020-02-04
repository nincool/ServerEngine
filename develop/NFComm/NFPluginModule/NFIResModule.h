///--------------------------------------------------------------------
/// �ļ���:		NFIResModule.h
/// ��  ��:		�����ļ�����
/// ˵  ��:		����proto��������ʽ�������ļ�
/// ��������:	2019��8��15��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NFI_RES_MODULE_H__
#define __NFI_RES_MODULE_H__

#include "NFIModule.h"
#include "Dependencies/protobuf/src/google/protobuf/message.h"
#include "KConstDefine.h"

class NFIResModule : public NFIModule
{
public:
	virtual ~NFIResModule() {};

	/// \breif   Load ���������ļ�
	virtual const google::protobuf::Message* Load(const char* szPath, ResReloadFunction pfReload = nullptr) = 0;

	/// \breif   Reload �����Ѿ����ع�������
	virtual bool Reload(const char* szPath) = 0;

	/// \breif   ReloadAll ���������Ѿ����ع�������
	virtual bool ReloadAll() = 0;

	/// \breif   ConvertMapData �� pb ����ת��Ϊmap
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName) = 0;
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName) = 0;
};
#endif // __NFI_RES_MODULE_H__