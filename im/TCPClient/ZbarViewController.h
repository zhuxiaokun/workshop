//
//  ZbarViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-17.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ZbarViewController : UIViewController<ZBarReaderDelegate>{
  
    ZBarReaderViewController *reader;
    IBOutlet UITextField *TextFilde;
  
    IBOutlet UIImageView *imageView;
}
- (IBAction)BackView:(id)sender;
- (IBAction)ProduceButton:(id)sender;
- (IBAction)ScanButton:(id)sender;


@end
