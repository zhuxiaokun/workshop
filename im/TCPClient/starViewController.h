//
//  starViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-18.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface starViewController : UIViewController<UIScrollViewDelegate>{
    
  
     UIScrollView *pageScroll;
}
@property (nonatomic ,strong)UIScrollView *pageScroll;
@end
