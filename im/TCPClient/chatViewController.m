//
//  chatViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-7.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "chatViewController.h"

@interface chatViewController ()

@end

@implementation chatViewController

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
	chattView.separatorStyle=UITableViewCellSeparatorStyleNone;
    UITapGestureRecognizer *tap=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(viewTapped1:)];
    tap.cancelsTouchesInView=YES;
    [self.view addGestureRecognizer:tap];
    messageTextFild.delegate=self;
    
    [tap release];
}
-(void)viewTapped1:(UIGestureRecognizer *)tap{
    [messageTextFild resignFirstResponder];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
-(BOOL)textFieldShouldReturn:(UITextField *)textField{
    [textField resignFirstResponder];
    return YES;
}
- (void)dealloc {
    [messageTextFild release];
    [chattView release];
    [messageTextFild release];
    [chattView release];
    [super dealloc];
}
- (IBAction)sendmessage:(id)sender {
}


@end
