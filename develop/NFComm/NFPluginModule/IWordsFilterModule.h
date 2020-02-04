///--------------------------------------------------------------------
/// �ļ���:		IWordsFilterModule.h
/// ��  ��:		���д�����
/// ˵  ��:		
/// ��������:	2019��9��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_IWordsFilterModule_H__
#define __H_IWordsFilterModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"

class IWordsFilterModule : public NFIModule
{
public:
	virtual ~IWordsFilterModule() {};


	//����һ�����д�
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord) = 0;

	//�����ַ��������е����д�
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out) = 0;

	//���ַ��������е����д��滻 �����´�
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*') = 0;

	//����ַ������Ƿ������д�
	virtual bool ExistSensitiveWord(const std::string& text) const = 0;

};

#endif //__H_IWordsFilterModule_H__