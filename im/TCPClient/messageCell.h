//
//  messageCell.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-14.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface messageCell : UITableViewCell
@property(nonatomic, strong)UILabel *senderandtimelable;
@property(nonatomic, strong)UITextView *messageContentView;
@property(nonatomic, strong)UIImageView *bgImageView;
@property(nonatomic, strong)UILabel * voicelable;
@property(nonatomic, strong)UIImageView *imageContentView;
@end
