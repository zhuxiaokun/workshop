//
//  Change.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-12.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncSocket.h"

@interface Change : NSObject
{
    NSString *name;
    AsyncSocket *_socket;
}

@property (strong ,nonatomic) NSString *name;
@property(strong ,nonatomic)AsyncSocket *_socket;
@property(strong ,nonatomic) NSString *sendMessage;
@property(assign) int8_t playerid;
@property(strong ,nonatomic)UIImageView *headimage;
@property(assign) int imagecount;
@end
