//
//  SocketDown.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-21.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncSocket.h"

@interface SocketDown : NSObject<AsyncSocketDelegate,NSStreamDelegate,UIPickerViewDelegate>
{
    AsyncSocket *socket;
}
-(void)sendImageviewToServer:(UIImage *)image Nm:(NSString *)name playerID:(uint64_t) playerid ;
-(void)sendVoiceToServer:(NSString *)filepath ImageNm:(NSString *)imagename playerID:(int8_t) playerid;
@end
