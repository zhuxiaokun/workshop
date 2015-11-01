//
//  Cell1.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-4.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "Cell1.h"

@implementation Cell1

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
    [Nmlable release];
    [super dealloc];
}
@end
