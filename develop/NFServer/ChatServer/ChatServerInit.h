///--------------------------------------------------------------------
/// �ļ���:		ChatServerInit.h
/// ��  ��:		�����������ʼ��
/// ˵  ��:		
/// ��������:	2019��11��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_ChatServerInit_H__
#define __H_ChatServerInit_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"

class NFMysqlModule;
class ChatServerInit : public NFIModule
{
public:
	ChatServerInit(NFIPluginManager* p);

	//���������ó�ʼ��
	virtual bool Init();

	// �ű���ʼ��
	virtual bool InitLua();

	// ���������ʼ��
	virtual bool InitNetServer();

	// �ͻ������ʼ��
	virtual bool InitNetClient();

	// ʹ�ü���
	void SetUseEncrypt(bool value) { m_bUseEncrypt = value; }
	bool GetUseEncrypt() const { return m_bUseEncrypt; }

	// Mysql����
	void SetMysqlConnect(std::string& value) { m_nMysqlConnect = value; }
	std::string& GetMysqlConnect() { return m_nMysqlConnect; }

	const std::string& GetDBName() { return m_strDBName; }

	//ÿ�����������¼�����������
	void SetRecordingMax(int value) { m_RecordingMax = value; }
	int GetRecordingMax() const { return m_RecordingMax; }

	//ÿ��������������Ϣ����
	void SetOfflineMsgMax(int value) { m_OfflineMsgMax = value;}
	int GetOfflineMsgMax() const { return m_OfflineMsgMax; }

	//ÿ�����������Ϣ��󱣴�ʱ��(��)
	void SetOfflineMsgCacheTimeMax(int value) { m_OfflineMsgCacheTimeMax = value; }
	int GetOfflineMsgCacheTimeMax() const { return m_OfflineMsgCacheTimeMax; }

	//����Ϣ���Ƶ��(��)
	void SetSendFrequencyMax(int value) { m_SendFrequencyMax = value; }
	int GetSendFrequencyMax() const { return m_SendFrequencyMax; }

	//û�ͻ��˵���󱣴�ʱ��
	void SetDelayDeletePlayerTime(int value) { mDelayDeletePlayerTime = value; }
	time_t GetDelayDeletePlayerTime() const { return mDelayDeletePlayerTime; }

	//ɾ����ʱ��û��ҵķ���
	void SetDelayDeleteRoomTime(int value) { mDelayDeleteRoomTime = value; }
	time_t GetDelayDeleteRoomTime() const { return mDelayDeleteRoomTime; }

	// ��������
	void CreateServer(const NetData& net_data);
	// ���������Ϣ
	void ConnectServer(const ConnectCfg& connect_data);

	//���³�����
	void UpReport(bool force = false);
private:
	// �������ݿ�����
	void InitDBConnect();

	// �������ݿ�����
	bool ParseDBConnect(std::string& DBConnect, std::string& strDBPassword,
		std::string& strDBUser, std::string& strDBIP, int& nDBPort);

private:
	//�Ƿ����
	bool m_bUseEncrypt = false;
	//Mysql����
	std::string m_nMysqlConnect = "";
	//���ݿ���
	std::string m_strDBName;
	//ÿ�����������¼�����������
	int m_RecordingMax = 500;
	//ÿ��������������Ϣ����
	int m_OfflineMsgMax = 60;
	//ÿ�����������Ϣ��󱣴�ʱ��(��)
	int m_OfflineMsgCacheTimeMax = 604800;
	//����Ϣ���Ƶ��(��)
	int m_SendFrequencyMax = 2;
	//û�ͻ��˵���󱣴�ʱ��
	time_t mDelayDeletePlayerTime = 180;
	//ɾ����ʱ��û��ҵķ���
	time_t mDelayDeleteRoomTime = 86400;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
};

#endif	//__H_ChatServerInit_H__