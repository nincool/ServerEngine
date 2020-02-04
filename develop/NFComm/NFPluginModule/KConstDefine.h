///--------------------------------------------------------------------
/// 文件名:		KConstDefine.h
/// 内  容:		核心定义
/// 说  明:		引擎内部和逻辑层共用的一些定义
/// 创建日期:	2019年8月22日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __KERNEL_CONST_DEFINE_H__
#define __KERNEL_CONST_DEFINE_H__

#include "NFIModule.h"
#include "Dependencies/protobuf/src/google/protobuf/message.h"
#include "NFComm/NFUtils/QLOG.h"

// 事件定义,没有实现的先注释掉
enum EventType
{
	EVENT_OnDefault = 0,	// 响应所有事件, 参数占用前两位提供注册信息,args{EventType事件类型, string 响应逻辑类}
	EVENT_OnNoSqlComplete,	// NoSql连接完成
	EVENT_OnNoSqlDisConnect,// NoSql断开
	EVENT_OnMySqlComplete,	// MySql连接完成
	EVENT_OnMySqlDisConnect,// MySql断开
	EVENT_OnAllNetComplete, // 所有网络已经完成
	EVENT_OnServerComplete,	// 当前服务启动完成
	EVENT_OnAllServerComplete,// 所有服务启动完成
	EVENT_OnServerStateChange,	//服务器状态改变 args{逻辑状态，id, appid, type, 网络状态，是否是新连接, SockIndex, 服务连接方式(0为NetServerInsideModule)}
 	EVENT_OnCreateClass,	// 逻辑类创建 args{string 逻辑类名}
	EVENT_OnCreateRole,		// 角色创建 self 角色 args{string 自定义数据}
	EVENT_OnCreate,			// 对象创建 self 对象
	EVENT_OnDestroy,		// 对象销毁 self 对象
	EVENT_OnEntry,			// 进入容器 self 对象 sender 容器 args{int 位置(从0开始)}
	EVENT_OnLeave,			// 离开容器 self 对象 sender 容器
	EVENT_OnNoAdd,			// 测试容器是否允许加入对象(返回非0表示不允许) self 容器 sender 对象 args{int 位置(从0开始 负数表示任意位置)}
	EVENT_OnAdd,			// 容器加入对象前 self 容器 sender 对象 args{int 位置(从0开始)}
	EVENT_OnAfterAdd,		// 容器加入对象后 self 容器 sender 对象 args{int 位置(从0开始)}
	EVENT_OnNoRemove,		// 测试容器移除对象(返回非0表示不允许) self 容器 sender 对象
	EVENT_OnBeforeRemove,	// 容器移除对象前 self 容器 sender 对象 args{int 位置(从0开始)}
	EVENT_OnRemove,			// 容器移除对象后 self 容器 sender 对象 args{int 位置(从0开始)}
	EVENT_OnChange,			// 容器对象改变位置 self 容器 sender 对象 args{int 旧位置(从0开始), int 新位置(从0开始)}
	EVENT_OnLoad,			// 对象加载配置文件完成 self 对象
	EVENT_OnRecover,		// 对象从数据库恢复数据完成 self 对象
	EVENT_OnComplete,		// 对象创建完成(所有数据全部加载完成) self 对象
	EVENT_OnPubComplete,	// 公共域数据全部加载完成
	EVENT_OnGuildComplete,	// 公会数据全部加载完成
	EVENT_OnEntryGame,		// 玩家进入游戏
	EVENT_OnReady,			// 客户端就绪 self 角色
	//EVENT_OnContinue,		// 客户端断线重连 self 角色
	EVENT_OnDisconnect,		// 客户端断线 self 角色
	EVENT_OnStore,			// 存库前 self 对象 args{int 类型(StoreState)}
	//EVENT_OnSendLetter,		// 发送信件完成 self 发件者 sender 收件者 args{int 结果(1成功)}
	EVENT_OnQueryLetter,	// 查询信件完成 self 查件者 args{int 收取的信件数量}
	//EVENT_OnRecvLetter,		// 收取信件(数据库删除) self 接收者 args{string 发件者ID, string 发件者名称, int64 信件发送时间, int 信件类型, string 信件内容, string 信件附件, string 信件流水号}
	//EVENT_OnRecvLetterFail,	// 收取信件失败 self 接收者 args{string 信件流水号}
	EVENT_OnLookLetter,		// 查看信件(数据库不删除)  self 接收者 args{int 信件数量 string 信件流水号，guid 发件者ID, string 发件者名称, string 信件发送时间, string 信件标题, string 信件内容, string 信件附件}
	//EVENT_OnServerClose,	// 服务器关闭
	EVENT_OnChangeName,		// 改名 self 角色 args{int 结果(1成功), string 旧名字, string 新名字}
	EVENT_CALLBACK_MAX,
};

typedef int (NFIModule::* EVENT_HOOK)(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
// 宏替换注册回调的 对象指针, 函数指针, 优先级
#define METHOD_ARGS(hook, ...) this, (EVENT_HOOK)&hook, __VA_ARGS__ 

typedef std::function<int(const NFGUID&, const char*, const int, const int)> OBJECT_HEARTBEAT_FUNCTOR;
typedef NF_SHARE_PTR<OBJECT_HEARTBEAT_FUNCTOR> OBJECT_HEARTBEAT_FUNCTOR_PTR;//HEART

typedef void(NFIModule::* NetMsgFun)(const NFSOCK nSockIndex, const uint16_t nMsgID, const char* msg, const uint32_t nLen);

#pragma region
class IMethodMsg
{
public:
	virtual ~IMethodMsg() {};
	virtual int Invoke(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg) = 0;
	virtual google::protobuf::Message& GetMsg() = 0;

	// 获取优先级
	virtual int GetPrior()
	{
		return m_nPrior;
	}

	// 获取对象
	virtual NFIModule* GetObj()
	{
		return m_pObj;
	}

protected:
	NFIModule* m_pObj = nullptr;
	int m_nPrior = 0;
};

template<class __C, class __P>
class MethodMsg : public IMethodMsg
{
private:
	typedef int(__C::* fptr)(int nMsgId, const NFGUID& self, const __P& msg);
	fptr f = nullptr;
public:
	MethodMsg(__C* obj, fptr f, int nPrior)
	{
		this->f = f;
		this->m_pObj = obj;
		this->m_nPrior = nPrior;
	}

	virtual int Invoke(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg)
	{
		return ((__C*)(this->m_pObj)->*f)(nMsgId, self, (__P&)msg);
	}

	virtual google::protobuf::Message& GetMsg()
	{
		static __P msg;
		return (google::protobuf::Message&)msg;
	}
};

template<class __C, class __P>
inline NF_SHARE_PTR<IMethodMsg> RTTIMsgFuncOf(__C* obj, int (__C::*f)(int nMsgId, const NFGUID& self, const __P& msg), int nPrior = 0)
{
	assert(obj != nullptr);
	NF_BASE_OF(google::protobuf::Message, __P);
	return NF_MAKE_SPTR<MethodMsg<__C, __P>>(obj, f, nPrior);
}
#pragma endregion
// 使用PB结构为参数的回调，回调中需要根据回调定义的PB结构创建对象来解析数据 
// 回调定义 int FunctionName(const NFGUID& self, const NFGUID& sender, const PBMsg& msg)
// FunctionName 为自定义函数 PBMsg为自定义结构
// 此宏定义来生成 NF_SHARE_PTR<IMethodMsg> 传递给注册函数 参数1 函数 参数2 优先级 (默认为0)
#define METHOD_MSG(func, ...) RTTIMsgFuncOf(this, &func, __VA_ARGS__)

// 只允许使用第三方库的代码使用此宏
// 不要包含任何其他函数
#define CATCH_BEGIN try {
#define CATCH_END \
} catch (std::exception & ia) \
{ \
	QLOG_ERROR << "exception " << ia.what(); \
	assert(0); \
}

enum StoreState
{
	STORE_EXIT,		// 玩家离开游戏
	STORE_TIMING,	// 定时保存
	STORE_MANUAL,	// 人工保存（调用SavePlayerData）
};

//服务器类型
enum NF_SERVER_TYPES
{
	NF_ST_NONE = 0,			// NONE
	NF_ST_REDIS = 1,		//REDIS服务器
	NF_ST_MYSQL = 2,		//MYSQL服务器
	NF_ST_MASTER = 3,		//主服务器
	NF_ST_LOGIN = 4,		//登录服务器
	NF_ST_PROXY = 5,		//代理接入服务器
	NF_ST_GAME = 6,			//场景服务器
	NF_ST_WORLD = 7,		//世界服务器
	NF_ST_DB = 8,			//数据存取服务器
	NF_ST_PUB = 9,			//公共服务器
	NF_ST_CHAT = 10,		//聊天服务器
	NF_ST_RANK = 11,		//排行服务器
	NF_ST_PLATFORM = 12,	//平台服务器
	NF_ST_ROOM = 13,		//战场服务器
	NF_ST_RANK_DB = 14,		//排行榜DB
	NF_ST_MAX,				//类型最大值
};

static std::map<std::string, NF_SERVER_TYPES> g_ServerMap =
{
	{"MasterServer", NF_ST_MASTER},
	{"LoginServer", NF_ST_LOGIN},
	{"ProxyServer", NF_ST_PROXY},
	{"GameServer", NF_ST_GAME},
	{"WorldServer", NF_ST_WORLD},
	{"DBServer", NF_ST_DB},
	{"PubServer", NF_ST_PUB},
	{"ChatServer", NF_ST_CHAT},
	{"RankServer", NF_ST_RANK},
	{"PlatformServer", NF_ST_PLATFORM},
	{"RoomServer", NF_ST_ROOM},
	{"RankDBServer", NF_ST_RANK_DB}
};

typedef std::function<void(const std::string& strPath, const google::protobuf::Message* pRes)> ResReloadFunction;
// Res Map形式的数据
// <属性名, 属性值>
typedef std::map<std::string, NFData> MapRowData;
// <ID, 整行数据>
typedef std::map<std::string, MapRowData> ResMapStrData;
typedef std::map<int, MapRowData> ResMapIntData;

// 排行榜排序类型
enum RankOrderType
{
	RANK_ORDER_DESC = 0, // 降序
	RANK_ORDER_ASCE = 1, // 升序
};

// 玩家在线状态
enum OnlineState
{
	ONLINE_TRUE = 0,	// 在线
	ONLINE_FALSE,		// 离线 (上线事件全部触发过)

	// 编辑状态(没有触发任何事件) 
	// 有且只有 AlterPlayer回调可能返回此状态的玩家
	// 此状态的玩家通过GetObject接口获取不到
	ONLINE_ALTER,
};

#endif // __KERNEL_CONST_DEFINE_H__

