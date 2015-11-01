//
//  ImData.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-30.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncSocket.h"
#import <AVFoundation/AVFoundation.h>

@interface ImData : NSObject<NSStreamDelegate>
{
    uint  remainingToRead;
    NSMutableData *dataBuffer;
    NSInputStream *inputstream;
    NSMutableData *Recivedata;
    int dataLength;
    int DataCount;
    int type;
    AVAudioPlayer *avaudio;
    //NSInputStream *inputstream;
}
@property (assign)BOOL isFirstFourBytes;
@property(nonatomic ,strong)NSMutableData *dataBuffer;
-(void)dealImageData:(NSData *)data socket:(AsyncSocket *) socket tag:(long) tag;
@end
