//
//  ImData.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-30.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "ImData.h"
#import "staic.h"
#import "Time.h"
#import "AsyncSocket.h"
#import "VoiceRecorderBaseVC.h"

#define DATA_LENGTH 883
#define HEAD_LENGTH 91

@implementation ImData
@synthesize isFirstFourBytes,dataBuffer;
-(void)dealImageData:(NSData *)data socket:(AsyncSocket *)socket tag:(long)tag
{
  
    struct header head;
    struct message _message;
    
#pragma mark - 数据解析区
    
    if(!isFirstFourBytes){
    
    inputstream=[[NSInputStream alloc]initWithData:data];
    [inputstream setDelegate:self];
    [inputstream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputstream open];

     [inputstream read:(uint8_t *)&head maxLength:sizeof(head)];
     [inputstream read:(uint8_t *)&_message maxLength:head.len];
        NSLog(@"datalength is %d",_message.datalength);
            
        dataLength = _message.datalength + HEAD_LENGTH * _message.dataCount;
        DataCount=_message.dataCount;
        type=_message.type;
        NSLog(@"datalength is ---%d",dataLength);
       isFirstFourBytes=YES;
    }
    
    if(!dataBuffer)
    {
        dataBuffer=[[NSMutableData alloc]init];
    }
        
        if(isFirstFourBytes){
        [dataBuffer appendData:data];
            //NSLog(@"dataBuffer is %d",[dataBuffer length]);
        }
    if([dataBuffer length] == dataLength)
    {
         NSLog(@"databuffer is %s",(char *)dataBuffer);
        [self manageData:dataBuffer];
        dataBuffer = nil;
        isFirstFourBytes=NO;
    }
    [socket readDataWithTimeout:-1 tag:1];
}

-(void)manageData:(NSData *)data
{
    struct header head;
    struct message _message;
   
    inputstream=[[NSInputStream alloc]initWithData:data];
    [inputstream setDelegate:self];
    [inputstream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputstream open];
   
    Recivedata=[[NSMutableData alloc]init];
    for(int  i=0;i<DataCount;i++)
    {
        [inputstream read:(uint8_t *)&head maxLength:sizeof(head)];
        [inputstream read:(uint8_t *)&_message maxLength:head.len];
        [Recivedata appendBytes:_message.strmsg length:_message.strlen];
    }
    
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if(type==2){
        
        [Recivedata retain];
        
        NSLog(@"imagedata is %s,%d",(char *)Recivedata,Recivedata.length);
        UIImage *image=[UIImage imageWithData: Recivedata];
        [dict setObject:image forKey:@"image"];
        [dict setObject:[Time getCurrentTime] forKey:@"time"];
        [dict setObject:[NSString stringWithCString:_message.playerNm encoding:NSUTF8StringEncoding] forKey:@"senter"];
    } else if(type==3){
        NSError *err;
        NSData *_recodedata=[NSData dataWithData:Recivedata];
        NSLog(@"str is -----%s",(char *)Recivedata);
        NSLog(@"recode is long %d",[_recodedata length]);
        NSString  *docDirPath=[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
        NSString *filePath=[NSString stringWithFormat:@"%@/%@.caf",docDirPath,[Time getCurrentTime]];
       [Recivedata writeToFile:filePath atomically:YES];
        NSURL *fileurl=[NSURL fileURLWithPath:filePath];
       avaudio=[[AVAudioPlayer alloc]initWithContentsOfURL:fileurl error:&err];
       if(avaudio ==nil)
        {
            avaudio=[[AVAudioPlayer alloc]initWithData:Recivedata error:nil];
         NSLog(@"err is %@",[err description]);
        }
        [dict setObject:filePath forKey:@"voice"];
        [dict setObject:[Time getCurrentTime] forKey:@"time"];
        [dict setObject:[NSString stringWithCString:_message.playerNm encoding:NSUTF8StringEncoding] forKey:@"senter"];
       
    }
    [[NSNotificationCenter defaultCenter]postNotificationName:@"dataNotification" object:nil userInfo:dict];
    //[inputstream release];
    //[dataBuffer release];
    //[Recivedata release];
}


@end;
