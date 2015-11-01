//
//  Time.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-14.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "Time.h"

@implementation Time

//*********获得时间的方法************//

+(NSString *)getCurrentTime{
    NSDate *nowUTC=[NSDate date];
    NSDateFormatter *dategoematter=[[NSDateFormatter alloc]init];
    [dategoematter setTimeZone:[NSTimeZone localTimeZone]];
    [dategoematter setTimeStyle:NSDateFormatterMediumStyle];
    [dategoematter setDateStyle:NSDateFormatterMediumStyle];
    return  [[dategoematter stringFromDate:nowUTC] autorelease];
}
@end
