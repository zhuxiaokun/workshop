//
//  starViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-18.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "starViewController.h"

@interface starViewController ()

@end

@implementation starViewController
@synthesize pageScroll;
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
    pageScroll=[[UIScrollView alloc]initWithFrame:CGRectMake(0, 0, 320, 480)];
    pageScroll.delegate=self;
       pageScroll.contentSize = CGSizeMake(self.view.frame.size.width * 5, self.view.frame.size.height);
    int i;
    for ( i=1; i<6; i++) {
UIImageView *iv=[[UIImageView alloc]initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"%d%d",i,i]]];
        CGRect rect=iv.frame;
        rect.origin.x=(i*320);
        rect.origin.y=0;
        [pageScroll addSubview:iv];
    }
    [self.view addSubview:pageScroll];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [pageScroll release];
       [super dealloc];
}
@end
