//
//  CreateGroupViewController.m
//  TCPClient
//
//  Created by tangwei on 14-2-27.
//  Copyright (c) 2014年 tw. All rights reserved.
//

#import "CreateGroupViewController.h"

@interface CreateGroupViewController ()

@end

@implementation CreateGroupViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title=@"创建组";
    self.view.backgroundColor=[UIColor lightGrayColor];
    
    UIButton *button=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    button.frame=CGRectMake(0, 10, 70, 30);
    [button setTitle:@"返回" forState:UIControlStateNormal];
    [button addTarget:self action:@selector(back) forControlEvents:UIControlEventTouchUpInside];
    
    UIBarButtonItem *bat=[[UIBarButtonItem alloc]initWithCustomView:button];
    self.navigationItem.leftBarButtonItem=bat;
    [bat release];
    
    self.groupTF=[[UITextField alloc]initWithFrame:CGRectMake(0, 130, 320, 30)];
    _groupTF.placeholder=@"请输入欲创建组的名称";
    _groupTF.font=[UIFont systemFontOfSize:15.0];
    _groupTF.backgroundColor=[UIColor whiteColor];
    _groupTF.keyboardType=UIKeyboardAppearanceDefault;
    _groupTF.delegate=self;
    
    UIButton *addbut=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    addbut.frame=CGRectMake(100, 180, 120, 70);
    [addbut setTitle:@"创建" forState:UIControlStateNormal];
    [addbut addTarget:self  action:@selector(createGroup) forControlEvents:UIControlEventTouchUpInside];
    
    [self.view addSubview:addbut];
    [self.view addSubview:_groupTF];
    
    [self.groupTF release];
    
    UITapGestureRecognizer *tapGR = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hideKeyBoard)];
    [self.view addGestureRecognizer:tapGR];
    [tapGR release];
}

- (void)hideKeyBoard
{
    [self.view endEditing:YES];
}

- (void)createGroup
{
    NSString *groupName = self.groupTF.text;
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:[NSString stringWithFormat:@"创建的组名为：%@",groupName] delegate:self cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
    [alertView show];
    [alertView release];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}

-(void)back
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)dealloc
{
    self.groupTF = nil;
    [super dealloc];
}

@end
