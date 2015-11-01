//
//  Cell2.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-4.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "Cell2.h"

@implementation Cell2
@synthesize friendNm,MessageTalk,mtable,topIM,topLABLE,headerIM;
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
    [friendNm release];
    [MessageTalk release];
    [headerIM release];
    [topIM release];
    [topLABLE release];
    [super dealloc];
}
@end
