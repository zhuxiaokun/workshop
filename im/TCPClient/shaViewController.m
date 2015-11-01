//
//  shaViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-17.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "shaViewController.h"
#import "tbViewController.h"

@interface shaViewController ()

@end

@implementation shaViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor=[UIColor whiteColor];
    iv=[[UIImageView alloc]initWithImage:[UIImage imageNamed:@"ico_deco_shake"]];
    iv.frame=CGRectMake(20, 100, 280, 200);
    but=[[UIButton alloc]initWithFrame:CGRectMake( 20, 300, 280, 80)];
    [but addTarget:self action:@selector(addAnimations1) forControlEvents:UIControlEventTouchUpInside];
    [but setTitle:@"yao-yao" forState:UIControlStateNormal];
    [but setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [self.view addSubview:but];
    [but release];
    
    [self.view addSubview:iv];
    [iv release];
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(addAnimations) name:@"shake" object:nil];
    NSString *path = [[NSBundle mainBundle] pathForResource:@"shake" ofType:@"wav"];
	AudioServicesCreateSystemSoundID((CFURLRef)[NSURL fileURLWithPath:path], &soundID);
    UIButton* but1=[[UIButton alloc]initWithFrame:CGRectMake( 0, 20, 70 , 30)];
    [but1 addTarget:self action:@selector(addAnimations2) forControlEvents:UIControlEventTouchUpInside];
    [but1 setTitle:@"back" forState:UIControlStateNormal];
    [but1 setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [self.view addSubview:but1];
    [but1 release];
}

-(void)addAnimations2
{
    //    tbViewController *tb=[[tbViewController alloc]init];
    //    [self presentViewController:tb animated:YES completion:nil];
    [self dismissViewControllerAnimated:YES completion:nil];
}
-(void)addAnimations
{
    CABasicAnimation *translation = [CABasicAnimation animationWithKeyPath:@"transform"];
    translation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    translation.toValue=[NSValue valueWithCATransform3D:CATransform3DMakeRotation(-M_PI_4, 0, 0, 100)];
    
    translation.duration = 0.2;
    translation.repeatCount = 2;
    translation.autoreverses = YES;
    
    [iv.layer addAnimation:translation forKey:@"translation"];
}

-(BOOL)canBecomeFirstResponder{
    return YES;
}
-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self becomeFirstResponder];
}
-(void)viewWillDisappear:(BOOL)animated
{
    [self resignFirstResponder];
    [super viewWillDisappear:animated];
}

-(void)addAnimations1
{
    [self addAnimations];
    AudioServicesPlaySystemSound (soundID);
}

@end
