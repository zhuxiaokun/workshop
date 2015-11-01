//
//  frindViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-11.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#import "firstViewController.h"
#import "headview.h"
@interface frindViewController : UIViewController<UITableViewDataSource,UITableViewDelegate,UIApplicationDelegate,firstViewdelegate,HeadViewDelegate>{
    
    IBOutlet UITableView *friendTableView;
    NSMutableArray *dataarray;
    NSString *chatUserName;
    NSInteger _currentSection;
    NSInteger _currentRow;
    NSDictionary *dict;
    NSDictionary *mesdict;
   NSInteger *ArrayCount;
    int contentCount;
    NSMutableArray *messageArray;
    NSIndexPath *
    FriendIndexPath;
   
  

}
@property(assign )BOOL isOpen;
@property(nonatomic ,strong)NSIndexPath *selectIndex;
@property(assign) BOOL isChange;

@property (retain, nonatomic) IBOutlet UIBarButtonItem *PopupControls;
@property (retain, nonatomic) IBOutlet UIBarButtonItem *createGroupItem;

- (IBAction)Popcontrols:(id)sender;
- (IBAction)Creategroup:(id)sender;


@end
