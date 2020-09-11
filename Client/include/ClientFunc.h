#ifndef CLIENTFUNC_H_
#define CLIENTFUNC_H_
#include "MessageProtocol.h"
int LoginVerify(int my_sfd, const char *account);

int PutsFileToServer(int my_sfd, const char *file_name);
int PutsDirToServer(int my_sfd, const char *dir_name);
int PutsDirToServerCore(int my_sfd, const char *dir_name, Msg_t *pMsg);
int GetsFileFromServer(int my_sfd, const char *file_name);
int GetsContinueFileFromServer(int my_sfd, const char *file_name);
int GetsDirFromServer(int my_sfd, const char *dir_name);
int GetLsResult(int my_sfd);
int GetCdResult(int my_sfd);
int GetPwdResult(int my_sfd);
#endif
