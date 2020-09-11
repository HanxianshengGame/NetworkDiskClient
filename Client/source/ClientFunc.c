#include "../include/ClientFunc.h"
#include <func.h>
#include <crypt.h>

void CheckSendRet(int send_ret, int des) {
    if(send_ret != des) {
        printf("Server has closed!");
        return;
    }
}
int LoginVerify(int my_sfd, const char *account) {
   
   Msg_t msg;
   Msg_t *pMsg = &msg;
   memset(pMsg, 0, sizeof(Msg_t));
   pMsg->len = strlen(account);
   pMsg->cmd = CMDLOGIN;
   strncpy(pMsg->data, account, pMsg->len);
   int send_sz = send(my_sfd, pMsg, 8+pMsg->len, MSG_WAITALL);
   CheckSendRet(send_sz, 8+pMsg->len);
   printf("I send my account: %s!\n", account);

   //接收到salt 盐值
   memset(pMsg, 0, sizeof(Msg_t));
   ReceiveFixedSized(my_sfd, &pMsg->len, 4);
   ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
   ReceiveFixedSized(my_sfd,  pMsg->data, pMsg->len);
   printf("I recv my salt!\n");
   if(pMsg->cmd == CMDERROR) {
       printf("accout no active!\n");
       return -1;
   }
   
   char salt[32];
   memset(salt, 0, sizeof(salt));
   strncpy(salt, pMsg->data, pMsg->len);
   char *passwd = getpass("enter password: ");
   memset(pMsg, 0, sizeof(Msg_t));
   strcpy(pMsg->data, crypt(passwd,salt));
   pMsg->len = strlen(pMsg->data);
   pMsg->cmd = CMDLOGIN;
   send_sz = send(my_sfd, pMsg, 8 + pMsg->len, MSG_WAITALL);
   CheckSendRet(send_sz, 8 + pMsg->len);
   
   //判断是否通过验证
   memset(pMsg, 0, sizeof(Msg_t));
   ReceiveFixedSized(my_sfd, &pMsg->len, 4);
   ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
   ReceiveFixedSized(my_sfd,  pMsg->data, pMsg->len);
   if(pMsg->cmd == CMDLOGIN) {
       printf("passwd access!\n");
       return 0;
   } 
   printf("passwd failure!\n");
   return -1;
   
}


int GetPwdResult(int my_sfd) {
    
    Msg_t msg;
    memset(&msg, 0 ,sizeof(msg));
    recv(my_sfd, &msg, sizeof(msg), MSG_WAITALL);
    printf("%s", msg.data);
    return 0;
}
int GetCdResult(int my_sfd) {
    Msg_t msg;
    while(1) {
        memset(&msg, 0, sizeof(msg));
        recv(my_sfd, &msg, sizeof(Msg_t), MSG_WAITALL);
        if(msg.cmd == CMDEOF) {
            break;
        }
        printf("%s", msg.data);
    }
    return 0;

}
int GetLsResult(int my_sfd) {
    Msg_t msg;
    while(1) {
        memset(&msg, 0, sizeof(msg));
        recv(my_sfd, &msg, sizeof(Msg_t), MSG_WAITALL);
        if(msg.cmd == CMDEOF) {
            break;
        }
        printf("%s", msg.data);
    }
    return 0;
}

int GetsContinueFileFromServer(int my_sfd, const char *file_name) {
     //请求的文件不在，暂不考虑
    
    Msg_t msg;
    Msg_t *pMsg = &msg;
    memset(pMsg, 0, sizeof(Msg_t));
    int newfd =  open(file_name,O_CREAT|O_RDWR,0666);
    struct stat file_stat;
    fstat(newfd,&file_stat); 
    lseek(newfd, file_stat.st_size-1, SEEK_SET);
    msg.len = 4;
    msg.cmd = GETSFILE;
    memcpy(msg.data, &file_stat.st_size, 4);    
    send(my_sfd, &msg, 8+msg.len, MSG_WAITALL);
#ifdef DEBUG
    printf("this file: %s cur_size: %d\n",file_name, *(int *)msg.data);
#endif

    memset(pMsg, 0, sizeof(Msg_t));
    ReceiveFixedSized(my_sfd, &pMsg->len, 4) ;
    ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
    ReceiveFixedSized(my_sfd, pMsg->data, pMsg->len);

    int des_file_sz = 0;
    memcpy(&des_file_sz, pMsg->data, pMsg->len);
#ifdef DEBUG
    printf("transfer_file size: %d\n", des_file_sz);
#endif

    int cur_file_sz = 0;
    while(cur_file_sz < des_file_sz) {
        memset(pMsg, 0, sizeof(Msg_t));
        ReceiveFixedSized(my_sfd, &pMsg->len, 4) ;
        ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
        ReceiveFixedSized(my_sfd, pMsg->data, pMsg->len);
        printf("trans_data: %s\n", pMsg->data);
        int write_sz = write(newfd, pMsg->data, pMsg->len);
        cur_file_sz += write_sz;
    }
    
#ifdef DEBUG
    printf("transfer file : %s over!\n", file_name);
#endif
    close(newfd);
    return 0;

}

int GetsFileFromServer(int my_sfd, const char *file_name) {
    //请求的文件不在，暂不考虑
    Msg_t msg;
    Msg_t *pMsg = &msg;
    memset(pMsg, 0, sizeof(Msg_t));
    int newfd =  open(file_name,O_CREAT|O_RDWR,0666);

    ReceiveFixedSized(my_sfd, &pMsg->len, 4) ;
    ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
    ReceiveFixedSized(my_sfd, pMsg->data, pMsg->len);

    int des_file_sz = 0;
    memcpy(&des_file_sz, pMsg->data, pMsg->len);
#ifdef DEBUG
    printf("transfer_file size: %d\n", des_file_sz);
#endif

    int cur_file_sz = 0;
    while(cur_file_sz < des_file_sz) {
        memset(pMsg, 0, sizeof(Msg_t));
        ReceiveFixedSized(my_sfd, &pMsg->len, 4) ;
        ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
        ReceiveFixedSized(my_sfd, pMsg->data, pMsg->len);
        int write_sz = write(newfd, pMsg->data, pMsg->len);
        cur_file_sz += write_sz;
    }
#ifdef DEBUG
    printf("transfer file : %s over!\n", file_name);
#endif
    close(newfd);

  
    
    return 0;
}

int GetsDirFromServer(int my_sfd, const char *dir_name) {
    Msg_t msg;
    memset(&msg, 0, sizeof(Msg_t));
    Msg_t *pMsg = &msg;
    while(1) {
        memset(pMsg, 0, sizeof(Msg_t));
        ReceiveFixedSized(my_sfd, &pMsg->len, 4) ;
        ReceiveFixedSized(my_sfd, &pMsg->cmd, 4);
        ReceiveFixedSized(my_sfd, pMsg->data, pMsg->len);
#ifdef DEBUG
        printf("transfer content %s\n", pMsg->data);
#endif
        if(pMsg->cmd == PUTSDIR) {
#ifdef DEBUG
            printf("mkdir: %s\n", pMsg->data);
#endif
            mkdir(pMsg->data, 0777);
        } else if(pMsg->cmd == PUTSFILE){

#ifdef DEBUG
            printf("mkfile: %s\n", pMsg->data);
#endif
            GetsFileFromServer(my_sfd, pMsg->data);
        } else {
            printf("GetsDir over!\n");
            break;
        }         

    }
    return 0;


    return 0;
}
int PutsDirToServer(int my_sfd, const char *dir_name) {
    Msg_t msg;
    Msg_t *pMsg = &msg;
    PutsDirToServerCore(my_sfd, dir_name, pMsg);
    pMsg->len = 0;
    pMsg->cmd = CMDEOF;
    int send_sz = send(my_sfd, pMsg, 8, 0);
    if(send_sz == -1) {
#ifdef DEBUG
        printf("server has closed!\n");
#endif
        exit(-1);
    }
    else {
#ifdef DEBUG
        printf("Puts Dir %s over!\n", dir_name);
#endif
    }
    return 0;
}
int PutsDirToServerCore(int my_sfd, const char *dir_name, Msg_t *pMsg) {
    memset(pMsg, 0, sizeof(Msg_t));
    pMsg->len = strlen(dir_name);
    pMsg->cmd = PUTSDIR;
    memcpy(pMsg->data, dir_name, pMsg->len);
#ifdef DEBUG
    printf("transfer dir: %s\n", pMsg->data);
#endif
    int send_sz = send(my_sfd, pMsg, 8+pMsg->len, 0);
    if(send_sz == -1){
#ifdef DEBUG
        printf("server has closed!\n");
#endif
        exit(-1);
    }
    
    struct dirent *cur_dirent = {0};
    DIR *dirp = opendir(dir_name);
    
    char new_dir_path[128];
    char new_file_path[128];
    while((cur_dirent = readdir(dirp)) != NULL) {
        if(strcmp("..", cur_dirent->d_name) == 0 || strcmp(".", cur_dirent->d_name) == 0) {
            continue;
        }
        if(cur_dirent->d_type == 4) {
            memset(new_dir_path, 0, sizeof(new_dir_path));
            sprintf(new_dir_path, "%s/%s", dir_name, cur_dirent->d_name);
            PutsDirToServerCore(my_sfd, new_dir_path, pMsg);
        } else {
            memset(new_file_path, 0 ,sizeof(new_file_path));
            sprintf(new_file_path, "%s/%s", dir_name, cur_dirent->d_name);
#ifdef DEBUG
            printf("transfer child file: %s\n", new_file_path);
#endif
            memset(pMsg, 0, sizeof(Msg_t));
            pMsg->len = strlen(new_file_path);
            pMsg->cmd = PUTSFILE;

            memcpy(pMsg->data, new_file_path, pMsg->len);
            send_sz = send(my_sfd, pMsg, 8+pMsg->len, 0);
            if(send_sz == -1){
#ifdef DEBUG
                printf("server has closed!\n");
#endif
                exit(-1);
            }
            PutsFileToServer(my_sfd, new_file_path);
        }
    }
    closedir(dirp);
    return 0;
}
//根据my_sfd，文件名（相对路径），以及火车发送文件
//注意点：文件名是命令截取的 文件名，若putsfile  main.c 则传入 main.c 即可，
//先发文件大小，再发文件
int PutsFileToServer(int my_sfd, const char *file_name) {
    Msg_t msg;
    Msg_t *pMsg = &msg;
    memset(pMsg, 0 ,sizeof(Msg_t));
    
    struct stat stat_buf;
    memset(&stat_buf, 0, sizeof(stat_buf));
#ifdef DEBUG
    printf("filename= %s, len = %d\n",file_name, (int)strlen(file_name) );
#endif
    stat(file_name, &stat_buf);
    pMsg->cmd = PUTSFILE;
    pMsg->len = 4;
    int file_sz = stat_buf.st_size;
    memcpy(pMsg->data, &file_sz, pMsg->len);
#ifdef DEBUG
    printf("file_sz = %d\n", file_sz);
#endif
    int send_sz = send(my_sfd, pMsg, 8+pMsg->len, 0);
    if(send_sz == -1) {
        printf("server has closed !\n");
        exit(-1);
    }
    
    int cur_send_sz = 0;
    int fdr = open(file_name, O_RDWR, 0666);
    int read_sz = 0;
    while(cur_send_sz < file_sz) {
        read_sz = read(fdr, pMsg->data, sizeof(pMsg->data));
        pMsg->len = read_sz;
        pMsg->cmd = PUTSFILE;
        send_sz = send(my_sfd, pMsg, 8+pMsg->len, 0);
        
        if(send_sz == -1) {
            printf("server has closed!\n");
            exit(-1);
        }
        cur_send_sz += (send_sz - 8);
    }
    
    printf("send file: %s over!\n", file_name);
    
    close(fdr);
    return 0;
}


















