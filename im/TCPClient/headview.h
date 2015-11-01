//
//  headview.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-5.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
@protocol HeadViewDelegate;
@interface headview : UIView{
    
    id<HeadViewDelegate> _delegate;
    NSInteger section;
    UIButton *backBtn;
    BOOL open;
}
@property(assign)BOOL open;
@property(nonatomic ,strong)id<HeadViewDelegate>delegate;
@property(assign)NSInteger section;
@property(nonatomic ,strong)UIButton *backBtn;
@end
@protocol HeadViewDelegate <NSObject>

-(void)selectedWith:(headview *)view;

@end
