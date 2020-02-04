///--------------------------------------------------------------------
/// �ļ���:		NetDefine.h
/// ��  ��      ������ض���
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_NetDefine_H__
#define __H_NetDefine_H__

//���������¼�
enum class NET_EVENT
{
	NET_EVENT_READING = 0x01,	/**< error encountered while reading */
	NET_EVENT_WRITING = 0x02,	/**< error encountered while writing */
	NET_EVENT_EOF = 0x10,		//���ӹر�
	NET_EVENT_ERROR = 0x20,		//���ӳ���
	NET_EVENT_TIMEOUT = 0x40,	//��ȡ��ʱ
	NET_EVENT_CONNECTED = 0x80,	//���ӳɹ�
};

#endif //__H_NetDefine_H__