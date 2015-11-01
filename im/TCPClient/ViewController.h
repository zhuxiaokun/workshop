//
//  ViewController.h
//  TCPClient
//
//  Created by qianfeng on 13-4-26.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AsyncSocket.h"

@interface ViewController : UIViewController<AsyncSocketDelegate,NSStreamDelegate>{
    IBOutlet UITextField* ipField;
    IBOutlet UITextField *password;
    IBOutlet UITextField *username;
    IBOutlet UITextField* sendField;
    IBOutlet UITextView* textView;
    AsyncSocket* clientSocket;
}

- (IBAction)connection:(id)sender;
- (IBAction)sendText:(id)sender;

@end




