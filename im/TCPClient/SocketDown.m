//
//  SocketDown.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-21.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "SocketDown.h"
#import  "AsyncSocket.h"
#import "Photo.h"
#import "NSData+Base64.h"
#import "staic.h"
#import "Change.h"
#import "Time.h"
#import <AVFoundation/AVFoundation.h>

#define A_RECEIVE 800


@implementation SocketDown
-(void)sendImageviewToServer:(UIImage *)image Nm:(NSString *)name playerID:(uint64_t)playerid
{
    socket=[[AsyncSocket alloc]init];
    socket.delegate=self;
    [socket connectToHost:@"192.168.3.5" onPort:22001 error:nil];
    [self pastImagedata:image dataNm:name playerid:playerid];
  
}
-(void)pastImagedata:(UIImage *)Image dataNm:(NSString *)Iname playerid:(uint64_t)playerid
{  
    NSData *imagedata=[NSData dataWithData:UIImageJPEGRepresentation(Image, 0.75)];
 
   NSLog(@"imagedata is --------%s,%d",(char *)imagedata,[imagedata length]);
    Change *change=[[Change alloc]init];
    NSMutableArray *imageDataArray=[[NSMutableArray alloc]init];
    NSLog(@"imagedata is %d",[imagedata length]);
    int lastValue=0;
    for(int i = 0; i<=[imagedata length]-A_RECEIVE;i+=A_RECEIVE)
    {
     
        lastValue=i+A_RECEIVE;
        NSString *rangestr=[NSString stringWithFormat:@"%i,%i",i,A_RECEIVE];
        NSData *subData=[imagedata subdataWithRange:NSRangeFromString(rangestr)];
        [imageDataArray addObject:subData];
    }
    NSString *range=[NSString stringWithFormat:@"%i,%i",lastValue,[imagedata length]%A_RECEIVE];
    NSData *data=[imagedata subdataWithRange:NSRangeFromString(range)];
    [imageDataArray addObject:data];
    
    change.imagecount=[imageDataArray count];
    NSMutableDictionary *dic=[NSMutableDictionary dictionary];
    [dic setObject:imagedata forKey:@"Arraycount"];
    [[NSNotificationCenter defaultCenter]postNotificationName:@"count" object:nil userInfo:dic];
    
    for(int j=0;j<[imageDataArray count];j++)
    {
    struct message _image;
    _image.msgcmd=20001;
    _image.group=1;
    _image.playerID=playerid;
        strcpy(_image.playerNm, [Iname cStringUsingEncoding:NSASCIIStringEncoding]);
    _image.type=2;
    _image.strlen=[[imageDataArray objectAtIndex:j] length];
   memcpy(_image.strmsg,[[imageDataArray objectAtIndex:j] bytes], _image.strlen);
   _image.datalength=[imagedata length];
   _image.dataCount=[imageDataArray count];
        

    
   

   struct senduser _imagesenduser;
   _imagesenduser.subcmd =2007;
    _imagesenduser.length=sizeof(_image)-sizeof(_image.strmsg)+_image.strlen;
    _imagesenduser.usercount=1;
    strcpy(_imagesenduser.sendNm, [Iname cStringUsingEncoding:NSASCIIStringEncoding]);
    
    
    
    struct header _imagehead;
    _imagehead.flag=0;
        _imagehead.len= sizeof(_imagesenduser)+_imagesenduser.length;
    _imagehead.cmd= 10006;
    
    
    //NSLog(@"header is ----%d",_imagehead.len);
    
        NSOutputStream *stream1=[[NSOutputStream alloc]initToMemory];
        [stream1 setDelegate:self];
        [stream1 scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [stream1 open];

    [stream1 write:(uint8_t *)&_imagehead maxLength:sizeof(_imagehead)];
    [stream1 write:(uint8_t *)&_imagesenduser maxLength:sizeof(_imagesenduser)];
    [stream1 write:(uint8_t *)&_image maxLength:_imagesenduser.length];
   NSData *data1=[stream1 propertyForKey:NSStreamDataWrittenToMemoryStreamKey];
        static int totalLength = 0;
        totalLength += [data1 length];
       // NSLog(@"totallenth is %d",totalLength);

       // [socket readDataToLength:953 withTimeout:5 tag:j];
    [socket writeData:data1  withTimeout:-1 tag:111];
    

     }

}
-(void)sendVoiceToServer:(NSString *)filepath ImageNm:(NSString *)imagename playerID:(int8_t)playerid
{
    socket=[[AsyncSocket alloc]init];
    socket.delegate=self;
    [socket connectToHost:@"192.168.3.5" onPort:22001 error:nil];
  

   NSData *recodedata=[NSData dataWithContentsOfURL:[NSURL fileURLWithPath:filepath]];
    
   NSLog(@"filepath is %d",[recodedata length]);

    NSLog(@"recodedata is --------%s",(char *)recodedata);
    NSMutableArray *voiceDataArray=[[NSMutableArray alloc]init];
    NSLog(@"imagedata is %d",[recodedata length]);
    int lastValue=0;
    for(int i = 0; i<=[recodedata length]-A_RECEIVE;i+=A_RECEIVE)
    {
        
        lastValue=i+A_RECEIVE;
        NSString *rangestr=[NSString stringWithFormat:@"%i,%i",i,A_RECEIVE];
        NSData *subData=[recodedata subdataWithRange:NSRangeFromString(rangestr)];
        [voiceDataArray addObject:subData];
    }
    NSString *range=[NSString stringWithFormat:@"%i,%i",lastValue,[recodedata length]%A_RECEIVE];
    NSData *data=[recodedata subdataWithRange:NSRangeFromString(range)];
    [voiceDataArray addObject:data];
    

   
    
    for(int j=0;j<[voiceDataArray count];j++)
    {
        struct message _image;
        _image.msgcmd=20001;
        _image.group=1;
        _image.playerID=playerid;
        strcpy(_image.playerNm, [imagename cStringUsingEncoding:NSASCIIStringEncoding]);
        _image.type=3;
        _image.strlen=[[voiceDataArray objectAtIndex:j] length];
        memcpy(_image.strmsg,[[voiceDataArray objectAtIndex:j] bytes], _image.strlen);
        _image.datalength=[recodedata length];
        _image.dataCount=[voiceDataArray count];
        
        
        
        
        
        struct senduser _imagesenduser;
        _imagesenduser.subcmd =2007;
        _imagesenduser.length=sizeof(_image)-sizeof(_image.strmsg)+_image.strlen;
        _imagesenduser.usercount=1;
        strcpy(_imagesenduser.sendNm, [imagename cStringUsingEncoding:NSASCIIStringEncoding]);
        
        
        
        struct header _imagehead;
        _imagehead.flag=0;
        _imagehead.len= sizeof(_imagesenduser)+_imagesenduser.length;
        _imagehead.cmd= 10006;
        
        
        //NSLog(@"header is ----%d",_imagehead.len);
        
        NSOutputStream *stream1=[[NSOutputStream alloc]initToMemory];
        [stream1 setDelegate:self];
        [stream1 scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [stream1 open];
        
        [stream1 write:(uint8_t *)&_imagehead maxLength:sizeof(_imagehead)];
        [stream1 write:(uint8_t *)&_imagesenduser maxLength:sizeof(_imagesenduser)];
        [stream1 write:(uint8_t *)&_image maxLength:_imagesenduser.length];
        NSData *data1=[stream1 propertyForKey:NSStreamDataWrittenToMemoryStreamKey];
        static int totalLength = 0;
        totalLength += [data1 length];
     
        [socket writeData:data1  withTimeout:-1 tag:111];
        
        
    }
}
@end
