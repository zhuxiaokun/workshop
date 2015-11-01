//
//  GroupViewController.m
//  TCPClient
//
//  Created by Kai Deng on 14-2-24.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import "GroupViewController.h"
#import "GroupCell.h"
#import "groupChatViewController.h"
#import "addgroupViewController.h"

@interface GroupViewController ()

@end

@implementation GroupViewController
@synthesize bar;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title=@"群";
    UIButton *button=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    button.frame=CGRectMake(230, 10, 25, 25);
    [button setImage:[UIImage imageNamed:@"group_right_btn@2x.png"] forState:UIControlStateNormal];
    [button addTarget:self action:@selector(PopupControls:) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *baritem=[[UIBarButtonItem alloc]initWithCustomView:button];
    self.navigationItem.rightBarButtonItem=baritem;
    
    groupTableview = [[UITableView alloc]initWithFrame:CGRectMake(0, 0, 320, [UIScreen mainScreen].bounds.size.height) style:UITableViewStylePlain];
    groupTableview.delegate=self;
    groupTableview.dataSource=self;
   
    [self.view addSubview:groupTableview];
    [self.view addSubview:[self ManufacturingCotrols]];
    
    [groupTableview release];
   
    
////添加手势
    
    [groupTableview addGestureRecognizer:[self addTap]];
    change=YES;

 
}
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
    
}
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 10;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 55;
}
-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString*str=@"groupcell";
    GroupCell *cell=[tableView dequeueReusableCellWithIdentifier:str];
    if (cell==nil) {
        cell=[[[NSBundle mainBundle] loadNibNamed:@"groupcell" owner:self options:nil] lastObject];
    }
      
    return cell;
}
//tableviewcell点击事件

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    groupChatViewController *gchat=[[groupChatViewController alloc]init];
    UINavigationController *na=[[UINavigationController alloc]initWithRootViewController:gchat];
    [self presentViewController:na animated:YES completion:nil];
    
    [gchat release];
    [na release];
    
}

-(void)back
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(UILongPressGestureRecognizer *)addTap
{
    UILongPressGestureRecognizer *tap=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(preseTap:)];
    
    tap.cancelsTouchesInView=YES;
    tap.numberOfTouchesRequired=1;
    tap.minimumPressDuration=1;
    
    
    return [tap autorelease];
}

//点击手势的方法

-(void)preseTap:(UILongPressGestureRecognizer*) longpress
{
    if(longpress.state == UIGestureRecognizerStateEnded){
        
//获得点击的位置
        
        CGPoint location=[longpress locationInView:groupTableview];
      NSIndexPath * indexpath1=[groupTableview indexPathForRowAtPoint:location];
        indexpath=[indexpath1 retain];
        [(GroupCell *)longpress.view becomeFirstResponder];
        
    UIAlertView *al=[[UIAlertView alloc]initWithTitle:@"是否退出该群" message:nil delegate:self cancelButtonTitle:@"是" otherButtonTitles:@"取消", nil];
    
    [al show];
        [al release];
    }
}

//alertview的delgate的方法

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex==0) {
        NSLog(@"indexpath is %d",indexpath.row);
        
    }
}

#pragma mark - 弹出控件的方法

-(void)PopupControls:(id)senter
{
    if (change) {
       
        [UIView animateWithDuration:0.5 animations:^{
                bar.frame=CGRectMake(0, 55, 320, 55);
        }];
    
       
      
        change=NO;
    }else
    {
        [UIView animateWithDuration:0.5 animations:^{
            bar.frame=CGRectMake(0, 0, 320, 55);
        }];
        change=YES;
    }
}

#pragma mark - 控件的制作方法

-(UIView *)ManufacturingCotrols
{
     bar=[[UINavigationBar alloc]initWithFrame:CGRectMake(0, 0, 320, 55)];
    UIButton *btn1=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    btn1.frame=CGRectMake(5, 5, 150, 45);
    [btn1 setTitle:@"创建群" forState:UIControlStateNormal];
    btn1.tag=1;
    
    UIButton *btn2=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    btn2.frame=CGRectMake(167, 5, 150, 45);
    [btn2 setTitle:@"加入群" forState:UIControlStateNormal];
    btn2.tag=2;
    
    [btn1 addTarget:self action:@selector(Manufacturinggroup:) forControlEvents:UIControlEventTouchUpInside];
    [btn2 addTarget:self action:@selector(addgroup:) forControlEvents:UIControlEventTouchUpInside];
    
    [bar addSubview:btn1];
    [bar addSubview:btn2];
    
    
    return [bar autorelease];
}

//创建群

-(void)Manufacturinggroup:(id)senter

{
    [UIView animateWithDuration:0.5 animations:^{
        bar.frame=CGRectMake(0, 0, 320, 55);
    }];
}

//加入群

-(void)addgroup:(id)senter1
{
    [UIView animateWithDuration:0.5 animations:^{
        bar.frame=CGRectMake(0, 0, 320, 55);
    }];
    addgroupViewController *group=[[addgroupViewController alloc]init];
    UINavigationController *na=[[UINavigationController alloc]initWithRootViewController:group];
    [self presentViewController:na animated:YES completion:nil];
    [group release];
    
}

@end
