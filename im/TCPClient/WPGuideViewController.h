//
//  WPGuideViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-23.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface WPGuideViewController : UIViewController
{
    BOOL _anmiating;
    
    UIScrollView *_pageScroll;
}
@property (nonatomic ,assign)BOOL animating;

@property(nonatomic ,strong) UIScrollView *pageScoll;
+(WPGuideViewController *)shareGuide;

+(void)show;
+(void)hide;
@end
