///---------------------------------------------------------
/// 文件名: NFILetterModule.h
/// 内 容:  信件接口
/// 说 明:       
/// 创建日期: 2019/08/30
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------

#ifndef NFI_LETTER_MODULE_H
#define NFI_LETTER_MODULE_H

#include <string>
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFList.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "KConstDefine.h"

class NFILetterModule : public  NFIModule
{
public:
	virtual ~NFILetterModule() {}
	/// \brief 发送信件
	/// \param sender 发送者(玩家有效)
	/// \param sendName 发送名
	/// \param receiver 接收者
	/// \param strTitle 标题
	/// \param strContent 内容
	/// \param params 内容参数
	/// \param strAppend 附件
	virtual bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend) = 0;

	/// \brief 查看信件数量
	/// \param self 对象
	virtual bool QueryLetter(const NFGUID& self) = 0;

	/// \brief 查看信件信息
	/// \param self 对象
	/// \param num 数量
	virtual bool LookLetter(const NFGUID& self, const int num) = 0;

	/// \brief 删除信件
	/// \param self 对象
	/// \param strLetterID 信件id(为空删除全部)
	virtual bool DeleteLetter(const NFGUID& self, std::string strLetterID) = 0;
};

#endif //NFI_LETTER_MODULE_H
