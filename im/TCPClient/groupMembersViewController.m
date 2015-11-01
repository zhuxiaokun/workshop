//
//  groupMembersViewController.m
//  TCPClient
//
//  Created by Kai Deng on 14-2-24.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import "groupMembersViewController.h"
#import "groupmembersCell.h"
#import "groupChatViewController.h"
#import "Chat1ViewController.h"

@interface groupMembersViewController ()

@end

@implementation groupMembersViewController

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
	UIButton *btn=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    btn.frame=CGRectMake(0, 10, 70, 50);
    [btn setTitle:@"群消息" forState:UIControlStateNormal];
    [btn addTarget:self action:@selector(back) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *bar=[[UIBarButtonItem alloc]initWithCustomView:btn];
    self.navigationItem.leftBarButtonItem=bar;
    
    groupmembersTableview =[[UITableView alloc]initWithFrame:CGRectMake(0, 0, 320, [UIScreen mainScreen].bounds.size.height) style:UITableViewStylePlain];
    groupmembersTableview.delegate=self;
    groupmembersTableview.dataSource=self;
    
    [self.view addSubview:groupmembersTableview];
    
    [bar release];
}
-(void)back
{
    
    [self dismissViewControllerAnimated:YES completion:nil];
}
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 10;
}
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath

{
    NSString *str=@"groupmemberscell";
    groupmembersCell *cell=[tableView dequeueReusableCellWithIdentifier:str];
    if(cell==nil){
        cell=[[[NSBundle mainBundle]loadNibNamed:str owner:self options:nil]lastObject];
    }
   
    return cell;
}
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    Chat1ViewController *chat=[[Chat1ViewController alloc]initWithNibName:@"cView" bundle:nil];
    [self presentViewController:chat animated:YES completion:nil];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
