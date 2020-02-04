///--------------------------------------------------------------------
/// �ļ���:		PlayerData.cpp
/// ��  ��:		������������߼�������
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "PlayerData.h"

//��ʼ��
void PlayerData::Init(const NFGUID& uid)
{
	mGUID = uid;
}

const NFGUID& PlayerData::GetGUID()
{
	return mGUID;
}

//���ڷ���ID
const std::set<int>& PlayerData::GetRoomIdSet()
{
	return mHaveRoomIdSet;
}

//�����·���ID
void PlayerData::EnterRoom(int nRooId)
{
	mHaveRoomIdSet.insert(nRooId);
}

//ɾ��һ�����뷿��ID��¼
void PlayerData::LeaveRoom(int nRooId)
{
	mHaveRoomIdSet.erase(nRooId);
}

//�Ƿ�����ָ���ķ���
bool PlayerData::IsMemberByRoom(int nRooId)
{
	if (mHaveRoomIdSet.end() == mHaveRoomIdSet.find(nRooId))
	{
		return false;
	}

	return true;
}

//��������ID
void PlayerData::SetSockIndex(NFSOCK nSockIndex)
{
	mSockIndex = nSockIndex;
}

//ȡ������ID
NFSOCK PlayerData::GetSockIndex()
{
	return mSockIndex;
}

//����
void PlayerData::Rese()
{
	mSockIndex = -1;
	mGUID = NFGUID();
	mHaveRoomIdSet.clear();
	Banned = false;
	Info = "";
}