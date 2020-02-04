///--------------------------------------------------------------------
/// 文件名:		MaintainMsgDataDefine.h
/// 内  容:		维护模块 通信数据格式定义
/// 说  明:		
/// 创建日期:	2019年10月14日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_MaintainMsgDataDefine_H__
#define __H_MaintainMsgDataDefine_H__
#include "Dependencies/ajson/ajson.hpp"

//请求取得对象列表类型
struct REQGetObjList 
{ 
	std::string u = "";
	int t = 0; 
};
AJSON(REQGetObjList, u, t);

//请求取得对象
struct ACKGetOBJ
{
	std::string u;
	std::string n;
	std::string c;
	std::string cf;
	int t = 0;
};
AJSON(ACKGetOBJ, u, n, c, cf, t);

//对象列表
struct ACKGetOBJList
{
	std::string p;
	std::vector<ACKGetOBJ> l;
};
AJSON(ACKGetOBJList, p, l);

//请求对象选择
struct REQSelectObj { std::string u; };
AJSON(REQSelectObj, u);

//对象属性
struct JProp
{
	std::string n;
	std::string v;
	int t = 0;
	bool pr;
	bool pu;
	bool s;
};
AJSON(JProp, n, v, t, pr, pu, s);

//对象属性列表
struct ACKGetObjPropList
{
	std::string u;
	std::vector<JProp> p;
};
AJSON(ACKGetObjPropList, u, p);


//取得对象表格列表
struct ACKGetObjRecList
{
	std::string u;
	std::vector<std::string> l;
	int pr_cout = 0;
	int s_count = 0;
};
AJSON(ACKGetObjRecList, u, pr_cout, l, s_count);

//选择表
struct J_SelectObjRec
{ 
	std::string u;
	std::string r;
};
AJSON(J_SelectObjRec, u, r);

//对象的表内容
struct J_ObjRecData
{
	std::string u;
	std::string r;
	int row = 0;
	int rowMax = 0;
	int col = 0;
	bool pr;
	bool pu;
	bool s;
	std::vector<std::vector<std::string>> data;
};
AJSON(J_ObjRecData, u, r, row, col, rowMax, pr, s, data);

//取得对象定时器列表
struct ACKGetObjBeatList
{
	std::string u;
	std::vector<std::string> l;
};
AJSON(ACKGetObjBeatList, u, l);

//选择定时器
struct J_SelectObjBeat
{
	std::string u;
	std::string h;
};
AJSON(J_SelectObjBeat, u, h);

//对象的定时器内容
struct J_ObjBeatData
{
	int slot;		// 时间轮位置
	int circle;		// 时间轮圈数
	OBJECT_HEARTBEAT_FUNCTOR_PTR pFunc;
	bool del;		//是否已经被删除
	int64_t time;
	int mc;		//最大次数
	int ct;		//次数
	std::string u;
	std::string h;
};
AJSON(J_ObjBeatData, u, h, slot, circle, del, time, mc, ct);

//对象属性回调
struct JPropCritical
{
	std::string n;
	int c;
};
AJSON(JPropCritical, n, c);

//对象属性回调列表
struct ACKGetObjPropCriticalList
{
	std::string u;
	std::vector<JPropCritical> p;
};
AJSON(ACKGetObjPropCriticalList, u, p);

//对象表格回调
struct JRecCallBack
{
	std::string n;
	int c;
};
AJSON(JRecCallBack, n, c);

//对象表格回调列表
struct ACKGetObjRecCallBackList
{
	std::string u;
	std::vector<JRecCallBack> p;
};
AJSON(ACKGetObjRecCallBackList, u, p);

//请求取得服务器信息列表类型
struct REQGetServerInfoList
{
	std::string u = "";
	int t = 0;
};
AJSON(REQGetServerInfoList, u, t);

//服务器信息
struct ACKGetServerInfo
{
	int s_id;
	std::string s_name;
	std::string s_ip;
	int s_port;
	std::string m_ip;
	int m_port;
	int max_ol;
	int cur_c;
	int state;
	int type;
	int app_id;
	int dist_id;
	std::string notes;
};
AJSON(ACKGetServerInfo, s_id, s_name, s_ip, s_port, m_ip, m_port, max_ol, cur_c, state, type, app_id, dist_id, notes);

//服务器信息列表
struct ACKGetServerInfoList
{
	std::string u;
	std::vector<ACKGetServerInfo> l;
};
AJSON(ACKGetServerInfoList, u, l);

//修改对象属性
struct J_ModifyObjProp
{
	std::string u;
	std::string p;
	std::string v;
};
AJSON(J_ModifyObjProp, u, p, v);

//修改对象表
struct J_ModifyObjRec
{
	std::string u;
	std::string r;
	int row = -1;
	std::vector<std::string> data;
};
AJSON(J_ModifyObjRec, u, r, row, data);

//修改容器
struct J_ModifyChild
{
	std::string parent;
	std::string remove;
};
AJSON(J_ModifyChild, parent, remove);

//操作的服务器Notes信息
struct REQOptServerNotes
{
	std::string sid;
	std::string notes;
};
AJSON(REQOptServerNotes, sid, notes);

//操作的服务器Notes信息响应
struct ACKOptServerNotes
{
	int result;
};
AJSON(ACKOptServerNotes, result);

//请求取得玩家GUID Name
struct ACKPlayerGN
{
	std::string u;
	std::string n;
};
AJSON(ACKPlayerGN, u, n);

//取得玩家GUID Name列表
struct ACKPlayerGNList
{
	std::vector<ACKPlayerGN> l;
};
AJSON(ACKPlayerGNList, l);
#endif //__H_MaintainMsgDataDefine_H__