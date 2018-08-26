//
// Created by chenbaiyi on 18-7-7.
//

#ifndef GCMSG_MSGCMDDEF_H
#define GCMSG_MSGCMDDEF_H

//取值宏定义
#define GET_MSG_REQ_TYPE(msg_req_type) ((msg_req_type >> 48) & 0x0000000000001111)

//消息类型宏定义
#define CTC_MSG             0x1
#define CTS_MSG             0x2
#define CTG_MSG             0x3


#define REGISTER_MSG_REPLAY "register_msg_replay"













#endif //GCMSG_MSGCMDDEF_H
