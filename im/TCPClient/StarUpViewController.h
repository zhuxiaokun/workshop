//
//  StarUpViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-12-18.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface StarUpViewController : UIViewController<UIScrollViewDelegate>{
     UIScrollView *pageScroll;
    UIPageControl *pageControl;
    UIButton *button;
    BOOL hidden1;
}

@end
