//
//  tbViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-16.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "tbViewController.h"
#import "frindViewController.h"
#import "Chat1ViewController.h"
#import "myViewController.h"
#import "WeiXinViewController.h"
#import "Photo.h"
#import "groupViewController.h"
@interface tbViewController ()

@end

@implementation tbViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.tabBar.hidden=YES;
        
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
#pragma mark recive data
    
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(adddata:) name:@"dataNotification" object:nil];
    
	frindViewController *f1=[[frindViewController alloc]init];
    f1.title=@"好友列表";
    
    myViewController *m1=[[myViewController alloc]init];
    m1.title=@"设置";
    
    GroupViewController *g1=[[GroupViewController alloc]init];
    UINavigationController *nv=[[UINavigationController alloc]initWithRootViewController:g1];
   
    
    NSArray *array=[NSArray arrayWithObjects:f1,nv,m1, nil];

    [self setViewControllers:array];
    [self loadCustomTabBarView];
    self.view.backgroundColor=[UIColor clearColor];
    
    [f1 release];
    [m1 release];
    [g1 release];
    [nv release];

}
-(void)loadCustomTabBarView
{
    
    UIImageView *tabbar=[[UIImageView alloc]initWithFrame:CGRectMake(0, 431, 320, 49)];
    tabbar.userInteractionEnabled=YES;
    tabbar.image=[UIImage imageNamed:@"ToolViewBkg@2x.jpg"];
    [self.view addSubview:tabbar];
    
    selectIm=[[UIImageView alloc]initWithFrame:CGRectMake(10, 49/2-45/2, 53, 45)];
    selectIm.image=[UIImage imageNamed:@"SystemTipBg@2x.png"];
    [tabbar addSubview:selectIm];
    
    float index=0;
    for(int i =0 ;i<3;i++){
        UIButton *button=[UIButton buttonWithType:UIButtonTypeRoundedRect];
        button.tag=i;
        button.frame=CGRectMake(14+index, 49/2-20, 42, 40);
        NSString *imagename=[NSString stringWithFormat:@"ii%d.png",i+1];
        [button setBackgroundImage:[UIImage imageNamed:imagename] forState:UIControlStateNormal];
        [tabbar addSubview:button];
        [button addTarget:self action:@selector(changeViewController:) forControlEvents:UIControlEventTouchUpInside];
        index+=120;
        
    }
    
    
    [tabbar release];
    
}
-(void)changeViewController:(UIButton *)button
{
    self.selectedIndex=button.tag;
    [UIView beginAnimations:nil context:nil];
    selectIm.frame=CGRectMake(10+button.tag*120, 49/2-45/2, 53, 45);
    [UIView commitAnimations];
}
-(void)tabBar:(UITabBar *)tabBar didSelectItem:(UITabBarItem *)item
{
   
}
-(void)adddata:(NSNotification *)not
{
    NSMutableArray *array=[[NSMutableArray alloc]initWithCapacity:0];
    NSDictionary *dic=[not userInfo];
    [array addObject:dic];
    datacount=[array count];
    
    [array release];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
