//
//  ViewController.m
//  TCPClient
//
//  Created by qianfeng on 13-4-26.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "ViewController.h"
#import "staic.h"


@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    clientSocket = [[AsyncSocket alloc] initWithDelegate:self];
}

//连接服务端
- (void)connection:(id)sender{
    //连接ip地址为ipfield.text，端口为5678的服务端
    //65536 0~65535  <5000 
    [clientSocket connectToHost:ipField.text onPort:22001 error:nil];
    //什么时候连接成功
}

//调用这个方法就证明连接成功
- (void)onSocket:(AsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port{
    NSLog(@"连接成功");
    //开始监听
    [sock readDataWithTimeout:-1 tag:0];
}

//调用这个方法就证明监听到了数据
- (void)onSocket:(AsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag{
    NSString* str = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
    textView.text = [NSString stringWithFormat:@"%@%@:%@\n",textView.text,sock.connectedHost,str];
    //继续监听
    [sock readDataWithTimeout:-1 tag:0];
}

//发送数据
- (void)sendText:(id)sender{
    //如果连接上的话
    if (clientSocket.isConnected) {
       struct header head;
        head.flag=0;
        head.len=124;
        head.cmd=1004;
        //strcopy
        NSString *str=username.text;
        struct login _login;
       // [str cStringUsingEncoding:NSASCIIStringEncoding];
        strcpy(_login.username,[str cStringUsingEncoding:NSASCIIStringEncoding] );
       
        strcpy(_login.password, [password.text cStringUsingEncoding:NSASCIIStringEncoding]);
      //  NSStream *stream = [[NSOutputStream alloc]init];
        //NSInputStream *inputstream=[[NSInputStream alloc]init];
        NSOutputStream *stream1=[[NSOutputStream alloc]initToMemory];
        [stream1 setDelegate:self];
       // NSOutputStream *outstream=[[NSOutputStream alloc]init];
        [stream1 scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [stream1 open];
       [stream1 write:(uint8_t *)&head maxLength:sizeof(head)];
        [stream1 write:(uint8_t *)&_login maxLength:sizeof(_login)];
        
        //NSData* data = [sendField.text dataUsingEncoding:NSUTF8StringEncoding];
        //[inputstream read:(uint8_t *)&head maxLength:sizeof(head)];
        //[inputstream read:(uint8_t *)&_login maxLength:sizeof(_login)];
        NSData *data1=[stream1 propertyForKey:NSStreamDataWrittenToMemoryStreamKey];
        //发送数据
    
        [clientSocket writeData:data1 withTimeout:-1 tag:0];
        textView.text = [NSString stringWithFormat:@"%@我说:%@\n",textView.text,sendField.text];
        sendField.text = @"";
        
    }
}

//调用这个方法就证明数据发送完成
- (void)onSocket:(AsyncSocket *)sock didWriteDataWithTag:(long)tag{
    //[sock disconnect];
    NSLog(@"发送成功");
}

//断开后会调用这个方法
- (void)onSocketDidDisconnect:(AsyncSocket *)sock{
    
}


- (void)dealloc {
    [username release];
    [password release];
    
    [super dealloc];
}
@end
