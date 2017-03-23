//
//  SocketClient.c
//  skynetClient_OC
//
//  Created by mengyun on 2017/2/27.
//  Copyright © 2017年 mengyun. All rights reserved.
//

//
//  SocketClient.c
//  skynetClient
//
//  Created by mengyun on 2017/2/27.
//  Copyright © 2017年 mengyun. All rights reserved.
//
#include "SocketClient.h"
#include "socket_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

//char buf[1024] = {0};
struct socket_server* ss;
struct socket_message result;
int fd;
pthread_t pid;
int reply()
{
    //replyV(result.data);
    free(result.data);
    return 0;
}
static void *
_poll(void * ud) {
    //struct socket_server *ss = ud;
    
    for (;;) {
        int type = socket_server_poll(ss, &result, NULL);
        // DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
        switch (type) {
            case SOCKET_EXIT:
                return NULL;
            case SOCKET_DATA:
                printf("SOCKET_DATA message(%" PRIuPTR ") [id=%d] size=%d,data=%s\n",result.opaque,result.id, result.ud,result.data);
                reply();
                break;
            case SOCKET_CLOSE:
                printf("close(%" PRIuPTR ") [id=%d]\n",result.opaque,result.id);
                break;
            case SOCKET_OPEN:
                printf("open(%" PRIuPTR ") [id=%d] %s\n",result.opaque,result.id,result.data);
                break;
            case SOCKET_ERROR:
                printf("error(%" PRIuPTR ") [id=%d]\n",result.opaque,result.id);
                break;
            case SOCKET_ACCEPT:
                printf("accept(%" PRIuPTR ") [id=%d %s] from [%d]\n",result.opaque, result.ud, result.data, result.id);
                socket_server_start(ss, 300, result.ud);
                break;
        }
    }
}

int socket_init(void)
{
    ss = socket_server_create();
    fd = socket_server_connect(ss, 100, "127.0.0.1", 8888);
    //pthread_t pid;
    pthread_create(&pid, NULL, _poll, (void*)ss);
    return 0;
}

int socket_send(char *buf)
{
    //char buf[111] = "12345";
    if (strncmp(buf, "quit", 4) == 0)
        return -1;
    //buf[strlen(buf)-1] = '\n';		// 去除\n
    printf("|%s|\n",buf);
    char* sendbuf = (char*)malloc(sizeof(buf)+1);
    memcpy(sendbuf, buf, strlen(buf)+1);
    socket_server_send(ss, fd, sendbuf, strlen(sendbuf));
    return 0;
}

int socket_close()
{
    socket_server_exit(ss);
    pthread_join(pid, NULL);
    socket_server_release(ss);
    return 0;
}


//int main(void)
//{
//    struct socket_server* ss = socket_server_create();
//    int conn_id = socket_server_connect(ss, 100, "127.0.0.1", 8888);
//
//    pthread_t pid;
//    pthread_create(&pid, NULL, _poll, (void*)ss);
//
//    char buf[1024] = {0};
//    while (fgets(buf, sizeof(buf), stdin) != NULL)
//    {
//        if (strncmp(buf, "quit", 4) == 0)
//            break;
//        buf[strlen(buf)-1] = '\n';		// 去除\n
//        char* sendbuf = (char*)malloc(sizeof(buf)+1);
//        memcpy(sendbuf, buf, strlen(buf)+1);
//        socket_server_send(ss, conn_id, sendbuf, strlen(sendbuf));
//    }
//    socket_server_exit(ss);
//    pthread_join(pid, NULL);
//    socket_server_release(ss);
//    return 0;
//}


