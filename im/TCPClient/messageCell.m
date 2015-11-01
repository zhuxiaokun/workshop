//
//  messageCell.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-14.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "messageCell.h"
#define padd 20;
@implementation messageCell
@synthesize senderandtimelable,messageContentView,bgImageView,voicelable,imageContentView;
- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
    //日期标签
        senderandtimelable=[[UILabel alloc]initWithFrame:CGRectMake(10  , -15, 300, 50)];
//        局中显示
        senderandtimelable.textAlignment=NSTextAlignmentCenter;
        senderandtimelable.font=[UIFont systemFontOfSize:11.0];
//        //文字颜色
        senderandtimelable.textColor=[UIColor lightGrayColor];
        [self.contentView addSubview:senderandtimelable];

    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
