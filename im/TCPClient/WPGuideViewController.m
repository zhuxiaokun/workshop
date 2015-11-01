//
//  WPGuideViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-23.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "WPGuideViewController.h"

@interface WPGuideViewController ()

@end

@implementation WPGuideViewController
@synthesize animating,pageScoll;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.view.backgroundColor=[UIColor whiteColor];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    _pageScroll = [[UIScrollView alloc]initWithFrame:CGRectMake(0, 0, 320, 480)];
    _pageScroll.pagingEnabled=YES;
    _pageScroll.contentSize=CGSizeMake(3*320, 480) ;
    [self.view addSubview:_pageScroll];
    
    
    int i;
    for ( i=0; i<3; i++) {
        UIImageView *iv=[[UIImageView alloc]initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"%d%d.jpg",i+1,i+1]]];
        CGRect rect=iv.frame;
        rect.origin.x=(i*320);
        rect.origin.y=0;
        rect.size.height=480;
        rect.size.width=320;
        iv.frame=rect;
        iv.tag=i;
        [_pageScroll addSubview:iv];
        
    }
}
+(WPGuideViewController *)shareGuide
{
    @synchronized(self)
    {
        static WPGuideViewController *shareGuide = nil;
        if (shareGuide==nil) {
            shareGuide = [[self alloc]init];
        }
        return  shareGuide;
    }

}

#pragma mark - 淡入淡出的效果
+(void)show
{
    [[WPGuideViewController shareGuide].pageScoll setContentOffset:CGPointMake(0, 0)];
    [[WPGuideViewController shareGuide]showGuide];
}
+(void)hide
{
    [[WPGuideViewController shareGuide]hideGuide];
}

-(void)showGuide
{
    if(!_anmiating &&self.view.superview==nil)
    {
        [WPGuideViewController shareGuide].view.frame=[self offscreenFtame];
    
    [[self mainWindow] addSubview:[WPGuideViewController shareGuide].view];
    
    _anmiating = YES;
    
    [UIView beginAnimations:nil context:nil];
    [UIView  setAnimationDuration:2.0];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDidStopSelector:@selector(guiideShown)];
    [WPGuideViewController shareGuide].view.frame=[self onscreenFrame];
    [UIView commitAnimations];
    }
}
-(void)guiideShown
{
    _anmiating = NO;
}

- (void)hideGuide
{
	if (!_anmiating && self.view.superview != nil)
	{
		_anmiating = YES;
		[UIView beginAnimations:nil context:nil];
		[UIView setAnimationDuration:0.4];
		[UIView setAnimationDelegate:self];
		[UIView setAnimationDidStopSelector:@selector(guideHidden)];
		[WPGuideViewController shareGuide].view.frame = [self offscreenFtame];
		[UIView commitAnimations];
	}
}

- (void)guideHidden
{
	_anmiating = NO;
	[[[WPGuideViewController shareGuide] view] removeFromSuperview];
}
-(CGRect)onscreenFrame
{
    return  [UIScreen mainScreen].applicationFrame;
}
-(CGRect)offscreenFtame
{
    CGRect frame = [self onscreenFrame];
	switch ([UIApplication sharedApplication].statusBarOrientation)
    {
		case UIInterfaceOrientationPortrait:
			frame.origin.y = frame.size.height;
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			frame.origin.y = -frame.size.height;
			break;
		case UIInterfaceOrientationLandscapeLeft:
			frame.origin.x = frame.size.width;
			break;
		case UIInterfaceOrientationLandscapeRight:
			frame.origin.x = -frame.size.width;
			break;
	}
	return frame;
}

-(UIWindow *)mainWindow
{
    UIApplication *app=[UIApplication sharedApplication];
    if ([app.delegate respondsToSelector:@selector(window)]) {
        return  [app.delegate window];
    }else
    {
        return  [app keyWindow];
    }
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
