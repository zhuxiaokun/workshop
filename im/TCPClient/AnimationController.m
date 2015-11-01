//
//  AnimationShakeViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-17.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "AnimationController.h"
#import <QuartzCore/QuartzCore.h>

@interface AnimationController ()

@end

@implementation AnimationShakeViewController

//- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
//{
//    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
//    if (self) {
//        // Custom initialization
//    }
//    return self;
//}
//
//- (void)viewDidLoad
//{
//    [super viewDidLoad];
//    NSString *path=[[NSBundle mainBundle]pathForResource:@"shake" ofType:@"wav"];
//AudioServicesCreateSystemSoundID((CFURLRef)[NSURL fileURLWithPath:path], &soundID);
//    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(addAnimations) name:@"shake" object:nil];
//	
//}
//-(BOOL)canBecomeFirstResponder
//{
//    return  YES;
//}
//
//-(void)viewDidAppear:(BOOL)animated
//{
//    [super viewDidAppear:YES];
//    [self becomeFirstResponder];
//}
//-(void)addAnimations
//{
//    CABasicAnimation *translation=[CABasicAnimation animationWithKeyPath:@"transform"];
//    translation.timingFunction=[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseOut];
//    translation.toValue=[NSValue valueWithCATransform3D:CATransform3DMakeRotation(-M_PI_4, 0, 0, 100)];
//    translation.duration=0.2;
//    translation.repeatCount=2;
//    translation.autoreverses=YES;
//    [shakeImageView.layer addAnimation:translation forKey:@"translation"];
//    
//}
//
//- (void)dealloc {
//    [shakeImageView release];
//    [super dealloc];
//}
//- (IBAction)shake:(id)sender {
//    [self addAnimations];
//}
@end
