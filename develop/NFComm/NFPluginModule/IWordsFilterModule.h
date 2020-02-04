///--------------------------------------------------------------------
/// 文件名:		IWordsFilterModule.h
/// 内  容:		敏感词屏蔽
/// 说  明:		
/// 创建日期:	2019年9月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_IWordsFilterModule_H__
#define __H_IWordsFilterModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"

class IWordsFilterModule : public NFIModule
{
public:
	virtual ~IWordsFilterModule() {};


	//增加一个敏感词
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord) = 0;

	//查找字符串里所有的敏感词
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out) = 0;

	//把字符串里所有的敏感词替换 返回新串
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*') = 0;

	//检测字符串里是否有敏感词
	virtual bool ExistSensitiveWord(const std::string& text) const = 0;

};

#endif //__H_IWordsFilterModule_H__