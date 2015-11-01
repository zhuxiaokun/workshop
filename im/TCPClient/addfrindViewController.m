//
//  addfrindViewController.m
//  TCPClient
//
//  Created by Kai Deng on 14-2-26.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import "addfrindViewController.h"

@interface addfrindViewController ()

@end

@implementation addfrindViewController

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
	self.title=@"加好友";
    self.view.backgroundColor=[UIColor lightGrayColor];
    
    UIButton *button=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    button.frame=CGRectMake(0, 10, 70, 30);
    [button setTitle:@"好友列表" forState:UIControlStateNormal];
    [button addTarget:self action:@selector(back) forControlEvents:UIControlEventTouchUpInside];
    
    UIBarButtonItem *bat=[[UIBarButtonItem alloc]initWithCustomView:button];
    self.navigationItem.leftBarButtonItem=bat;
    
    friendtextfield=[[UITextField alloc]initWithFrame:CGRectMake(0, 130, 320, 30)];
    friendtextfield.placeholder=@"请输入号码／关键字";
    friendtextfield.font=[UIFont systemFontOfSize:15.0];
    friendtextfield.backgroundColor=[UIColor whiteColor];
    friendtextfield.keyboardType=UIKeyboardAppearanceDefault;
    friendtextfield.delegate=self;
    
    UIButton *addbut=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    addbut.frame=CGRectMake(100, 180, 120, 70);
    [addbut setTitle:@"添加" forState:UIControlStateNormal];
    [addbut addTarget:self  action:@selector(addfriend) forControlEvents:UIControlEventTouchUpInside];
    
    
    [self.view addSubview:addbut];
    [self.view addSubview:friendtextfield];

    [bat release];
    
}



-(void)addfriend
{
    
}

-(void)back
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
