///--------------------------------------------------------------------
/// 文件名:		GameKernel.h
/// 内  容:		GameServer核心模块
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_GAMEKERNEL_H__
#define __H_GAMEKERNEL_H__

#include "NFComm/NFPluginModule/IGameKernel.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "NFComm/NFPluginModule/NFILetterModule.h"
#include "GameServer.h"
#include "PlayerRenameModule.h"
#include "PubDataModule.h"
#include "NFComm/NFPluginModule/ICreateRoleModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFCore/NFPlayer.h"

class CGameKernel : public IGameKernel
{
public:
	typedef std::unordered_map<NFGUID, NF_SHARE_PTR<NFPlayer>> HashPlayer;
public:
	CGameKernel(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual ~CGameKernel() {};

	//初始化
	virtual bool Init();

	//初始化后
	virtual bool AfterInit();

	virtual NFIKernelModule* CKernel();

	//发COMMADND命令给对象
	virtual bool Command(int nCommandId, const NFGUID& self, const NFGUID& target, const NFDataList& msg);

	//发COMMADND命令给所有玩家对象
	virtual bool CommandByWorld(int nCommandId, const NFGUID& self, const NFDataList& msg);

	/// \breif   AddCustomCallBack 添加客户端消息回调
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);

	/// \breif   Custom 发消息给对象
	virtual bool Custom(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg);
	virtual bool Custom(int nMsgId, const NFGUID& self, const std::string& msg);

	/// \breif   CustomByWorld 发消息给所有玩家对象
	virtual bool CustomByWorld(int nMsgId, const google::protobuf::Message& msg);

	///////////////////////////////////////////////////////////////////信件相关 BEGIN////////////////////////////////////////////////////////////////////
	/// \brief 发送信件
	/// \param sender 发送者(玩家有效)
	/// \param sendName 发送名
	/// \param receiver 接收者
	/// \param strTitle 标题
	/// \param strContent 内容
	/// \param params 内容参数
	/// \param strAppend 附件
	virtual bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend);

	/// \brief 查看信件数量
	/// \param self 对象
	virtual bool QueryLetter(const NFGUID& self);

	/// \brief 查看信件信息
	/// \param self 对象
	/// \param num 数量
	virtual bool LookLetter(const NFGUID& self, const int num);

	/// \brief 删除信件
	/// \param self 对象
	/// \param strLetterID 信件id(为空删除全部)
	virtual bool DeleteLetter(const NFGUID& self, std::string strLetterID);
	///////////////////////////////////////////////////////////////////信件相关 END////////////////////////////////////////////////////////////////////

	// 获得公共GUID
	virtual const NFGUID& GetPubGuid(const std::string& name);
	// 获得公会GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name);
	// Message事件给公共域
	virtual bool CommandToPub(int nCommandId, const NFGUID& sendobj, const NFDataList& msg);
	// Message事件给公共域Guid
	virtual bool CommandToGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg);

	//写入数据库日志
	virtual void LogToDB(const std::string& recName, const NFDataList& col);

	// 编辑玩家数据
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp);

	//获取名字/Guid
	virtual const std::string& SeekRoleName(const NFGUID& self) const;
	virtual const NFGUID& SeekRoleGuid(const std::string& name) const;
	// 获取所有玩家Guid
	virtual const std::unordered_map<NFGUID, std::string>& GetRoleGuids() const;
	// 获取所有玩家名称
	virtual const std::unordered_map<std::string, NFGUID>& GetRoleNames() const;

	// 获取玩家
	virtual NF_SHARE_PTR<NFPlayer> GetPlayer(const NFGUID& player) const;
	// 获取所有玩家
	virtual const HashPlayer& GetPlayerAll() const;
	// 添加我那家
	virtual bool AddPlayer(NF_SHARE_PTR<NFPlayer> pPlayer);
	// 删除玩家
	virtual bool RemovePlayer(const NFGUID& player);
	// 封停角色
	virtual bool ClosurePlayer(const NFGUID& player, int64_t dnOpenTime);

protected:
	// 通用接收消息
	void OnRecvMsgFunction(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	/// \breif   PlayerRename 玩家改名
	virtual bool PlayerRename(const NFGUID& self, const std::string& strName);

	// 加载GUID Name
	void OnLoadPlayerGuidName(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 网络初始化成功
	int OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	int OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	int OnPlayerDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

private:

	// 名称映射GUID
	std::unordered_map<std::string, NFGUID> m_mapNameGuid;
	// GUID映射名称
	std::unordered_map<NFGUID, std::string> m_mapGuidName;

	// 玩家集合
	HashPlayer m_hashPlayer;

	//gameServer 连接世界模块
	CGameServer* m_pGameServer = nullptr;
	//通用内核模块
	NFIKernelModule* m_pKernelModule = nullptr;
	//引擎类型描述管理
	NFIClassModule* m_pClassModule = nullptr;
	NFILetterModule* m_pLetterModule = nullptr;
	//玩家改名
	PlayerRenameModule* m_pPlayerRenameModule = nullptr;
	// pub缓存
	CPubDataModule* m_pPubDataModule = nullptr;

	NF_SHARE_PTR<NFIClass> pPlayerClass = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	ICreateRoleModule* m_pCreateRoleModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif //__H_KernelGameServer_H__