//
//  registerViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-6.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "registerViewController.h"

@interface registerViewController ()

@end

@implementation registerViewController
@synthesize tView;
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
	//registerViewController *v=[[registerViewController alloc]initWithNibName:@"registerView" bundle:nil];
    //[self.view addSubview:tView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_ruserName release];
    [_rpassword release];
   // [_rtView release];
    [super dealloc];
}
- (IBAction)Register:(id)sender {
}
@end
