//
//  groupChatViewController.h
//  TCPClient
//
//  Created by Kai Deng on 14-2-24.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Chat1ViewController.h"
#import "ChatVoiceRecorderVC.h"
@interface groupChatViewController : UIViewController<UITableViewDataSource,UITableViewDelegate,UITextFieldDelegate,UIImagePickerControllerDelegate,UINavigationControllerDelegate,UIActionSheetDelegate,UIAlertViewDelegate>
{
    
    UITableView *groupChattableview;
    UIImagePickerController *imagepicker;
    NSMutableArray *groupMessage;
    UITextField  * textfiled;
    Chat1ViewController *chatView;
  
  
}

@property(strong ,nonatomic)voide * Voide;

//添加手势的方法



@end
