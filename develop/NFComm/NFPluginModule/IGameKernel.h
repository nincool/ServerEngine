///--------------------------------------------------------------------
/// 文件名:		IKernelGameServer.h
/// 内  容:		GameServer核心模块
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __NFI_GAMEKERNEL__
#define __NFI_GAMEKERNEL__

#include "NFIKernelModule.h"

#define IGAME_LOGIC_VERSION 0x0001

class IGameKernel : public NFIModule
{
public:
	virtual ~IGameKernel() {};

	// 获取共用核心
	virtual NFIKernelModule* CKernel() = 0;

	/// \breif   Command 发内部消息给对象
	/// \Access  virtual public 
	/// \param   int nMsgId 消息号
	/// \param   const NFGUID & self 发送者
	/// \param   const NFGUID & target 接收者
	/// \param   const NFDataList & msg 参数列表
	/// \return  bool 是否成功
	virtual bool Command(int nMsgId, const NFGUID& self, const NFGUID& target, const NFDataList& msg) = 0;

	/// \breif   CommandByWorld 发内部消息给所有玩家对象
	/// \Access  virtual public 
	/// \param   int nMsgId 消息号
	/// \param   const NFGUID & self 发送者
	/// \param   const NFDataList & msg 消息列表
	/// \return  bool 是否成功
	virtual bool CommandByWorld(int nMsgId, const NFGUID& self, const NFDataList& msg) = 0;

	/// \breif   AddCustomCallBack 添加客户端消息回调
	/// \Access  virtual public 
	/// \param   int nMsgId 消息号
	/// \param   NF_SHARE_PTR pMethod 消息回调包装类对象智能指针 通过宏 METHOD_MSG 生成
	/// \return  bool
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;

	/// \breif   Custom 发消息给对象
	/// \Access  virtual public 
	/// \param   int nMsgId 消息号
	/// \param   const NFGUID & self 发送者
	/// \param   const google::protobuf::Message& msg 消息PB结构
	/// \return  bool 是否成功
	virtual bool Custom(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg) = 0;
	virtual bool Custom(int nMsgId, const NFGUID& self, const std::string& msg) = 0;

	/// \breif   CustomByWorld 发消息给所有玩家对象
	/// \Access  virtual public 
	/// \param   int nMsgId 消息号
	/// \param   const google::protobuf::Message& msg 消息PB结构
	/// \return  bool 是否成功
	virtual bool CustomByWorld(int nMsgId, const google::protobuf::Message& msg) = 0;

	/// \breif   PlayerRename 玩家改名
	/// \Access  virtual public 
	/// \param   const NFGUID& self 玩家
	/// \param   const std::string& strName 名字
	/// \return  bool 是否成功
	virtual bool PlayerRename(const NFGUID& self, const std::string& strName) = 0;

	///////////////////////////////////////////////////////////////////信件相关 BEGIN//////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////信件相关 END//////////////////////////////////////////////////////////////////////

	/// \breif   GetPubGuid 获得公共域GUID
	/// \Access  virtual public 
	/// \param   const std::string& name 名字
	/// \return  NFGUID& 对应GUID
	virtual const NFGUID& GetPubGuid(const std::string& name) = 0;

	/// \breif   GetGuildGuid 获得公会GUID
	/// \Access  virtual public 
	/// \param   const std::string& name 名字
	/// \return  NFGUID& 对应GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name) = 0;

	/// \breif   CommandToPub 发消息给PUB
	/// \Access  virtual public 
	/// \param   int nCommandId 消息号
	/// \param   const NFGUID& sendobj 发送对象
	/// \param   const NFDataList& msg 数据信息
	/// \return  bool 是否成功
	virtual bool CommandToPub(int nCommandId, const NFGUID& sendobj, const NFDataList& msg) = 0;

	/// \breif   CommandToGuid 发消息给公共域Guid
	/// \Access  virtual public 
	/// \param   int nCommandId 消息号
	/// \param   const NFGUID& sendobj 发送对象
	/// \param   const NFGUID& targetobj 接受对象
	/// \param   const NFDataList& msg 数据信息
	/// \return  bool 是否成功
	virtual bool CommandToGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg) = 0;

	//写入数据库日志
	virtual void LogToDB(const std::string& recName, const NFDataList& col) = 0;

	/// \breif   AlterPlayer 编辑玩家数据,如果玩家存在则直接回调消息，并且刷新存在时间,
	/// \breif   所以使用此功能最好不要判断玩家是否能够获取到,直接调用
	/// \Access  virtual public 
	/// \param   const NFGUID & player 需要编辑的玩家
	/// \param   int nMsgID 回调Command消息
	/// \param   const NFGUID & sender 
	/// \param   const NFDataList & args 附带数据 直接传递给回调消息
	/// \return  bool
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp) = 0;

	/// \brief 获取角色名字
	/// \Access  virtual public 
	/// \param   const NFGUID & self 角色对象
	/// \return  const std::string&
	virtual const std::string& SeekRoleName(const NFGUID& self) const = 0;

	/// \brief 获取角色Guid
	/// \Access  virtual public 
	/// \param   const std::string& name 角色名字
	/// \return  const NFGUID
	virtual const NFGUID& SeekRoleGuid(const std::string& name) const = 0;

	// 获取所有玩家Guid
	virtual const std::unordered_map<NFGUID, std::string>& GetRoleGuids() const = 0;
	// 获取所有玩家名称
	virtual const std::unordered_map<std::string, NFGUID>& GetRoleNames() const = 0;

	/// \breif   ClosurePlayer 封停角色
	/// \Access  virtual public 
	/// \param   const NFGUID & player 角色id
	/// \param   int64_t dnOpenTime 允许登录时间(封停截止时间戳)
	/// \return  bool
	virtual bool ClosurePlayer(const NFGUID& player, int64_t dnOpenTime) = 0;

};

#endif //__NFI_GAMEKERNEL__