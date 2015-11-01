//
//  Cell2.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-4.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Cell2 : UITableViewCell
@property (retain, nonatomic) IBOutlet UILabel *friendNm;
@property (retain, nonatomic) IBOutlet UILabel *topLABLE;
@property (retain, nonatomic) IBOutlet UIImageView *topIM;
@property (retain, nonatomic) IBOutlet UILabel *MessageTalk;
@property (retain, nonatomic) IBOutlet UIImageView *headerIM;
@property(nonatomic ,strong)UITableView *mtable;
@end
