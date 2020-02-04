///--------------------------------------------------------------------
/// �ļ���:		PlayerData.h
/// ��  ��:		������������߼�������
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_PlayerData_H__
#define __H_PlayerData_H__

#include "NFComm\NFPluginModule\NFGUID.h"
#include <set>

class PlayerData
{
public:
	//��ʼ��
	void Init(const NFGUID& uid);

	const NFGUID& GetGUID();

	//���ڷ���ID
	const std::set<int>& GetRoomIdSet();

	//���뷿��ID��¼
	void EnterRoom(int nRooId);

	//ɾ��һ�����뷿��ID��¼
	void LeaveRoom(int nRooId);

	//�Ƿ�����ָ���ķ���
	bool IsMemberByRoom(int nRooId);

	//��������ID
	void SetSockIndex(NFSOCK nSockIndex);
	//ȡ������ID
	NFSOCK GetSockIndex();

	//����
	void Rese();

private:
	NFGUID mGUID;
	//���ڷ���ID
	std::set<int> mHaveRoomIdSet;
	//��������ID
	NFSOCK mSockIndex = -1;
public:

	//����״̬
	bool Banned = false;
	//�Զ�������
	std::string Info;
};

#endif	//__H_PlayerData_H__