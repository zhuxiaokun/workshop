//
//  StarUpViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-18.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "StarUpViewController.h"
#import "firstViewController.h"
#import "WPGuideViewController.h"


@interface StarUpViewController ()

@end

@implementation StarUpViewController


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
       self.view.backgroundColor=[UIColor whiteColor];
    pageScroll=[[UIScrollView alloc]initWithFrame:CGRectMake(0, 0, 320, 480)];
    pageScroll.delegate=self;
    pageScroll.contentSize = CGSizeMake(self.view.frame.size.width * 3, self.view.frame.size.height);
    pageScroll.backgroundColor=[UIColor blueColor];
    [self.view addSubview:pageScroll];
    pageScroll.pagingEnabled=YES;
    pageScroll.bounces=NO;
    pageScroll.showsHorizontalScrollIndicator=NO;
    pageScroll.backgroundColor=[UIColor whiteColor];
    int i;
    for ( i=0; i<53; i++) {
        UIImageView *iv=[[UIImageView alloc]initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"%d%d%d.jpg",i+1,i+1,i+1]]];
        CGRect rect=iv.frame;
        rect.origin.x=(i*320);
        rect.origin.y=0;
        rect.size.height=480;
        rect.size.width=320;
        iv.frame=rect;
        iv.tag=i;
        [pageScroll addSubview:iv];
        
    if(i>=1)
    {
        iv.alpha=0.0;
        
    }
        
        [iv release];
    }
    
    
#pragma mark pagecontrol
    
    pageControl=[[UIPageControl alloc]initWithFrame:CGRectMake(0, 420, 320, 30)];
    pageControl.numberOfPages=3;
    pageControl.currentPage=0;
    [pageControl addTarget:self action:@selector(pageturn:) forControlEvents:UIControlEventValueChanged];
    pageControl.backgroundColor=[UIColor grayColor];
    [self.view addSubview:pageControl];

#pragma mark button

    button=[[UIButton alloc]initWithFrame:CGRectMake(250, 380, 70, 50)];
    [button addTarget:self action:@selector(gomainView:) forControlEvents:UIControlEventTouchUpInside];
    [button setTitle:@"进入" forState:UIControlStateNormal];
    [button setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [button setTitleColor:[UIColor whiteColor] forState:UIControlStateSelected];
    [self.view addSubview:button];
    
     hidden1 =YES;
    
}

#pragma mark next////////////

-(void)gomainView:(id)sender
{
    UIButton *butt=(UIButton *)sender;
    butt.selected=!butt.selected;
    [[NSUserDefaults standardUserDefaults]setBool:NO forKey:@"firstLaunc"];
    firstViewController *fv=[[firstViewController alloc]init];
    [self presentViewController:fv animated:YES completion:nil];
    
}

-(void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    CGPoint offset=scrollView.contentOffset;
    CGRect bounds=scrollView.frame;
    [pageControl setCurrentPage:offset.x/bounds.size.width];
    CGPoint point=scrollView.contentOffset;
    CGRect bounds1=scrollView.frame;
    
    int i=point.x/bounds1.size.width;
    if (i==2) {
        hidden1 = YES;
    }
    [self setHidden:hidden1 tag:i];
    
    
    
}


#pragma mark - 引导页动画效果

-(void)setHidden:(BOOL) hidden2 tag:(int)tag
{
    
    if(hidden1){
       UIImageView *image=(UIImageView*)[self.view viewWithTag:tag];
        image.alpha=0.0;
        [UIView beginAnimations:@"fadeout" context:nil];
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDuration:1.0];
        [UIView setAnimationDidStopSelector:@selector(animationstop)];
        image.alpha=1.0;
        [UIView commitAnimations];
    }
}
-(void)animationstop
{
    hidden1 = NO;
}
-(void)pageturn:(UIPageControl *)sender
{
    CGSize viewsize=pageScroll.frame.size;
    CGRect rect=CGRectMake(sender.currentPage*viewsize.width, 0, viewsize.width, viewsize.height);
    [pageScroll scrollRectToVisible:rect animated:YES];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
