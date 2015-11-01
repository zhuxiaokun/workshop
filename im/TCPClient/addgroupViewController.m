//
//  addgroupViewController.m
//  TCPClient
//
//  Created by Kai Deng on 14-2-26.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import "addgroupViewController.h"
#import "GroupViewController.h"

@interface addgroupViewController ()

@end

@implementation addgroupViewController

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
	self.title=@"加入群";
    self.view.backgroundColor=[UIColor lightGrayColor];
    
    UIButton *button=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    button.frame=CGRectMake(0, 10, 50, 30);
    [button setTitle:@"群" forState:UIControlStateNormal];
    [button addTarget:self action:@selector(back) forControlEvents:UIControlEventTouchUpInside];
    
    UIBarButtonItem *bat=[[UIBarButtonItem alloc]initWithCustomView:button];
    self.navigationItem.leftBarButtonItem=bat;
    
    textfield=[[UITextField alloc]initWithFrame:CGRectMake(0, 130, 320, 30)];
    textfield.placeholder=@"请输入群号码／关键字";
    textfield.font=[UIFont systemFontOfSize:15.0];
    textfield.backgroundColor=[UIColor whiteColor];
    textfield.keyboardType=UIKeyboardAppearanceDefault;
    textfield.delegate=self;
    
    UIButton *addbut=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    addbut.frame=CGRectMake(100, 180, 120, 70);
    [addbut setTitle:@"添加" forState:UIControlStateNormal];
    [addbut addTarget:self  action:@selector(addgroup) forControlEvents:UIControlEventTouchUpInside];

    
    [self.view addSubview:addbut];
    [self.view addSubview:textfield];
    
    
    [bat release];
    [textfield release];
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textfield resignFirstResponder];
    return YES;
}

-(void)addgroup
{
    
}
-(void)back

{
  
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
   
}

@end
