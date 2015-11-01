//
//  ImageViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-29.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "ImageViewController.h"
#import "Photo.h"

#define _alpha 0.5
@interface ImageViewController ()

@end

@implementation ImageViewController
@synthesize Imagelable;

- (void)viewDidLoad
{
    [super viewDidLoad];
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(showImage:) name:@"imagedata" object:nil];
 //self.view.alpha=_alpha;
 
}

-(void)showImage:(NSNotification *)notification
{
 
   array=[notification object];
    NSLog(@"array is count %d",[array count]);
    scrollview =[[UIScrollView alloc]init];
	scrollview.frame=CGRectMake(0, 70, 320,480);
    scrollview.contentSize=CGSizeMake([array count]*320, 480);
    scrollview.delegate=self;
    scrollview.pagingEnabled=YES;
    [self.view addSubview:scrollview];
   // scrollview.alwaysBounceVertical=YES;
    imagecount=[array count];
    int i;
    for (i=0; i<[array count]; i++) {
        UIImageView *iv=[[UIImageView alloc]initWithImage:[Photo scaleImage:[array objectAtIndex:i] toWidth:300 toHeight:300]];
        CGRect rect=iv.frame;
       rect.origin.x=(i*320)+10;
        rect.origin.y=0;
        UITapGestureRecognizer *tap1=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(tapImage)];
        [iv addGestureRecognizer:tap1];
        tap1.numberOfTouchesRequired=1;
        tap1.numberOfTapsRequired=1;
        iv.frame=rect;
        iv.userInteractionEnabled=YES;
        
        NSLog(@"frame is %f",iv.frame.size.height);
        [scrollview addSubview:iv];
        
        [iv release];
        [tap1 release];
    }
}
-(void)tapImage
{
    UIActionSheet *ac=[[UIActionSheet alloc]initWithTitle: nil delegate:self cancelButtonTitle:@"取消" destructiveButtonTitle:nil otherButtonTitles:@"微博分享",@"微信分享", nil];
    [ac showInView:self.view];
    
    [ac release];
}
-(void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    CGPoint  offset=scrollView.contentOffset;
    CGRect  bounds=scrollView.frame;
    int i=(int)offset.x/bounds.size.width;
    Imagelable.text=[NSString stringWithFormat:@"%d/%d",i+1,imagecount];
    
    
}
- (void)dealloc {
    [Imagelable release];
    [super dealloc];
}
- (IBAction)backItem:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}
@end
