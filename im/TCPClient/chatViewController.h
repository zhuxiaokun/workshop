//
//  chatViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-7.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface chatViewController : UIViewController<UITextFieldDelegate>{
    
 
   
    IBOutlet UITableView *chattView;
    IBOutlet UITextField *messageTextFild;
}

- (IBAction)sendmessage:(id)sender;

@end
