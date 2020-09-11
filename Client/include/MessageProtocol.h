#ifndef  MESSAGEPROTOCOL_H_
#define  MESSAGEPROTOCOL_H_
enum CMD_TYPE{
    CD,
    LS,
    PUTSFILE,
    PUTSDIR,
    GETSFILE,
    GETSDIR,
    PWD,
    MKDIR,
    REMOVE,
    CMDERROR,
    CMDEOF,
    CMDQUIT,
    CMDLOGIN,
};
typedef struct Message {
    typedef unsigned int message_size_type;
    typedef CMD_TYPE              cmd_type;
    message_size_type                  len;
    cmd_type                           cmd;
    char                         data[128];
}Msg_t;
int SubCmdString( char *total_str, const char *sub_str);

int ReceiveFixedSized(int client_sfd, void *buffer, int recv_sz);
CMD_TYPE GetCmdType(const char *cmd);
#endif
