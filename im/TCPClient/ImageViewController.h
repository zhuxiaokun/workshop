//
//  ImageViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-29.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ImageViewController : UIViewController<UIScrollViewDelegate,UIScrollViewAccessibilityDelegate,UIActionSheetDelegate>
{
    UIScrollView *scrollview;
    NSArray *array;
    int imagecount;
}
- (IBAction)backItem:(id)sender;
@property (retain, nonatomic) IBOutlet UILabel *Imagelable;

@end
