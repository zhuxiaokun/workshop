//
//  cCustomMethod.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-19.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RegexKitLite.h"
#import "OHAttributedLabel.h"
#import "SCGIFImageView.h"

@interface cCustomMethod : NSObject
+ (NSString *)escapedString:(NSString *)oldString;
+ (void)drawImage:(OHAttributedLabel *)label;
+ (NSMutableArray *)addHttpArr:(NSString *)text;
+ (NSMutableArray *)addPhoneNumArr:(NSString *)text;
+ (NSMutableArray *)addEmailArr:(NSString *)text;
+ (NSString *)transformString:(NSString *)originalStr  emojiDic:(NSDictionary *)_emojiDic;
@end
