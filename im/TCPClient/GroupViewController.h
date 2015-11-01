//
//  GroupViewController.h
//  TCPClient
//
//  Created by Kai Deng on 14-2-24.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface GroupViewController : UIViewController<UITableViewDataSource,UITableViewDelegate,UIAlertViewDelegate>
{
    UITableView *groupTableview;
    NSIndexPath *indexpath;
      BOOL change;
    UINavigationBar *bar;
}
@property(nonatomic ,strong) UINavigationBar *bar;
-(UILongPressGestureRecognizer *)addTap;
@end
