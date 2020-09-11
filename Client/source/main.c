#include <func.h>
#include "../include/MessageProtocol.h"
#include "../include/Epoll.h"
#include "../include/ClientFunc.h"
int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 3);
    int my_sfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_sockaddr;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_in));
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(atoi(argv[2]));
    server_sockaddr.sin_addr.s_addr = inet_addr(argv[1]);

    int connect_ret = connect(my_sfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr));
    ERROR_CHECK(connect_ret, -1, "connect");

    int login_ret;
    char account[32];

    memset(account, 0, sizeof(account));
    printf("account: ");
    fflush(stdout);
    int read_sz = read(STDIN_FILENO, account, sizeof(account));
    account[read_sz - 1] = 0;
    

    login_ret = LoginVerify(my_sfd, account);
    if(-1 == login_ret)
        return -1;

    printf("Login Success!\n");

    int epoll_fd;
    int epoll_build_ret = EpollBuild(&epoll_fd);
    ERROR_CHECK(epoll_build_ret, -1, "EpollBuild");

    struct epoll_event epoll_event;
    memset(&epoll_event, 0, sizeof(epoll_event));

    EpollAddListener(epoll_fd, &epoll_event, STDIN_FILENO, EPOLLIN);
    EpollAddListener(epoll_fd, &epoll_event, my_sfd, EPOLLIN);

    struct epoll_event response_events[2];
    memset(response_events, 0, sizeof(response_events));

    Msg_t msg;



    char buffer[128];
    for( ;  ; ) {
        int epoll_ret = epoll_wait(epoll_fd, response_events, 2, -1);
        ERROR_CHECK(epoll_ret, -1, "epoll_wait");
        for(int i = 0; i != epoll_ret; ++i) {
            memset(&msg, 0, sizeof(msg));
            if(response_events[i].data.fd == STDIN_FILENO) {
                memset(buffer, 0, sizeof(buffer));
                memset(&msg, 0, sizeof(Msg_t));
                int read_sz = read(STDIN_FILENO, buffer, sizeof(buffer));
                buffer[read_sz-1] = 0;
                if(strcmp(buffer, "quit") == 0) {
                    msg.cmd = CMDQUIT;
                    msg.len = 0;
                    send(my_sfd, &msg, 8+msg.len, 0);
                    close(my_sfd);
                    return 0;
                }

                msg.cmd = GetCmdType(buffer);
                if(msg.cmd == CMDERROR)
                    continue;
                SubCmdString(msg.data, buffer);
                msg.len = strlen(msg.data);
                int send_ret = -1;
                send_ret  = send(my_sfd, &msg, 8+msg.len, 0);                
                if(send_ret == -1) {
#ifdef DEBUG
                    printf("server has closed!\n");
#endif
                    break;
                }
                switch(msg.cmd) {
                case LS:
                    GetLsResult(my_sfd);
                    break;
                case PUTSFILE:
                    PutsFileToServer(my_sfd, msg.data);
                    break;
                case PUTSDIR:
                    PutsDirToServer(my_sfd, msg.data);
                    break;
                case GETSFILE:
                    GetsContinueFileFromServer(my_sfd, msg.data);
                    break;
                case GETSDIR:
                    GetsDirFromServer(my_sfd, msg.data);
                    break;
                case PWD:
                    GetPwdResult(my_sfd);
                case MKDIR:
                    break; 
                case REMOVE:
                    break;
                case CD:
                    GetCdResult(my_sfd);
                    break;
                default:
                    break;

                }

            } else if(response_events[i].data.fd == my_sfd) {

            }
        }

    }
} 


