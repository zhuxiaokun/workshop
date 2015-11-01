//
//  Voicehud.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-25.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Voicehud : UIView
@property (nonatomic) float progress;
-(void) show;
-(void) hide;
@end
