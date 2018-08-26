//GC Common Def

#ifndef __COMMSTRUDEF_H__
#define __COMMSTRUDEF_H__

#include <stdint.h>

/********** ��Ϣ���ṹ **********/
typedef struct _tagStruMsg
{
	/*************** ��Ϣͷ *******************/
	uint32_t msg_head_len;//��Ϣͷ���ȣ�
    uint32_t client_version;//�ͻ��˰汾
    uint64_t key_token;//��½�ɹ����ص�token������ʱЧ��
    uint64_t msg_id;//��Ϣid��ȫ��Ψһ
    uint64_t msg_type;//��Ϣ����
    uint32_t msg_seq;//��Ϣ��ţ�ÿ����Ϣ�ϴ��´ﶼ����
    uint8_t* msg_reserve;//��ϢͷԤ����Ԥ�����ȣ�msg_head_len-client_version-key_token-msg_id-msg_seq-msg_type

	/*************** ��Ϣ�� *******************/
    uint32_t msg_data_len;//��Ϣ�峤��
    uint8_t* msg_data;//��Ϣ����
}StruMsg, *PStruMsg;


/********** ע����Ϣ����������ͨ�ýṹ **********/
typedef struct _tagStruCommRegisterMsgBodyReq
{
    uint8_t phone_number[25];//�绰����
	uint8_t nick_name[25];//�ǳ�
    uint64_t ip;//�˺ž�����¼ip[���ܻᶯ̬�仯]
    uint64_t create_date_time;//����ʱ��
}StruCommRegisterMsgBodyReq, *PStruCommRegisterMsgBodyReq;

/********** ע����Ϣ�����ݻظ�ͨ�ýṹ **********/
typedef struct _tagStruCommRegisterMsgBodyReply
{
    uint64_t user_id;//�ظ�ע������,����ȫ��Ψһid
}StruCommRegisterMsgBodyReply, *PStruCommRegisterMsgBodyReply;


/********** �޸ĸ���������Ϣ������ͨ�ýṹ **********/
typedef struct _tagStruPersonalDataMsgBodyReq
{
    bool gender;//�Ա�[1��ʾ�У�0��ʾŮ]
    uint8_t nick_name[25];//�ǳ�
    uint64_t birthday;//��������
    uint8_t blood_type;//Ѫ��[0��ʾA��1��ʾB��2��ʾO�� 3��ʾAB]
    uint8_t occupation[25];//ְҵ
    uint8_t hometown[50];//����
    uint8_t location[50];//��ǰ���ڵ�
    uint8_t school[50];//��ҵԺУ
    uint8_t company[50];//��˾����
    uint8_t label[50];//��ǩ����Ӧuser_lable��
    uint8_t personality[100];//����ǩ��
    uint8_t head_portrait_path[100];//ͷ���ַ
    uint8_t user_id[50];//�û�id
    uint64_t create_date_time;//�޸�ʱ��
}StruPersonalDataMsgBodyReq, *PStruPersonalDataMsgBodyReq;


/********** ��¼��Ϣ����������ͨ�ýṹ **********/
typedef struct _tagLoginMsgBodyReq
{
    uint8_t authorization_type;//��Ȩ����[0��ʾ�ֶ�ע��-�������id��1��ʾ�绰���룬2��ʾ����, 3��ʾQQ��4��ʾ΢�ţ�5��ʾ΢��]
    uint8_t authorization_uuid[50];//��Ȩuuid
    uint8_t credential[50];//��Ȩƾ֤
    uint32_t key_token_survival_time;//��Ϣ������Կ����Ч��
    uint64_t login_ip;//��¼�豸ip
    uint64_t login_datetime;//��¼ʱ��
    uint8_t verification_code[25];//��֤��
}LoginMsgBodyReq, *PLoginMsgBodyReq;


/********** ��¼��Ϣ��ظ�����ͨ�ýṹ **********/
typedef struct _tagLoginMsgBodyReply
{
    uint8_t authorization_type;//��Ȩ����[0��ʾ�ֶ�ע��-�������id��1��ʾ�绰���룬2��ʾ����, 3��ʾQQ��4��ʾ΢�ţ�5��ʾ΢��]
    uint8_t authorization_uuid[50];//��Ȩuuid
    uint8_t credential[50];//��Ȩƾ֤
    uint64_t login_ip;//��¼�豸ip
    uint64_t login_datetime;//��¼ʱ��
    uint8_t verification_code[25];//��֤��
}LoginMsgBodyReply, *PLoginMsgBodyReply;


/********** �ǳ���Ϣ����������ͨ�ýṹ **********/
typedef struct _tagLogoutMsgBodyReq
{
    uint64_t user_id;//�û�id[�ֶ��˳�������֮ǰЭ���key_token]
    uint64_t logout_datetime;//�ǳ�ʱ��
}LogoutMSgBodyReq, *PLogoutMsgBodyReq;


/********** ������Ϣ������ͨ�ýṹ **********/
typedef struct _tagRealtimeRankMsgBody
{
    uint8_t rank_type;//�������е�����
}RealtimeRankMsgBody, *PRealtimeRankMsgBody;


/********** ������Ϣ������ͨ�ýṹ **********/
typedef struct _tagSearchMsgBody
{
    uint8_t search_condition[250];//��ѯ����
    uint8_t filter_condition[50];//��������[ϵͳĬ���趨�Ŀ�ѡ��]
    uint64_t search_datetime;//��ѯʱ��[��������л�ȡ������Ҫǰ�˴���]
}SearchMsgBody, *PSearchMsgBody;


/********** ��ѯ������Ϣ������ͨ�ýṹ **********/
typedef struct _tagQueryUserDataMsgBody
{
    uint64_t user_id;//�û�id
}QueryUserDataMsgBody, *PQueryUserDataMsgBody;


/********** ��ѯ��ʷ��Ϣ������ͨ�ýṹ **********/
typedef struct _tagQueryHistoryMsgBody
{
    uint8_t query_type;//��ѯ����[0��ʾ������ʷ��1��ʾ���������ʷ��2��ʾ������ʷ��3��ʾ���Ե�������ʷ��4��ʾ���Զ�������ʷ]
    uint64_t history_datetime;//����ѯ����ʷʱ��
	union CommonCondition//ͨ�������ṹ
	{
		union AddFriendHistory
		{
            uint64_t friend_id;//����id
		};

		union CTCChatHistory
		{
            uint64_t user_id;//�������id
		};
	
		union CTGChatHistory
		{
            uint64_t room_id;//����id
		};
	};

}QueryHistoryMsgBody, *PQueryHistoryMsgBody;


/********** ��Ӻ�����Ϣ������ͨ�ýṹ **********/
typedef struct _tagAddFriendMsgBody
{
    uint64_t friend_id;//��������ѵ��û�id
    uint8_t remarks_data[100];//��ע��Ϣ��ת�������������
    uint8_t group_name[50]; //������������������
    uint8_t remarks_name[50];//�����ѵı�ע
    uint64_t add_datetime;//���ʱ��
}AddFriendMsgBody, *PAddFriendMsgBody;


/********** ɾ��������Ϣ������ͨ�ýṹ **********/
typedef struct _tagDelFriendMsgBody
{
    uint64_t friend_id;//��ɾ�����ѵ��û�id
    uint64_t del_datetime;//ɾ��ʱ��[��������л�ȡ������Ҫǰ�˴���]
}DelFriendMsgBody, *PDelFriendMsgBody;


/********** ���ں�����Ϣ������ͨ�ýṹ **********/
typedef struct _tagBlackFriendMsgBody
{
    uint64_t friend_id;//���������ѵ��û�id
    uint64_t del_datetime;//����ʱ��[��������л�ȡ������Ҫǰ�˴���]
}BlackFriendMsgBody, *PBlackFriendMsgBody;


/********** ���ѷ����ƶ���Ϣ������ͨ�ýṹ **********/
typedef struct _tagMoveFriendGroupMsgBody
{
    uint64_t friend_id;//���ƶ�����id
    uint8_t group_name[25];//�ƶ���������
    uint64_t move_datetime;//�ƶ��¼�[��������л�ȡ������Ҫǰ�˴���]
}MoveFriendGroupMsgBody, *PMoveFriendGroupMsgBody;


/********** ������Ϣ������ͨ�ýṹ **********/
typedef struct _tagChatMsgBody
{
    uint64_t receiver_id;//�����շ�id[������û�id��ʾ���Ե�������Ƿ���id��ʾȺ��]
    uint64_t send_datetime;//���͵�ʱ��
    uint8_t data_type;//��Ϣ����[0��ʾ���֣�1��ʾͼ�꣬2��ʾͼƬ��3��ʾ��Ƶ]
    uint64_t sequence;//���к�[������͵���Ϣһ���޷�ȫ�����ͣ���Ҫ��β�������ֶ����ڶ����ݽ���������]
    uint8_t* data;//ʵ������
}ChatMsgBody,*PChatMsgBody;


/********** ����������Ϣ������ͨ�ýṹ **********/
typedef struct _tagCreateRoomMsgBody
{
    uint8_t room_label[50];//�����ǩ
    uint8_t room_type[50];//��������[ϵͳĬ���趨�Ŀ�ѡ��]
    uint8_t room_name[50];//��������
    uint8_t room_password[50];//��������
    uint64_t create_datetime;//����ʱ��[��������л�ȡ������Ҫǰ�˴���]
    uint64_t survival_time;//���ʱ��[�û��ĵȼ�Խ�ߴ���ʱ��Խ��;���û����ͨ�����ӻ�Ծ���ӳ����������]
}CreateRoomMsgBody, *PCreateRoomMsgBody;



/********** ����ɾ����Ϣ������ͨ�ýṹ **********/
typedef struct _tagDelRoomMsgBody
{
    uint64_t room_id;//��ɾ���ķ���id
    uint64_t del_datetime;//ɾ��ʱ��
}DelRoomMsgBody, *PDelRoomMsgBody;


/********** ���뷿����Ϣ������ͨ�ýṹ **********/
typedef struct _tagEnterRoomMsgBody
{

    uint64_t room_id;//������ķ���id
    uint64_t enter_datetime;//����ʱ��
}EnterRoomMsgBody, *PEnterRoomMsgBody;


/********** �˳�������Ϣ������ͨ�ýṹ **********/
typedef struct _tagExitRoomMsgBody
{
    uint64_t room_id;//������ķ���id
    uint64_t exit_datetime;//����ʱ��
}ExitRoomMsgBody, *PExitRoomMsgBody;





/*******************************************/
/*******************************************/
/*******************************************/
/*******************************************/
/*******************************************/





#endif













