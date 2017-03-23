//
//  ViewController.m
//  skynetClient_OC
//
//  Created by mengyun on 2017/2/27.
//  Copyright © 2017年 mengyun. All rights reserved.
//

#import "ViewController.h"
//#import "SocketClient.h"
#import "WildPointerChecker/WildPointerChecker.h"

@interface ViewController ()

@property (strong, nonatomic) IBOutlet UITextField *content;
- (IBAction)socketInit:(id)sender;
- (IBAction)socketSend:(id)sender;
@property (strong, nonatomic) IBOutlet UILabel *reciveContext;

@property (nonatomic, assign)struct socket_server* ss;
@property (nonatomic, assign)int socketState; //0,1:to send,2:to recive
@property (strong)NSString *bufData;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    startWildPointerCheck();
    _socketState = 0;
    //_bufData = [NSString stringWithUTF8String:"1234567890987654323213121"];
    //NSLog(@"%@",_bufData);
    //_reciveContext.text = @"123";
    // Do any additional setup after loading the view, typically from a nib.
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)socketInit:(id)sender {
    NSThread *initThread = [[NSThread alloc] initWithTarget:self selector:@selector(socket_init) object:nil];
    // 设置线程的优先级(0.0 - 1.0，1.0最高级)
    initThread.threadPriority = 1;
    // 开启线程
    [initThread start];
}

- (void) receiveData{
    if (_socketState == 2){
        if ([NSThread isMainThread])
        {
            _reciveContext.text = _bufData;
        }
        else
        {
            dispatch_sync(dispatch_get_main_queue(), ^{
                //Update UI in UI thread here
                _reciveContext.text = _bufData;
            });
        }
    }
}

- (IBAction)socketSend:(id)sender {
    _bufData = _content.text;
    _socketState = 1;
    //socket_send([str UTF8String]);
}

- (int) socket_init{
    _ss = socket_server_create();
    int conn_id = socket_server_connect(_ss, 100, "192.168.1.100", 8888);

    NSThread *socketThread = [[NSThread alloc] initWithTarget:self selector:@selector(_poll) object: nil];
    // 设置线程的优先级(0.0 - 1.0，1.0最高级)
    socketThread.threadPriority = 1;
    // 开启线程
    [socketThread start];

    char *buf;
    while (1)
    {
        if (_socketState == 1) {
            _socketState = 0;
            buf = [_bufData UTF8String];
            printf("buf == %s \n",buf);
            if (strncmp(buf, "quit", 4) == 0)
                break;
            //buf[strlen(buf)-1] = '\n';		// 去除\n
            char* sendbuf = (char*)malloc(sizeof(buf));
            memcpy(sendbuf, buf, strlen(buf)+1);
            socket_server_send(_ss, conn_id, sendbuf, strlen(sendbuf)+1);
        }
        
    }
    socket_server_exit(_ss);
    //pthread_join(pid, NULL);
    socket_server_release(_ss);
    return 0;
}


- (void *)_poll {
    //_struct socket_server *ss = ud;
    struct socket_message result;
    for (;;) {
        int type = socket_server_poll(_ss, &result, NULL);
        // DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
        switch (type) {
            case SOCKET_EXIT:
                return NULL;
            case SOCKET_DATA:
                printf("SOCKET_DATA message(%" PRIuPTR ") [id=%d] size=%d,data=%s\n",result.opaque,result.id, result.ud,result.data);
                _socketState = 2;
                result.data[result.ud]='\0';
                printf("[%d__%d]\n",strlen(result.data),result.ud);
                _bufData = [NSString stringWithUTF8String:result.data];
                [self receiveData];
                free(result.data);
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
                socket_server_start(_ss, 300, result.ud);
                break;
        }
    }
}

@end
