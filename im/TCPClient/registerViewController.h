//
//  registerViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-6.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface registerViewController : UIViewController
@property (retain, nonatomic) IBOutlet UITextField *rpassword;
@property (retain, nonatomic) IBOutlet UITextField *ruserName;
- (IBAction)Register:(id)sender;
@property (retain, nonatomic) IBOutlet UIView *tView;

@end
