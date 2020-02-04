///--------------------------------------------------------------------
/// �ļ���:		MaintainMsgDataDefine.h
/// ��  ��:		ά��ģ�� ͨ�����ݸ�ʽ����
/// ˵  ��:		
/// ��������:	2019��10��14��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_MaintainMsgDataDefine_H__
#define __H_MaintainMsgDataDefine_H__
#include "Dependencies/ajson/ajson.hpp"

//����ȡ�ö����б�����
struct REQGetObjList 
{ 
	std::string u = "";
	int t = 0; 
};
AJSON(REQGetObjList, u, t);

//����ȡ�ö���
struct ACKGetOBJ
{
	std::string u;
	std::string n;
	std::string c;
	std::string cf;
	int t = 0;
};
AJSON(ACKGetOBJ, u, n, c, cf, t);

//�����б�
struct ACKGetOBJList
{
	std::string p;
	std::vector<ACKGetOBJ> l;
};
AJSON(ACKGetOBJList, p, l);

//�������ѡ��
struct REQSelectObj { std::string u; };
AJSON(REQSelectObj, u);

//��������
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

//���������б�
struct ACKGetObjPropList
{
	std::string u;
	std::vector<JProp> p;
};
AJSON(ACKGetObjPropList, u, p);


//ȡ�ö������б�
struct ACKGetObjRecList
{
	std::string u;
	std::vector<std::string> l;
	int pr_cout = 0;
	int s_count = 0;
};
AJSON(ACKGetObjRecList, u, pr_cout, l, s_count);

//ѡ���
struct J_SelectObjRec
{ 
	std::string u;
	std::string r;
};
AJSON(J_SelectObjRec, u, r);

//����ı�����
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

//ȡ�ö���ʱ���б�
struct ACKGetObjBeatList
{
	std::string u;
	std::vector<std::string> l;
};
AJSON(ACKGetObjBeatList, u, l);

//ѡ��ʱ��
struct J_SelectObjBeat
{
	std::string u;
	std::string h;
};
AJSON(J_SelectObjBeat, u, h);

//����Ķ�ʱ������
struct J_ObjBeatData
{
	int slot;		// ʱ����λ��
	int circle;		// ʱ����Ȧ��
	OBJECT_HEARTBEAT_FUNCTOR_PTR pFunc;
	bool del;		//�Ƿ��Ѿ���ɾ��
	int64_t time;
	int mc;		//������
	int ct;		//����
	std::string u;
	std::string h;
};
AJSON(J_ObjBeatData, u, h, slot, circle, del, time, mc, ct);

//�������Իص�
struct JPropCritical
{
	std::string n;
	int c;
};
AJSON(JPropCritical, n, c);

//�������Իص��б�
struct ACKGetObjPropCriticalList
{
	std::string u;
	std::vector<JPropCritical> p;
};
AJSON(ACKGetObjPropCriticalList, u, p);

//������ص�
struct JRecCallBack
{
	std::string n;
	int c;
};
AJSON(JRecCallBack, n, c);

//������ص��б�
struct ACKGetObjRecCallBackList
{
	std::string u;
	std::vector<JRecCallBack> p;
};
AJSON(ACKGetObjRecCallBackList, u, p);

//����ȡ�÷�������Ϣ�б�����
struct REQGetServerInfoList
{
	std::string u = "";
	int t = 0;
};
AJSON(REQGetServerInfoList, u, t);

//��������Ϣ
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

//��������Ϣ�б�
struct ACKGetServerInfoList
{
	std::string u;
	std::vector<ACKGetServerInfo> l;
};
AJSON(ACKGetServerInfoList, u, l);

//�޸Ķ�������
struct J_ModifyObjProp
{
	std::string u;
	std::string p;
	std::string v;
};
AJSON(J_ModifyObjProp, u, p, v);

//�޸Ķ����
struct J_ModifyObjRec
{
	std::string u;
	std::string r;
	int row = -1;
	std::vector<std::string> data;
};
AJSON(J_ModifyObjRec, u, r, row, data);

//�޸�����
struct J_ModifyChild
{
	std::string parent;
	std::string remove;
};
AJSON(J_ModifyChild, parent, remove);

//�����ķ�����Notes��Ϣ
struct REQOptServerNotes
{
	std::string sid;
	std::string notes;
};
AJSON(REQOptServerNotes, sid, notes);

//�����ķ�����Notes��Ϣ��Ӧ
struct ACKOptServerNotes
{
	int result;
};
AJSON(ACKOptServerNotes, result);

//����ȡ�����GUID Name
struct ACKPlayerGN
{
	std::string u;
	std::string n;
};
AJSON(ACKPlayerGN, u, n);

//ȡ�����GUID Name�б�
struct ACKPlayerGNList
{
	std::vector<ACKPlayerGN> l;
};
AJSON(ACKPlayerGNList, l);
#endif //__H_MaintainMsgDataDefine_H__