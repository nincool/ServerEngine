// -------------------------------------------------------------------------
//    @FileName	:	OuterMsgDefine.h
//    @Author  	:	EnumDefine Tool
// -------------------------------------------------------------------------

#ifndef __OUTERMSGDEFINE_H__
#define __OUTERMSGDEFINE_H__
namespace OuterMsg
{

enum ChatMsgID
{

	// 聊天服务器
	SS_CHAT_REMOVE_ROOM = 1,	// ChatRemoveRoom	移除一个聊天房间
	SS_CHAT_ADD_PLAYER_ROOM = 2,	// ChatAddPlayerToRoom	增加玩家到一个聊天房间(服务器内部用)
	SS_CHAT_REMOVE_PLAYER_ROOM = 3,	// ChatRemovePlayerToRoom	移除玩家到一个聊天房间(服务器内部用)
	SS_CHAT_PLAYER_INFO = 4,	// ChatPlayerInfo	更新玩家信息
	SS_CHAT_PLAYER_BANNED = 5,	// ChatPlayerBanned	设置玩家禁言
};


enum EngineMsgID
{

	// GOP游客绑定平台
	SS_REQ_ROLE_ADDRESS = 135,	// RoleAddress	请求角色所在区服信息
	SS_ACK_ROLE_ADDRESS = 136,	// RoleAddress	应答角色所在区服信息
	SS_REQ_UPDATE_ACCOUNT = 137,	// UpdateAccount	请求更新账号
	SS_ACK_UPDATE_ACCOUNT = 138,	// UpdateAccount	应答更新账号结果
	SS_REQ_OTHER_UPDATE_ACCOUNT = 139,	// OtherUpdateAccount	请求其他区更新账号
	SS_ACK_OTHER_UPDATE_ACCOUNT = 140,	// OtherUpdateAccount	应答其他区更新账号
	SS_GAME_SERVER_CONNECT = 141,	// ServerID string	游戏服连接Master服务器
	SS_CENTER_SERVER_CONNECT = 142,	// MsgEmpty	中心服务器连接Master服务器

	// GOP角色
	SS_REQ_GOP_ROLE_CREATE = 143,	// GOPRoleData	请求GOP角色创建
	SS_ACK_GOP_ROLE_CREATE = 144,	// GUID	应答GOP角色创建
	SS_REQ_GOP_ROLE_REMOVE = 145,	// GOPRoleData	请求GOP角色删除
	SS_ACK_GOP_ROLE_REMOVE = 146,	// GUID	应答GOP角色删除
	SS_REQ_GOP_ROLE_UPDATE = 147,	// GOPRoleData	请求GOP角色更新
	SS_ACK_GOP_ROLE_UPDATE = 148,	// GUID	应答GOP角色更新

	// LogToDB
	SS_LOG_TO_DB = 134,	// 	写入数据库的LOG

	// command
	SS_NET_COMMAND_TRANSMIT = 56,	// CommandMsg	command转发
	SS_COMMAND_TOGAME = 84,	// CommandMsg	ToPub的Command
	SS_COMMAND_TOPUB = 85,	// CommandMsg	ToGame的Command
	SS_COMMAND_TOPLATFORM = 112,	// CommandMsg	ToPlatform的Command
	SS_COMMAND_TOROOM = 113,	// CommandMsg	ToRoom的Command

	// letter
	SS_REQ_SEND_LETTER = 57,	// LetterDataPack	发送信件
	SS_REQ_LOOK_LETTER = 58,	// ReqLookLetter	查看信件
	SS_ACK_LOOK_LETTER = 59,	// LetterDataPack	回复查看信件
	SS_REQ_DELETE_LETTER = 60,	// ReqDeleteLetter	删除信件
	SS_REQ_QUERY_LETTER = 61,	// MsgEmpty	查看信件数量
	SS_ACK_QUERY_LETTER = 62,	// AckQueryLetter	回复信件数量

	// 公共服务
	SS_REQ_CREATE_PUB = 65,	// PubOpData	请求创建公共数据
	SS_ACK_CREATE_PUB = 66,	// AckResult	创建回复
	SS_REQ_CREATE_GUILD = 67,	// PubOpData	请求创建公会
	SS_ACK_CREATE_GUILD = 68,	// AckResult	创建回复
	SS_REQ_LOAD_PUB_DATA = 69,	// PubOpData	请求加载PUB数据
	SS_ACK_LOAD_PUB_DATA = 70,	// PubNameData	回复加载PUB数据
	SS_REQ_LOAD_GUILD_DATA = 71,	// PubOpData	请求加载GUILD数据
	SS_ACK_LOAD_GUILD_DATA = 72,	// PubNameData	回复加载GUILD数据
	SS_REQ_SAVE_PUB_DATA = 73,	// PubNameData	请求保存PUB数据
	SS_REQ_SAVE_GUILD_DATA = 74,	// ObjectDataPack	请求保存GUILD数据
	SS_REQ_START_LOAD_PUB = 75,	// MsgEmpty	请求公共域加载启动
	SS_ACK_START_LOAD_PUB = 76,	// PropertyList	回复公共域加载启动
	SS_REQ_START_LOAD_GUILD = 77,	// MsgEmpty	请求公会加载启动
	SS_ACK_START_LOAD_GUILD = 78,	// PropertyList	回复公会加载启动
	SS_ACK_LOAD_PUB_COMPLETE = 79,	// MsgEmpty	公共数据加载完成
	SS_ACK_LOAD_GUILD_COMPLETE = 80,	// MsgEmpty	公会加载完成
	SS_REQ_RESET_STATE = 83,	// MsgEmpty	请求重置加载状态
	SS_OBJECT_PROPERTY = 86,	// ObjectPropertyList	更新单条可视属性
	SS_OBJECT_TABLE = 87,	// ObjectVisRecord	更新单行可视表
	SS_REQ_DELETE_GUILD = 101,	// PubOpData	请求删除guild
	SS_REQ_DELETE_PUB = 102,	// PubOpData	请求删除pub

	// 关服流程
	SS_REQ_CLOSE_DBSERVER = 16,	// MsgEmpty	请求关闭DBServer
	SS_ACK_CLOSE_DBSERVER = 17,	// MsgEmpty	回复关闭DBServer
	SS_REQ_CLOSE_GAMESERVER = 18,	// MsgEmpty	请求关闭GameServer
	SS_ACK_CLOSE_GAMESERVER = 19,	// MsgEmpty	回复关闭GameServer
	SS_REQ_CLOSE_PUBSERVER = 20,	// MsgEmpty	请求关闭PubServer
	SS_ACK_CLOSE_PUBSERVER = 21,	// MsgEmpty	回复关闭PubServer
	SS_REQ_CLOSE_PROXYSERVER = 22,	// MsgEmpty	请求关闭ProxyServer
	SS_ACK_CLOSE_PROXYSERVER = 23,	// MsgEmpty	回复关闭ProxyServer
	SS_REQ_SAVE_PUB_DATA_EXIT = 121,	// PubNameData	关服保存Pub
	SS_REQ_SAVE_GUILD_DATA_EXIT = 122,	// ObjectDataPack	关服保存Guild
	SS_REQ_SAVE_PLATFORM_DATA_EXIT = 123,	// ObjectDataPack	关服保存Platform
	SS_ACK_OBJECT_SAVED = 124,	// GUID	反馈保存完毕

	// 平台服
	SS_REQ_START_LOAD_PLATFORM = 104,	// MsgEmpty	加载平台服
	SS_ACK_START_LOAD_PLATFORM = 105,	// PropertyList	回复平台服加载启动
	SS_ACK_LOAD_PLATFORM_DATA = 106,	// MsgEmpty	回复平台服加载数据
	SS_REQ_SAVE_PLATFORM_DATA = 107,	// MsgEmpty	请求保存平台服数据
	SS_ACK_LOAD_PLATFORM_COMPLETE = 108,	// MsgEmpty	回复加载平台服完成
	SS_REQ_CREATE_PLATFORM = 109,	// MsgEmpty	请求创建平台服
	SS_REQ_DELETE_PLATFORM = 110,	// MsgEmpty	请求删除平台服
	SS_ACK_CREATE_PLATFORM = 111,	// AckResult	回复创建平台服

	// 开服流程
	SS_SERVER_REGISTERED = 1,	// ServerInfoReport	注册服务
	SS_SERVER_UNREGISTERED = 2,	// ServerInfoReport	取消注册服务
	SS_SERVER_REFRESH = 3,	// ServerInfoReport	刷新信息
	SS_STS_NET_INFO = 4,	// ServerInfoReportList	通知转发服务信息
	SS_STS_HEART_BEAT = 5,	// ServerHeartBeat	心跳连接
	SS_NET_COMPLETE_DBSERVER = 6,	// MsgEmpty	DBServer网络完成
	SS_NET_COMPLETE_GAMESERVER = 7,	// MsgEmpty	GameServer网络完成
	SS_NET_COMPLETE_PUBSERVER = 8,	// MsgEmpty	PubServer网络完成
	SS_NET_COMPLETE_PROXYSERVER = 9,	// MsgEmpty	ProxyServer网络完成
	SS_SERVER_COMPLETE_ALLNET = 10,	// MsgEmpty	所有服务网络完成
	SS_OPEN_DBSERVER = 11,	// MsgEmpty	DBServer开启
	SS_OPEN_GAMESERVER = 12,	// MsgEmpty	GameServer开启
	SS_OPEN_PUBSERVER = 13,	// MsgEmpty	PubServer开启
	SS_OPEN_PROXYSERVER = 14,	// MsgEmpty	ProxyServer开启
	SS_REQ_SYN_INFO = 15,	// MsgEmpty	服务之间同步信息
	SS_NET_COMPLETE_PLATFORMSERVER = 24,	// MsgEmpty	PlatformServer网络完成
	SS_NET_COMPLETE_ROOMSERVER = 25,	// MsgEmpty	RoomServer网络完成
	SS_OPEN_PLATFORMSERVER = 26,	// MsgEmpty	PlatformServer开启
	SS_OPEN_ROOMSERVER = 27,	// MsgEmpty	RoomServer开启
	SS_OPEN_ALLSERVER = 28,	// MsgEmpty	所有服务开启
	SS_REMOVE_WORLDSERVER = 29,	// ServerInfoReportList	删除游戏服务
	SS_REQ_LOAD_PLAYER_GUID_NAME = 125,	// MsgEmpty	请求加载所有玩家GUID与Name
	SS_ACK_LOAD_PLAYER_GUID_NAME = 126,	// GuidNameList	应答所有玩家GUID与Name
	SS_ACK_LOAD_GUID_NAME_COMPLETE = 127,	// MsgEmpty	加载所有玩家GUID与Name完毕

	// 执行SQ脚本
	SS_SQL_EXECUTE = 97,	// SqlExecute	执行SQ脚本

	// 排行榜数据库
	SS_DB_RETURN_RANK_LIST = 90,	// ExtraMsg	DB服务器返回数据
	SS_DB_RANK_LIST_INIT = 91,	// RankListInit	初始化排行榜
	SS_DB_RANK_LIST_CREATE = 92,	// RankListCreate	创建排行榜
	SS_DB_RANK_LIST_DELETE = 93,	// RankListDelete	删除排行榜
	SS_DB_RANK_LIST_SET_DATA = 94,	// RankListData	设置数据
	SS_DB_RANK_LIST_GET_DATA = 95,	// RankListData	获取数据
	SS_DB_RANK_LIST_GET_RANGE_DATA = 96,	// RankListRange	获取范围数据
	SS_DB_RANK_LIST_UPDATE = 98,	// RankListUpdate	更新排行榜内容
	SS_DB_RANK_LIST_GET_RANK_DATA = 103,	// RankListRank	获取排名数据
	SS_DB_RANK_LIST_GET_DATA_ARRAY = 128,	// RankListDataArray	批量获取玩家数据

	// 玩家改名
	SS_REQ_PLAYER_RENAME = 81,	// 	请求玩家改名
	SS_ACK_PLAYER_RENAME = 82,	// 	回应玩家改名

	// 玩家状态机
	SS_REQ_PLAYER_REMOVE = 114,	// GUID	请求游戏服删除玩家
	SS_ACK_PLAYER_SAVED = 116,	// GUID	反馈给世界服玩家数据保存完毕
	SS_REQ_BREAK_CLIENT = 117,	// GUID	请求接入服断开客户端连接
	SS_ACK_USER_BREAK = 118,	// ClientConnectInfo	向世界服反馈客户端断开连接
	SS_REQ_PLAYER_LEAVE_GAME = 120,	// GUID	玩家离开游戏，立即保存数据

	// 登录流程
	SS_REQ_LOAD_ROLE_DATA = 50,	// ReqEnterGameServer	请求加载角色
	SS_ACK_LOAD_ROLE_DATA = 51,	// ObjectDataPack	回复加载角色
	SS_REQ_SAVE_ROLE_DATA = 52,	// ObjectDataPack	请求保存角色
	SS_ACK_SAVE_ROLE_DATA = 53,	// 	回复加载角色
	SS_ACK_ONLINE_NOTIFY = 54,	// RoleOnlineNotify	回复角色上线
	SS_ACK_OFFLINE_NOTIFY = 55,	// RoleOfflineNotify	回复角色下线
	SS_REQ_SAVE_ROLE_DATA_EXIT = 115,	// ObjectDataPack	保存角色，玩家退出前
	SS_REQ_UPDATE_PLAYER_DATA = 119,	// GUID	通知同步所有玩家数据给客户端

	// 编辑玩家数据
	SS_REQ_ALTER_PLAYER_DATA = 99,	// GUID	请求玩家数据
	SS_ACK_ALTER_PLAYER_DATA = 100,	// ObjectDataPack	应答玩家数据

	// 角色创建
	SS_REQ_PRECREATE_ROLE = 129,	// ReqCreateRole	请求预创建角色
	SS_ACK_PRECREATE_ROLE = 130,	// ReqCreateRole	应答预创建角色

	// 辅助服
	SS_REQ_SET_NOTES = 131,	// MsgEmpty	修改服务器状态
	SS_REQ_OPEN_SERVER = 132,	// MsgEmpty	请求开启服务器
	SS_REQ_CLOSE_SERVER = 133,	// MsgEmpty	请求关闭服务器

	// 附加服务器
	SS_REQ_EXTRA_SERVER = 88,	// ExtraMsg	请求附加服务器
	SS_ACK_EXTRA_SERVER = 89,	// ExtraMsg	附加服务器应答
};


enum Maintain
{

	// 修改服务器状态
	OPT_SET_SERVER_NOTES = 21,	// 请求:{"sid":"","notes":}	修改服务器状态

	// 关闭服务器
	OPT_CLOSE_SERVER = 23,	// 请求:{"sid":[]}	关闭服务器

	// 命令执行
	RUN_NET_COMMAND = 24,	// 命令:命令名加空格分割参数	命令执行

	// 对象修改
	SET_OBJ_PROP = 8,	// 请求:{"u":"", "p":"", "v":""};返回:{"u":"", "p":"", "v":""}	修改对象属性
	SET_OBJ_REC = 9,	// 	修改对象表
	SET_OBJ_CON = 10,	// 	修改对容器

	// 对象查看
	GET_OBJ_LIST = 3,	// 请求:{"t":0};返回:{"t":0,"l":[{"u":"", "n":""}]}	取得对象列表
	GET_OBJ_PROP_LIST = 4,	// 请求:{"u":""};返回:{"u":"", "p":[{"n":"", "v":""}]}	取得对象属性列表
	GET_OBJ_REC_LIST = 5,	// 请求:{"u":""};返回:{"u":"", "r":[""]}	取得对象表格列表
	GET_OBJ_CON_LIST = 6,	// 请求:{"u":""};返回:{"u":"", "c":[""]}	取得对象容器列表
	GET_OBJ_REC = 7,	// 请求:{"u":"","r":""};返回:{"u":"", "n":"","r":[[]]}	取得对象表格内容
	GET_OBJ_BEAT_LIST = 11,	// 请求:{"u":""};返回:{"u":"", "l":[""]}	取得对象定时器列表
	GET_OBJ_BEAT = 12,	// 请求:{"u":"","h":""};返回:{"u":"", "h":""}	取得对象定时器内容
	GET_OBJ_PROP_CRITICAL_LIST = 13,	// 请求:{"u":""};返回:{"u":"", "p":[{"n":"", "c":""}]}	取得对象属性回调列表
	GET_OBJ_REC_CALLBACK_LIST = 14,	// 请求:{"u":""};返回:{"u":"", "p":[{"n":"", "c":""}]}	取得对象表格回调列表
	GET_PLAYER_GN_LIST = 15,	// 请求:{};返回:{"l":[{"u":"", "n":""}]}	取得玩家GUID Name列表

	// 开启服务器
	OPT_OPEN_SERVER = 22,	// 请求:{"sid":[]}	开启服务器

	// 服务器列表
	GET_SERVER_LIST = 20,	// 请求:{"u":"","t":0};返回:{"u":"","l":[{}]}	取得服务器列表信息

	// 服务器状态
	GET_SERVER_STATE = 1,	// {server_name:"", state:0}	服务器状态
};


enum MsgID
{

	// 属性
	SC_PROPERTY_TABLE = 1009,	// ObjectPropertyList	下发所有可视属性
	SC_RECORD_TABLE = 1010,	// ObjectRecordList	下发所有可视表
	SC_OBJECT_PROPERTY = 1011,	// ObjectPropertyList	更新单条可视属性
	SC_OBJECT_TABLE = 1012,	// ObjectVisRecord	更新单行可视表

	// 测试网络通信
	CS_TEST_NET = 1040,	// NetTest	测试网络通信
	CS_NET_HEARTBEAT = 1041,	// 文本	网络心跳
	SC_TEST_NET = 1042,	// NetTest	测试网络通信

	// 登录流程
	CS_REQ_LOGIN = 1013,	// ReqAccountLogin	请求Login登录
	SC_ACK_LOGIN = 1014,	// AckEventResult	返回Login登录
	CS_REQ_LOGOUT = 1015,	// 	请求Login登出
	CS_REQ_WORLD_LIST = 1016,	// 	请求world列表
	SC_ACK_WORLD_LIST = 1017,	// AckWorldList	回复world列表
	CS_REQ_CONNECT_WORLD = 1018,	// ReqConnectWorld	请求连接world
	SC_ACK_CONNECT_WORLD = 1019,	// AckConnectWorldResult	回复连接world
	CS_REQ_VERIFY_KEY = 1020,	// ReqAccountLogin	请求验证串
	SC_ACK_VERIFY_KEY = 1021,	// AckEventResult	回复验证串
	CS_REQ_ROLE_LIST = 1022,	// ReqRoleList	请求角色列表
	SC_ACK_ROLE_LIST = 1023,	// RoleLiteInfo	回复角色列表
	CS_REQ_CREATE_ROLE = 1024,	// ReqCreateRole	请求创建角色
	SC_ACK_CREATE_ROLE = 1025,	// AckEventResult	回复创建角色
	CS_REQ_DELETE_ROLE = 1026,	// ReqDeleteRole	请求删除角色
	CS_REQ_ENTER_GAME = 1027,	// ReqEnterGameServer	请求进入游戏
	SC_ACK_ENTER_GAME = 1028,	// AckEventResult	回复进入游戏
	CS_REQ_LEAVE_GAME = 1029,	// ReqLeaveGameServer	请求离开游戏
	SC_ACK_LEAVE_GAME = 1030,	// 	回复离开游戏
	CS_REQ_ENTER_GAME_FINISH = 1031,	// ReqAckEnterGameSuccess	请求进入游戏完成
	SC_ACK_ENTER_GAME_FINISH = 1032,	// ReqAckEnterGameSuccess	回复进入游戏完成
	CS_VERIFY_ACCOUNT = 1038,	// ReqAccountLogin	帐号密码验证
	CS_REQ_GUEST_BIND = 1043,	// GuestBind	游客绑定平台
	SC_REQ_GUEST_BIND = 1044,	// GuestBind	游客绑定平台

	// 聊天服务器
	CS_CHAT_MSG_ROOM = 1035,	// ChatMsgRoom	客户端发消息到指定房间
	CS_CHAT_MSG_PLAYER = 1036,	// ChatMsgPlayer	客户端发消息给指定玩家
	CS_CHAT_UPDATA_CLIENT = 1037,	// ChatClientData	更新客户端信息
	CS_CHAT_RECORDING = 1039,	// ChatRecording	取聊天记录

	// 视窗
	SC_VIEW_CREATE = 1000,	// ViewCreate	视窗创建,容器的属性
	SC_VIEW_ADD = 1001,	// ViewAdd	视窗添加元素,元素的属性
	SC_VIEW_REMVOE = 1002,	// ViewRemove	视窗移除元素
	SC_VIEW_CHANGE = 1003,	// ViewChange	视窗元素调整位置
	SC_VIEW_CLOSE = 1004,	// ViewClose	视窗关闭
	SC_VIEW_PROPERTY = 1005,	// ViewProperty	视窗更新属性
	SC_VIEW_RECORD = 1006,	// ViewRecord	视窗更新表
	SC_VIEW_CELL_PROPERTY = 1007,	// ViewCellProperty	视窗元素更新属性
	SC_VIEW_CELL_RECORD = 1008,	// ViewCellRecord	视窗元素更新表
};

}
#endif // __OUTERMSGDEFINE_H__