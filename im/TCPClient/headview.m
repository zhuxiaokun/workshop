//
//  headview.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-5.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "headview.h"

@implementation headview
@synthesize delegate=_delegate;
@synthesize section,backBtn,open;
- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        open =NO;
        UIButton *btn=[UIButton buttonWithType:UIButtonTypeCustom];
        btn.frame=CGRectMake(0, 0, 340, 45.5);
        [btn addTarget:self action:@selector(doselect) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:btn];
        self.backBtn=btn;
    }
    return self;
}

-(void)doselect
{
    if (_delegate&&[_delegate respondsToSelector:@selector(selectedWith:)]) {
        [_delegate selectedWith:self];
    }
}

@end
