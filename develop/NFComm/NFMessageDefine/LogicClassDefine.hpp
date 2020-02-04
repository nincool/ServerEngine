// -------------------------------------------------------------------------
//    @FileName         :    LogicClassDefine.hpp
//    @Author           :    StructDefine Tool
//    @Module           :    LogicClassDefine
// -------------------------------------------------------------------------

#ifndef __LogicClassDefine_HPP__
#define __LogicClassDefine_HPP__

#include <string>

namespace LC
{

	class IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "IObject"; return x; };

		// property
		// int 容量
		static const std::string& Capacity(){ static std::string x = "Capacity"; return x; };
		// string 逻辑类名
		static const std::string& ClassName(){ static std::string x = "ClassName"; return x; };
		// string 配置ID
		static const std::string& ConfigID(){ static std::string x = "ConfigID"; return x; };
		// object 唯一标识
		static const std::string& Guid(){ static std::string x = "Guid"; return x; };
		// string 唯一标识 字符串形式
		static const std::string& ID(){ static std::string x = "ID"; return x; };
		// int 在容器中的位置 从0开始
		static const std::string& Index(){ static std::string x = "Index"; return x; };
		// string 名称
		static const std::string& Name(){ static std::string x = "Name"; return x; };
		// int 类型
		static const std::string& Type(){ static std::string x = "Type"; return x; };

		// record
	};

	class Helper : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "Helper"; return x; };

		// property

		// record
	};

	class NoSqlServer : public Helper
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "NoSqlServer"; return x; };

		// property
		// string 
		static const std::string& Auth(){ static std::string x = "Auth"; return x; };
		// int 
		static const std::string& ConnectType(){ static std::string x = "ConnectType"; return x; };
		// string 
		static const std::string& IP(){ static std::string x = "IP"; return x; };
		// int 
		static const std::string& Port(){ static std::string x = "Port"; return x; };
		// int 
		static const std::string& ServerID(){ static std::string x = "ServerID"; return x; };

		// record
	};

	class Server : public Helper
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "Server"; return x; };

		// property
		// int Buffer容量
		static const std::string& BufferSize(){ static std::string x = "BufferSize"; return x; };
		// int 客户端是否需要连接
		static const std::string& ClientConnect(){ static std::string x = "ClientConnect"; return x; };
		// string 连接的服务器
		static const std::string& ConnectServer(){ static std::string x = "ConnectServer"; return x; };
		// int CPU个数（机器）
		static const std::string& CpuCount(){ static std::string x = "CpuCount"; return x; };
		// int Http服务
		static const std::string& HttpServer(){ static std::string x = "HttpServer"; return x; };
		// string IP地址
		static const std::string& IP(){ static std::string x = "IP"; return x; };
		// string 维护IP(没双IP可不填)
		static const std::string& MaintainIP(){ static std::string x = "MaintainIP"; return x; };
		// int 维护Port
		static const std::string& MaintainPort(){ static std::string x = "MaintainPort"; return x; };
		// int 作为服务器时：服务器最大连接
		static const std::string& MaxOnline(){ static std::string x = "MaxOnline"; return x; };
		// int 作为服务器时：相对客户端的port
		static const std::string& Port(){ static std::string x = "Port"; return x; };
		// int 服务器ID
		static const std::string& ServerID(){ static std::string x = "ServerID"; return x; };
		// string 服务器名称
		static const std::string& ServerName(){ static std::string x = "ServerName"; return x; };
		// int 服务器类型
		static const std::string& Type(){ static std::string x = "Type"; return x; };
		// int WebPort
		static const std::string& WebPort(){ static std::string x = "WebPort"; return x; };

		// record
	};

	class DBServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "DBServer"; return x; };

		// property

		// record
	};

	class GameServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "GameServer"; return x; };

		// property

		// record
	};

	class WorldServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "WorldServer"; return x; };

		// property

		// record
	};

	class PubServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "PubServer"; return x; };

		// property

		// record
	};

	class MasterServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "MasterServer"; return x; };

		// property

		// record
	};

	class ProxyServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "ProxyServer"; return x; };

		// property

		// record
	};

	class LoginServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "LoginServer"; return x; };

		// property

		// record
	};

	class RankServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "RankServer"; return x; };

		// property

		// record
	};

	class RankDBServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "RankDBServer"; return x; };

		// property

		// record
	};

	class PlatformServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "PlatformServer"; return x; };

		// property

		// record
	};

	class RoomServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "RoomServer"; return x; };

		// property

		// record
	};

	class ChatServer : public Server
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "ChatServer"; return x; };

		// property

		// record
	};

	class MySqlServer : public Helper
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "MySqlServer"; return x; };

		// property
		// int 
		static const std::string& ConnectNum(){ static std::string x = "ConnectNum"; return x; };
		// string 
		static const std::string& IP(){ static std::string x = "IP"; return x; };
		// string 
		static const std::string& Id(){ static std::string x = "Id"; return x; };
		// int 
		static const std::string& Port(){ static std::string x = "Port"; return x; };
		// string 
		static const std::string& Pwd(){ static std::string x = "Pwd"; return x; };
		// string 
		static const std::string& User(){ static std::string x = "User"; return x; };

		// record
	};

	class PlatformManager : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "PlatformManager"; return x; };

		// property

		// record
		// 活动列表
		class BattleList
		{
		public:
			// record name
			static const std::string& ThisName(){ static std::string x = "BattleList"; return x; };
			static const int Col = 2;
			static const int Row = 100;
			// object 
			static const int objID = 0;
			// int 
			static const int battletype = 1;
		};
	};

	class Battle : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "Battle"; return x; };

		// property
		// int 活动序列
		static const std::string& BattleIndex(){ static std::string x = "BattleIndex"; return x; };
		// int 活动状态
		static const std::string& BattleState(){ static std::string x = "BattleState"; return x; };
		// int 开启时间
		static const std::string& BeginTime(){ static std::string x = "BeginTime"; return x; };
		// int 结束时间
		static const std::string& EndTime(){ static std::string x = "EndTime"; return x; };
		// int 子活动类型
		static const std::string& SubType(){ static std::string x = "SubType"; return x; };

		// record
	};

	class Player : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "Player"; return x; };

		// property
		// string 玩家的帐号
		static const std::string& Account(){ static std::string x = "Account"; return x; };
		// int 创建时间
		static const std::string& CreateTime(){ static std::string x = "CreateTime"; return x; };
		// int GM等级
		static const std::string& GMLevel(){ static std::string x = "GMLevel"; return x; };
		// int 登陆的服ID
		static const std::string& GameID(){ static std::string x = "GameID"; return x; };
		// int 登陆的网关ID
		static const std::string& GateID(){ static std::string x = "GateID"; return x; };
		// string 网络地址
		static const std::string& IP(){ static std::string x = "IP"; return x; };
		// string 简略数据,跟随角色列表发送给客户端
		static const std::string& LiteData(){ static std::string x = "LiteData"; return x; };
		// int 下线时间戳
		static const std::string& OfflineTime(){ static std::string x = "OfflineTime"; return x; };
		// int 允许登录时间戳 秒
		static const std::string& OpenTime(){ static std::string x = "OpenTime"; return x; };
		// int 在线总秒数
		static const std::string& TotalSec(){ static std::string x = "TotalSec"; return x; };

		// record
	};

	class Pub : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "Pub"; return x; };

		// property
		// int Pub序列
		static const std::string& PubIndex(){ static std::string x = "PubIndex"; return x; };

		// record
	};

	class Guild : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "Guild"; return x; };

		// property

		// record
	};

	class ItemBase : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "ItemBase"; return x; };

		// property

		// record
	};

	class BoxBase : public IObject
	{
	public:
		// class name
		static const std::string& ThisName(){ static std::string x = "BoxBase"; return x; };

		// property

		// record
	};
}

#endif // __LogicClassDefine_HPP__