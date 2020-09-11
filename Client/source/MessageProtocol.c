#include "../include/MessageProtocol.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
int ReceiveFixedSized(int client_sfd, void *buffer, int recv_sz) {
    char *p = (char *)buffer;
    int total = 0,ret;
    while(total < recv_sz) {
        ret = recv(client_sfd, p+total, recv_sz - total, 0);
        total+=ret;
    }
    return 0;
}

int SubCmdString( char *total_str, const char *sub_str) {
    size_t i = 0;
    size_t sz = strlen(sub_str);
    for(; i!=sz; ++i) {
        if(sub_str[i]== ' ') {
            while(i+1 != sz && sub_str[i+1]== ' ')
                ++i;
            ++i;
            break;
        }
    }
    if(i >= sz)
        return 0;

    strcpy(total_str, sub_str+i);
    return 0;
}


CMD_TYPE GetCmdType(const char *cmd) {
   if(strcmp("ls", cmd) == 0) {
        return LS;
    }       if(strcmp("pwd", cmd) == 0) {
        return PWD;
    }

    int i;
    int cmd_sz = strlen(cmd);
    for(i = 0; i!=cmd_sz; ++i) {
        if(cmd[i] == ' '|| cmd[i] == 0)
            break;
    }
    char tmp_cmd[64] = {0};
    strncpy(tmp_cmd, cmd, (size_t)i);
     if(strcmp("cd", tmp_cmd) == 0) {
        return CD;
    } 
    if(strcmp("putsfile", tmp_cmd) == 0) {
        return PUTSFILE;
    }  if(strcmp("putsdir", tmp_cmd) == 0) {
        return PUTSDIR;
    }  if(strcmp("getsfile", tmp_cmd) == 0) {
        return GETSFILE;
    }
    if(strcmp("getsdir", tmp_cmd) == 0) {
        return GETSDIR;
    }
    if(strcmp("mkdir", tmp_cmd) == 0)
    {
        return MKDIR;
    }
    if(strcmp("remove", tmp_cmd) == 0) {
        return REMOVE;
    }
    return CMDERROR;
}
