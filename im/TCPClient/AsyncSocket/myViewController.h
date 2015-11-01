//
//  myViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-16.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface myViewController : UIViewController<UITableViewDataSource,UITableViewDelegate>{
    
    UITableView *moretableview;
    NSArray *datasoure;
    UIButton *but;
}
- (IBAction)back:(id)sender;


@end
