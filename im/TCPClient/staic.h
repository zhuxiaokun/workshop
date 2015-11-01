//
//  staic.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-7.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <Foundation/Foundation.h>

#pragma pack(push, 1)

//包头
struct header{
    int flag:1;
    int len :15;
    int cmd :16;
};
struct sendheader{
    int sendflag:1;
    int sendlen :15;
    int sendcmd :16;
};

//登陆信息

struct login{
    char username[62];
    char password[62];
};
struct intnational{
    uint64_t userid;
    char userNm[62];
    
};
struct loginRsp {
    int8_t result;
    int64_t playerId;
};
struct user{
    int16_t count;
};
struct senduser{
    int16_t subcmd;
    int32_t length;
    int16_t  usercount;
    char sendNm[62];
};

//传送信息的内容

struct message{
    int16_t msgcmd;
    int16_t group;
    int64_t playerID;
    char playerNm[62];
    int8_t type  ;
    int32_t strlen;
    char strmsg[800];
    int32_t datalength;
    int32_t dataCount;

   // NSMutableData *data;
};

struct sendimage{
    int16_t subimagecmd;
    int32_t imagelength;
    int16_t imageusercount;
    char sendimageNm[62];
    
};

#pragma pack(pop)