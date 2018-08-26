//GC Common Def

#ifndef __COMMSTRUDEF_H__
#define __COMMSTRUDEF_H__

#include <stdint.h>

/********** 消息泛结构 **********/
typedef struct _tagStruMsg
{
	/*************** 消息头 *******************/
	uint32_t msg_head_len;//消息头长度，
    uint32_t client_version;//客户端版本
    uint64_t key_token;//登陆成功返回的token；具有时效性
    uint64_t msg_id;//消息id；全局唯一
    uint64_t msg_type;//消息类型
    uint32_t msg_seq;//消息序号；每次消息上传下达都＋１
    uint8_t* msg_reserve;//消息头预留；预留长度＝msg_head_len-client_version-key_token-msg_id-msg_seq-msg_type

	/*************** 消息体 *******************/
    uint32_t msg_data_len;//消息体长度
    uint8_t* msg_data;//消息数据
}StruMsg, *PStruMsg;


/********** 注册消息体请求数据通用结构 **********/
typedef struct _tagStruCommRegisterMsgBodyReq
{
    uint8_t phone_number[25];//电话号码
	uint8_t nick_name[25];//昵称
    uint64_t ip;//账号经常登录ip[可能会动态变化]
    uint64_t create_date_time;//创建时间
}StruCommRegisterMsgBodyReq, *PStruCommRegisterMsgBodyReq;

/********** 注册消息体数据回复通用结构 **********/
typedef struct _tagStruCommRegisterMsgBodyReply
{
    uint64_t user_id;//回复注册请求,返回全局唯一id
}StruCommRegisterMsgBodyReply, *PStruCommRegisterMsgBodyReply;


/********** 修改个人资料消息体数据通用结构 **********/
typedef struct _tagStruPersonalDataMsgBodyReq
{
    bool gender;//性别[1表示男，0表示女]
    uint8_t nick_name[25];//昵称
    uint64_t birthday;//出生年月
    uint8_t blood_type;//血型[0表示A，1表示B，2表示O， 3表示AB]
    uint8_t occupation[25];//职业
    uint8_t hometown[50];//家乡
    uint8_t location[50];//当前所在地
    uint8_t school[50];//毕业院校
    uint8_t company[50];//公司名称
    uint8_t label[50];//标签，对应user_lable表
    uint8_t personality[100];//个性签名
    uint8_t head_portrait_path[100];//头像地址
    uint8_t user_id[50];//用户id
    uint64_t create_date_time;//修改时间
}StruPersonalDataMsgBodyReq, *PStruPersonalDataMsgBodyReq;


/********** 登录消息体请求数据通用结构 **********/
typedef struct _tagLoginMsgBodyReq
{
    uint8_t authorization_type;//授权类型[0表示手动注册-及分配的id，1表示电话号码，2表示邮箱, 3表示QQ，4表示微信，5表示微博]
    uint8_t authorization_uuid[50];//授权uuid
    uint8_t credential[50];//授权凭证
    uint32_t key_token_survival_time;//消息交互的钥匙有效期
    uint64_t login_ip;//登录设备ip
    uint64_t login_datetime;//登录时间
    uint8_t verification_code[25];//验证码
}LoginMsgBodyReq, *PLoginMsgBodyReq;


/********** 登录消息体回复数据通用结构 **********/
typedef struct _tagLoginMsgBodyReply
{
    uint8_t authorization_type;//授权类型[0表示手动注册-及分配的id，1表示电话号码，2表示邮箱, 3表示QQ，4表示微信，5表示微博]
    uint8_t authorization_uuid[50];//授权uuid
    uint8_t credential[50];//授权凭证
    uint64_t login_ip;//登录设备ip
    uint64_t login_datetime;//登录时间
    uint8_t verification_code[25];//验证码
}LoginMsgBodyReply, *PLoginMsgBodyReply;


/********** 登出消息体请求数据通用结构 **********/
typedef struct _tagLogoutMsgBodyReq
{
    uint64_t user_id;//用户id[手动退出会销毁之前协议的key_token]
    uint64_t logout_datetime;//登出时间
}LogoutMSgBodyReq, *PLogoutMsgBodyReq;


/********** 排行消息体数据通用结构 **********/
typedef struct _tagRealtimeRankMsgBody
{
    uint8_t rank_type;//请求排行的类型
}RealtimeRankMsgBody, *PRealtimeRankMsgBody;


/********** 搜索消息体数据通用结构 **********/
typedef struct _tagSearchMsgBody
{
    uint8_t search_condition[250];//查询条件
    uint8_t filter_condition[50];//过滤条件[系统默认设定的可选项]
    uint64_t search_datetime;//查询时间[服务端自行获取，不需要前端传递]
}SearchMsgBody, *PSearchMsgBody;


/********** 查询资料消息体数据通用结构 **********/
typedef struct _tagQueryUserDataMsgBody
{
    uint64_t user_id;//用户id
}QueryUserDataMsgBody, *PQueryUserDataMsgBody;


/********** 查询历史消息体数据通用结构 **********/
typedef struct _tagQueryHistoryMsgBody
{
    uint8_t query_type;//查询类型[0表示搜索历史，1表示好友添加历史，2表示拉黑历史，3表示单对单聊天历史，4表示单对多聊天历史]
    uint64_t history_datetime;//欲查询的历史时间
	union CommonCondition//通过条件结构
	{
		union AddFriendHistory
		{
            uint64_t friend_id;//好友id
		};

		union CTCChatHistory
		{
            uint64_t user_id;//聊天对象id
		};
	
		union CTGChatHistory
		{
            uint64_t room_id;//房间id
		};
	};

}QueryHistoryMsgBody, *PQueryHistoryMsgBody;


/********** 添加好友消息体数据通用结构 **********/
typedef struct _tagAddFriendMsgBody
{
    uint64_t friend_id;//待添加朋友的用户id
    uint8_t remarks_data[100];//备注信息，转发给待添加朋友
    uint8_t group_name[50]; //添加朋友至分组的名称
    uint8_t remarks_name[50];//对朋友的备注
    uint64_t add_datetime;//添加时间
}AddFriendMsgBody, *PAddFriendMsgBody;


/********** 删除好友消息体数据通用结构 **********/
typedef struct _tagDelFriendMsgBody
{
    uint64_t friend_id;//待删除朋友的用户id
    uint64_t del_datetime;//删除时间[服务端自行获取，不需要前端传递]
}DelFriendMsgBody, *PDelFriendMsgBody;


/********** 拉黑好友消息体数据通用结构 **********/
typedef struct _tagBlackFriendMsgBody
{
    uint64_t friend_id;//待拉黑朋友的用户id
    uint64_t del_datetime;//拉黑时间[服务端自行获取，不需要前端传递]
}BlackFriendMsgBody, *PBlackFriendMsgBody;


/********** 好友分组移动消息体数据通用结构 **********/
typedef struct _tagMoveFriendGroupMsgBody
{
    uint64_t friend_id;//待移动好友id
    uint8_t group_name[25];//移动分组名称
    uint64_t move_datetime;//移动事件[服务端自行获取，不需要前端传递]
}MoveFriendGroupMsgBody, *PMoveFriendGroupMsgBody;


/********** 聊天消息体数据通用结构 **********/
typedef struct _tagChatMsgBody
{
    uint64_t receiver_id;//待接收方id[如果是用户id表示单对单，如果是房间id表示群聊]
    uint64_t send_datetime;//发送的时间
    uint8_t data_type;//消息类型[0表示文字，1表示图标，2表示图片，3表示视频]
    uint64_t sequence;//序列号[如果发送的消息一次无法全部发送，需要多次拆包，该字段用于对数据进行排序处理]
    uint8_t* data;//实际数据
}ChatMsgBody,*PChatMsgBody;


/********** 创建房间消息体数据通用结构 **********/
typedef struct _tagCreateRoomMsgBody
{
    uint8_t room_label[50];//房间标签
    uint8_t room_type[50];//房间类型[系统默认设定的可选项]
    uint8_t room_name[50];//房间名称
    uint8_t room_password[50];//房间密码
    uint64_t create_datetime;//创建时间[服务端自行获取，不需要前端传递]
    uint64_t survival_time;//存活时间[用户的等级越高存活的时间越长;　用户亦可通过增加活跃度延长房间的寿命]
}CreateRoomMsgBody, *PCreateRoomMsgBody;



/********** 房间删除消息体数据通用结构 **********/
typedef struct _tagDelRoomMsgBody
{
    uint64_t room_id;//待删除的房间id
    uint64_t del_datetime;//删除时间
}DelRoomMsgBody, *PDelRoomMsgBody;


/********** 加入房间消息体数据通用结构 **********/
typedef struct _tagEnterRoomMsgBody
{

    uint64_t room_id;//待进入的房间id
    uint64_t enter_datetime;//进入时间
}EnterRoomMsgBody, *PEnterRoomMsgBody;


/********** 退出房间消息体数据通用结构 **********/
typedef struct _tagExitRoomMsgBody
{
    uint64_t room_id;//待进入的房间id
    uint64_t exit_datetime;//进入时间
}ExitRoomMsgBody, *PExitRoomMsgBody;





/*******************************************/
/*******************************************/
/*******************************************/
/*******************************************/
/*******************************************/





#endif













