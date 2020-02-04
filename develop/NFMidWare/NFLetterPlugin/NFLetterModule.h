///---------------------------------------------------------
/// 文件名: NFLetterModule.h
/// 内 容:  
/// 说 明:       
/// 创建日期: 2019/08/26
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------
#ifndef NF_LETTER_MODULE_H__
#define NF_LETTER_MODULE_H__

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILetterModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class NFLetterModule : public NFILetterModule
{
public:
	NFLetterModule(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();

	virtual bool AfterInit();

	/// \brief 查看信件数量
	void OnAckQueryLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	/// \brief 查看信件回复
	void OnAckLookLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	/// \brief 发送信件
	bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend);

	/// \brief 查看信件数量
	bool QueryLetter(const NFGUID& self);

	/// \brief 查看信件(num=0全部取出来)
	bool LookLetter(const NFGUID& self, const int num);

	/// \brief 删除信件
	bool DeleteLetter(const NFGUID& self, std::string strLetterID);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;

};

#endif // NF_LETTER_MODULE_H__
