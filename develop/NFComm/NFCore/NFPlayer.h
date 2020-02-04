///--------------------------------------------------------------------
/// �ļ���:		NFPlayer.h
/// ��  ��:		���
/// ˵  ��:		
/// ��������:	2019��7��30��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_PLAYER_H__
#define __NF_PLAYER_H__

#include "NFVisible.h"
#include "NFViewport.h"

 class _NFExport NFPlayer : public NFVisible
 {
 public:
	 enum {VIEWPORT_MAX_NUM = 128};
 private:
	 NFPlayer() : NFVisible(NFGUID(), nullptr) {};

 public:
	 NFPlayer(NFGUID self, NFIPluginManager* pPluginManager);
	virtual ~NFPlayer();

	virtual bool IsPlayer() const { return true; }

	// ��������Ӵ�
	bool AddViewport(int id, NFVisible* pContainer);
	// ɾ�������Ӵ�
	bool RemoveViewport(int id);
	// ���������Ӵ�
	bool FindViewport(int id);
	// ɾ�����������Ӵ�
	void ClearViewport();
	// ��ȡ�����Ӵ���Ӧ������
	NFVisible* GetViewContainer(int id);
	// ���·������������Ӵ�
	void ResendViewports();

	// �����ͻ�����Ϣ
	int OnCustom(int nMsgId, const std::string& strMsg);

	// �ڲ�����
	RAW_INT(GameID); // ��Ϸ��ID
	RAW_INT(GateID); // ���ط�ID
	RAW_OBJECT(ClientID); // �ͻ�������ID
	RAW_STR(Account); // �˺�
	RAW_STR(IP); // ip
	RAW_INT(CreateTime); // ��ɫ����ʱ��
	RAW_INT(OpenTime); // �����¼ʱ��
	RAW_INT(TotalSec); // ������ʱ��
	RAW_INT(OnlineTime); // ����ʱ���
	RAW_INT(OfflineTime);   // ����ʱ���
	RAW_STR(LiteData); // ժҪ���� ��¼ʱ��ʾ
	RAW_STR(DeviceID); // �豸ID
	RAW_STR(DeviceMAC); // �豸MAC��ַ
	RAW_INT(OsType);   // �豸ϵͳ����
	RAW_INT(OnlineState);   // ����״̬

 private:
 	// �Ӵ�
	 std::vector<NF_SHARE_PTR<NFViewport>> m_vecViewport;
 };

#endif // __NF_PLAYER_H__