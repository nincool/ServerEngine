///--------------------------------------------------------------------
/// �ļ���:		KConstDefine.h
/// ��  ��:		���Ķ���
/// ˵  ��:		�����ڲ����߼��㹲�õ�һЩ����
/// ��������:	2019��8��22��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __KERNEL_CONST_DEFINE_H__
#define __KERNEL_CONST_DEFINE_H__

#include "NFIModule.h"
#include "Dependencies/protobuf/src/google/protobuf/message.h"
#include "NFComm/NFUtils/QLOG.h"

// �¼�����,û��ʵ�ֵ���ע�͵�
enum EventType
{
	EVENT_OnDefault = 0,	// ��Ӧ�����¼�, ����ռ��ǰ��λ�ṩע����Ϣ,args{EventType�¼�����, string ��Ӧ�߼���}
	EVENT_OnNoSqlComplete,	// NoSql�������
	EVENT_OnNoSqlDisConnect,// NoSql�Ͽ�
	EVENT_OnMySqlComplete,	// MySql�������
	EVENT_OnMySqlDisConnect,// MySql�Ͽ�
	EVENT_OnAllNetComplete, // ���������Ѿ����
	EVENT_OnServerComplete,	// ��ǰ�����������
	EVENT_OnAllServerComplete,// ���з����������
	EVENT_OnServerStateChange,	//������״̬�ı� args{�߼�״̬��id, appid, type, ����״̬���Ƿ���������, SockIndex, �������ӷ�ʽ(0ΪNetServerInsideModule)}
 	EVENT_OnCreateClass,	// �߼��ഴ�� args{string �߼�����}
	EVENT_OnCreateRole,		// ��ɫ���� self ��ɫ args{string �Զ�������}
	EVENT_OnCreate,			// ���󴴽� self ����
	EVENT_OnDestroy,		// �������� self ����
	EVENT_OnEntry,			// �������� self ���� sender ���� args{int λ��(��0��ʼ)}
	EVENT_OnLeave,			// �뿪���� self ���� sender ����
	EVENT_OnNoAdd,			// ���������Ƿ�����������(���ط�0��ʾ������) self ���� sender ���� args{int λ��(��0��ʼ ������ʾ����λ��)}
	EVENT_OnAdd,			// �����������ǰ self ���� sender ���� args{int λ��(��0��ʼ)}
	EVENT_OnAfterAdd,		// ������������ self ���� sender ���� args{int λ��(��0��ʼ)}
	EVENT_OnNoRemove,		// ���������Ƴ�����(���ط�0��ʾ������) self ���� sender ����
	EVENT_OnBeforeRemove,	// �����Ƴ�����ǰ self ���� sender ���� args{int λ��(��0��ʼ)}
	EVENT_OnRemove,			// �����Ƴ������ self ���� sender ���� args{int λ��(��0��ʼ)}
	EVENT_OnChange,			// ��������ı�λ�� self ���� sender ���� args{int ��λ��(��0��ʼ), int ��λ��(��0��ʼ)}
	EVENT_OnLoad,			// ������������ļ���� self ����
	EVENT_OnRecover,		// ��������ݿ�ָ�������� self ����
	EVENT_OnComplete,		// ���󴴽����(��������ȫ���������) self ����
	EVENT_OnPubComplete,	// ����������ȫ���������
	EVENT_OnGuildComplete,	// ��������ȫ���������
	EVENT_OnEntryGame,		// ��ҽ�����Ϸ
	EVENT_OnReady,			// �ͻ��˾��� self ��ɫ
	//EVENT_OnContinue,		// �ͻ��˶������� self ��ɫ
	EVENT_OnDisconnect,		// �ͻ��˶��� self ��ɫ
	EVENT_OnStore,			// ���ǰ self ���� args{int ����(StoreState)}
	//EVENT_OnSendLetter,		// �����ż���� self ������ sender �ռ��� args{int ���(1�ɹ�)}
	EVENT_OnQueryLetter,	// ��ѯ�ż���� self ����� args{int ��ȡ���ż�����}
	//EVENT_OnRecvLetter,		// ��ȡ�ż�(���ݿ�ɾ��) self ������ args{string ������ID, string ����������, int64 �ż�����ʱ��, int �ż�����, string �ż�����, string �ż�����, string �ż���ˮ��}
	//EVENT_OnRecvLetterFail,	// ��ȡ�ż�ʧ�� self ������ args{string �ż���ˮ��}
	EVENT_OnLookLetter,		// �鿴�ż�(���ݿⲻɾ��)  self ������ args{int �ż����� string �ż���ˮ�ţ�guid ������ID, string ����������, string �ż�����ʱ��, string �ż�����, string �ż�����, string �ż�����}
	//EVENT_OnServerClose,	// �������ر�
	EVENT_OnChangeName,		// ���� self ��ɫ args{int ���(1�ɹ�), string ������, string ������}
	EVENT_CALLBACK_MAX,
};

typedef int (NFIModule::* EVENT_HOOK)(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
// ���滻ע��ص��� ����ָ��, ����ָ��, ���ȼ�
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

	// ��ȡ���ȼ�
	virtual int GetPrior()
	{
		return m_nPrior;
	}

	// ��ȡ����
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
// ʹ��PB�ṹΪ�����Ļص����ص�����Ҫ���ݻص������PB�ṹ������������������ 
// �ص����� int FunctionName(const NFGUID& self, const NFGUID& sender, const PBMsg& msg)
// FunctionName Ϊ�Զ��庯�� PBMsgΪ�Զ���ṹ
// �˺궨�������� NF_SHARE_PTR<IMethodMsg> ���ݸ�ע�ắ�� ����1 ���� ����2 ���ȼ� (Ĭ��Ϊ0)
#define METHOD_MSG(func, ...) RTTIMsgFuncOf(this, &func, __VA_ARGS__)

// ֻ����ʹ�õ�������Ĵ���ʹ�ô˺�
// ��Ҫ�����κ���������
#define CATCH_BEGIN try {
#define CATCH_END \
} catch (std::exception & ia) \
{ \
	QLOG_ERROR << "exception " << ia.what(); \
	assert(0); \
}

enum StoreState
{
	STORE_EXIT,		// ����뿪��Ϸ
	STORE_TIMING,	// ��ʱ����
	STORE_MANUAL,	// �˹����棨����SavePlayerData��
};

//����������
enum NF_SERVER_TYPES
{
	NF_ST_NONE = 0,			// NONE
	NF_ST_REDIS = 1,		//REDIS������
	NF_ST_MYSQL = 2,		//MYSQL������
	NF_ST_MASTER = 3,		//��������
	NF_ST_LOGIN = 4,		//��¼������
	NF_ST_PROXY = 5,		//������������
	NF_ST_GAME = 6,			//����������
	NF_ST_WORLD = 7,		//���������
	NF_ST_DB = 8,			//���ݴ�ȡ������
	NF_ST_PUB = 9,			//����������
	NF_ST_CHAT = 10,		//���������
	NF_ST_RANK = 11,		//���з�����
	NF_ST_PLATFORM = 12,	//ƽ̨������
	NF_ST_ROOM = 13,		//ս��������
	NF_ST_RANK_DB = 14,		//���а�DB
	NF_ST_MAX,				//�������ֵ
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
// Res Map��ʽ������
// <������, ����ֵ>
typedef std::map<std::string, NFData> MapRowData;
// <ID, ��������>
typedef std::map<std::string, MapRowData> ResMapStrData;
typedef std::map<int, MapRowData> ResMapIntData;

// ���а���������
enum RankOrderType
{
	RANK_ORDER_DESC = 0, // ����
	RANK_ORDER_ASCE = 1, // ����
};

// �������״̬
enum OnlineState
{
	ONLINE_TRUE = 0,	// ����
	ONLINE_FALSE,		// ���� (�����¼�ȫ��������)

	// �༭״̬(û�д����κ��¼�) 
	// ����ֻ�� AlterPlayer�ص����ܷ��ش�״̬�����
	// ��״̬�����ͨ��GetObject�ӿڻ�ȡ����
	ONLINE_ALTER,
};

#endif // __KERNEL_CONST_DEFINE_H__

