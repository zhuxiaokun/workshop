//
//  GroupCell.m
//  TCPClient
//
//  Created by Kai Deng on 14-2-24.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import "GroupCell.h"

@implementation GroupCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)dealloc {

    [Grouplable release];
    [Groupimage release];
    [super dealloc];
}
@end
