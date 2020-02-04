///--------------------------------------------------------------------
/// 文件名:		NetObj.h
/// 内  容:		网络链接对象
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetObj_H__
#define __H_NetObj_H__
#include <WinSock2.h>
#include <string>
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NetCallback.h"

#pragma comment(lib,"ws2_32.lib")

#pragma pack(push, 1)

class NetServer;
class NetObj
{
public:
	NetObj(NetServer* pNet, NFSOCK fd, sockaddr_in& addr, void* pBev, size_t maxSingleReadSize)
	{
		m_pNet = pNet;
		nFD = fd;
		memset(&sin, 0, sizeof(sin));
		sin = addr;
		m_pUserData = pBev;
		m_pPackData = (char*)malloc(maxSingleReadSize);

		static atomic_int64_t nSockIndexCount = 0;
		++nSockIndexCount;
		if (nSockIndexCount == INT64_MAX)
		{
			nSockIndexCount = 1;
		}
		nSockIndex = nSockIndexCount;

		mLogInTime = time(nullptr);
	}

	~NetObj()
	{
		free(m_pPackData);
	}

	void* GetUserData()
	{
		return m_pUserData;
	}

	char* GetPackData()
	{
		return m_pPackData;
	}

	NetServer* GetNet()
	{
		return m_pNet;
	}

	int64_t GetSockIndex()
	{
		return nSockIndex;
	}

	const char* GetIP()
	{
		char* sz = inet_ntoa(sin.sin_addr);
		if (nullptr == sz)
		{
			return "";
		}

		return sz;
	}

	int GetPort()
	{
		return ntohs(sin.sin_port);
	}

	time_t GetLogInTime() const
	{
		return mLogInTime;
	}

	const std::string& GetSecurityKey() const
	{
		return mstrSecurityKey;
	}

	void SetSecurityKey(const std::string& strKey)
	{
		mstrSecurityKey = strKey;
	}

	int GetConnectKeyState() const
	{
		return mLogicState;
	}

	void SetConnectKeyState(const int nState)
	{
		mLogicState = nState;
	}

	int GetGameServerAppID() const
	{
		return mGameServerAppID;
	}

	void SetGameServerAppID(const int nData)
	{
		mGameServerAppID = nData;
	}

	const std::string& GetAccount() const
	{
		return mstrAccount;
	}

	void SetAccount(const std::string& strData)
	{
		mstrAccount = strData;
	}

	const NFGUID& GetClientID()
	{
		return mnClientID;
	}

	void SetClientID(const NFGUID& xClientID)
	{
		mnClientID = xClientID;
	}

	const NFGUID& GetPlayerID()
	{
		return mPlayerID;
	}

	void SetPlayerID(const NFGUID& nUserID)
	{
		mPlayerID = nUserID;
	}

	bool GetWaitDelete()
	{
		return bWaitDelete;
	}

	bool SetWaitDelete(bool v)
	{
		bWaitDelete = v;
		return true;
	}

	void SetLocalClient(const int64_t localClientSock, NetCallback* pLocalClientCallback)
	{
		mLocalClientSock = localClientSock;
		mpLocalClientCallback = pLocalClientCallback;
	}

	bool IsLocalClient()
	{
		return nullptr != mpLocalClientCallback;
	}

	bool SendLocalClient(const int16_t nMsgID, const char* msg, const size_t nLen)
	{
		if (nullptr == mpLocalClientCallback)
		{
			return false;
		}

		mpLocalClientCallback->RunMsgCB(mLocalClientSock, nMsgID, msg, (uint32_t)nLen);
		return true;
	}

private:
	sockaddr_in sin;
	void* m_pUserData = nullptr;
	char* m_pPackData = nullptr;
	NetServer* m_pNet = nullptr;
	NFSOCK nFD = -1;
	int64_t nSockIndex = -1;
	bool bWaitDelete = false;

	time_t mLogInTime = 0;
	std::string mstrSecurityKey;
	int32_t mLogicState = 0;
	int32_t mGameServerAppID = 0;
	std::string mstrAccount;
	NFGUID mPlayerID;//player id
	NFGUID mnClientID;//temporary client id

	//本地服务器直接回调
	NetCallback* mpLocalClientCallback = nullptr;
	int64_t mLocalClientSock = 0;
};

#pragma pack(pop)

#endif //__H_NetObj_H__