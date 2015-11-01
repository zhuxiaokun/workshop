//
//  AppDelegate.h
//  TCPClient
//
//  Created by qianfeng on 13-4-26.
//  Copyright (c) 2013年 PK. All rights reserved.
//
//
/*

    微信 appID : wxd33871ab0d164398

 */
#import <UIKit/UIKit.h>

@class ViewController;

@interface AppDelegate : UIResponder <UIApplicationDelegate>{
    NSMutableArray *_dataArray;
    NSMutableString *userName;
}
@property (strong, nonatomic)NSMutableArray *dataArray;
@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) ViewController *viewController;

@end
