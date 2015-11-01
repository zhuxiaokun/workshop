//
//  RegisenedViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-23.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Change.h"

@interface RegisenedViewController : UIViewController<UITextFieldDelegate,UIImagePickerControllerDelegate,UINavigationControllerDelegate,UIActionSheetDelegate>{
    
    IBOutlet UITextField *resigenedNM;
    IBOutlet UITextField *resigenedPW;
    IBOutlet UIImageView *HeaderView;
    UIImagePickerController *Picker;
    Change *headview;
}

- (IBAction)goback:(id)sender;
- (IBAction)Regisened:(id)sender;

@end
