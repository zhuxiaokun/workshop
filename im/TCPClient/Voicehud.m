//
//  Voicehud.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-25.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "Voicehud.h"
#import <QuartzCore/QuartzCore.h>

#pragma mark -常用的数据

#define _DEVICE_WINDOW ((UIView*)[UIApplication sharedApplication].keyWindow)
#define _DEVICE_HEIGHT ([UIScreen mainScreen].bounds.size.height-20.0f)
#define _DEVICE_WIDTH  ([UIScreen mainScreen].bounds.size.width)

#define _IMAGE_UNDER       [UIImage imageNamed:@"black_bg_ip5.png"]
#define _IMAGE_MIC_NORMAL  [UIImage imageNamed:@"mic_normal_358x358.png"]
#define _IMAGE_MIC_TALKING [UIImage imageNamed:@"mic_talk_358x358.png"]
#define _IMAGE_MIC_WAVE [UIImage imageNamed:@"wave70x117.png"]
#pragma mark uiviewfram
@interface UIView (UIViewFrame)

-(float)width;
-(float)height;

@end

@implementation UIView (UIViewFrame)

-(float)width
{
    return  self.frame.size.width;
}
-(float)height
{
    return  self.frame.size.height;
}

@end
@interface UIImage (Grayscale)
- (UIImage *) partialImageWithPercentage:(float)percentage
                                vertical:(BOOL)vertical
                           grayscaleRest:(BOOL)grayscaleRest;
@end
@implementation UIImage (Grayscale)

-(UIImage *)partialImageWithPercentage:(float)percentage vertical:(BOOL)vertical grayscaleRest:(BOOL)grayscaleRest
{
    const int ALPHA = 0;
    const int RED = 1;
    const int GREEN =2;
    const int BLUE =3;
    
    CGRect imagerect=CGRectMake(0, 0, self.size.width*self.scale, self.size.height*self.scale);
    int width=imagerect.size.width;
    int height= imagerect.size.height;
    
    uint32_t *pixels=(uint32_t *)malloc(width*height*sizeof(uint32_t));
    
    memset(pixels , 0, width*height*sizeof(uint32_t));
    
    CGColorSpaceRef colorSpace=CGColorSpaceCreateDeviceRGB();
    
    CGContextRef context=CGBitmapContextCreate(pixels, width, height, 8,                                                                                                                                   width*sizeof(uint32_t), colorSpace, kCGBitmapByteOrder32Little|kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), [self CGImage]);
    int x_origin = vertical ? 0 : width*percentage;
    int y_to =vertical ? height *(1.f -percentage):height;
    
    for (int y=0; y<y_to; y++) {
        for (int x=x_origin; x<width; x++) {
            uint8_t *rgbaPixel=(uint8_t *) &pixels[y*width +x];
            if(grayscaleRest){
                uint32_t gray=0.3 *rgbaPixel[RED] +0.59*rgbaPixel[GREEN] +rgbaPixel[BLUE]*0.11;
                rgbaPixel[RED] =gray;
                rgbaPixel[GREEN]=gray;
                rgbaPixel[BLUE]=gray;
            }
            else
            {
                rgbaPixel[ALPHA] = 0;
                rgbaPixel[RED] = 0;
                rgbaPixel[GREEN] = 0;
                rgbaPixel[BLUE] = 0;
            }
        }
    }
    
    CGImageRef image=CGBitmapContextCreateImage(context);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    free(pixels);
    
    UIImage *resultUIimage=[UIImage imageWithCGImage:image scale:self.scale orientation:UIImageOrientationUp];
    
    CGImageRelease(image);
    
    return  resultUIimage;
}

@end
#pragma mark -porgressImageView

@interface PorgressImageView : UIImageView
{
    UIImage *_originalImage;
    
    BOOL  _internalUpdating;
}

@property(nonatomic)float progress;
@property (nonatomic) BOOL hasGrayscaleBackground;
@property (nonatomic ,getter =isVerticalProgress ) BOOL verticalProgress;

@end
@interface PorgressImageView ()
@property(nonatomic ,strong) UIImage * originalImage;
-(void)commonInit;
-(void)updateDrawing;

@end
@implementation PorgressImageView

@synthesize progress= _progress,hasGrayscaleBackground= _hasGrayscaleBackground,verticalProgress= _verticalProgress;
-(id)initWithFrame:(CGRect)frame
{
    self=[super initWithFrame:frame];
    if(self){
        [self commonInit];
    }
    return self;
}
-(void)commonInit{
    _progress=0.f;
    _hasGrayscaleBackground=YES;
    _verticalProgress=YES;
    _originalImage=self.image;
}

#pragma  mark -custom
-(void)setImage:(UIImage *)image
{
    [super setImage:image];
    
    if(!_internalUpdating)
    {
        self.originalImage=image;
        [self updateDrawing];
    }
    
    _internalUpdating=NO;
}

-(void)setProgress:(float)progress
{
    _progress=MIN(MAX(0.f, progress), 1.f);
    [self updateDrawing];
}

-(void)setHasGrayscaleBackground:(BOOL)hasGrayscaleBackground
{
    _hasGrayscaleBackground=hasGrayscaleBackground;
    [self updateDrawing];
}

- (void)setVerticalProgress:(BOOL)verticalProgress{
    _verticalProgress =verticalProgress;
    [self updateDrawing];
    
}

//darw moteh

-(void)updateDrawing
{
    _internalUpdating=YES;
    self.image=[_originalImage partialImageWithPercentage:_progress vertical:_verticalProgress grayscaleRest:_hasGrayscaleBackground];
}
@end

#pragma  mark uiimageview

@interface Voicehud ()
{
    UIImageView         * _talkingImageView;
    PorgressImageView * _dynamicProgress;
}

@end
@implementation Voicehud
-(id)init{
    self =[super initWithFrame:_DEVICE_WINDOW.bounds];
    if(self){
        self.backgroundColor=[UIColor clearColor];
        self.progress=0.f;
        self.alpha=0;
        [self createMainUI];
    }
    return self;
}

#pragma  mark -view creat

-(void) createMainUI{
    
    CGRect frame1=CGRectMake(0, 20+44-44, _DEVICE_WIDTH, _DEVICE_HEIGHT+44);
    
    CGRect frame2= CGRectMake(_DEVICE_WIDTH/2-164/2, 200, 179, 179);
    
    CGRect frame3= CGRectMake(164/2-50/2,164/2-93.5/2, 179,179);
    CGRect frame4=CGRectMake(0, 0, 35, 58.5);
    
    UIImageView *backBlackImagrView=[[UIImageView alloc]initWithFrame:frame1];
    
    backBlackImagrView.image=_IMAGE_UNDER;
    [self addSubview:backBlackImagrView];
    
    UIImageView* micNormalimage=[[UIImageView alloc]initWithImage:_IMAGE_MIC_NORMAL];
    micNormalimage.frame=frame2;
    micNormalimage.center=self.center;
    [self addSubview:micNormalimage];
    
    _talkingImageView=[[UIImageView alloc]initWithFrame:frame3];
    _talkingImageView.image=_IMAGE_MIC_TALKING;
    [self addSubview:_talkingImageView];
    _talkingImageView.center=self.center;
    
    _dynamicProgress=[[PorgressImageView alloc]initWithFrame:frame4];
    _dynamicProgress.image=_IMAGE_MIC_WAVE;
    [self addSubview:_dynamicProgress];
    
    //set
    
    _dynamicProgress.progress=0;
    _dynamicProgress.hasGrayscaleBackground=YES;
    _dynamicProgress.verticalProgress=YES;
    _dynamicProgress.center=CGPointMake(self.center.x, self.center.y-13);
    
}
#pragma  mark -custom accessor progress

-(void)setProgress:(float)progress
{
    if(_dynamicProgress){
        _dynamicProgress.progress=progress;
        if(progress<=0.01){
            [self showHighLight:NO];
        }
        else{
            [self showHighLight:YES];
        }
    }
}
-(float) progress{
    if(_dynamicProgress)return _dynamicProgress.progress;
    return 0.f;
}
-(void)removeFromSuperview
{
    [super removeFromSuperview];
    self=nil;
}
-(void)showHighLight:(BOOL)yesOrNo
{
    if(yesOrNo)
    {
        [UIView animateWithDuration:0.2 animations:^{
            _talkingImageView.alpha=1;
        }];
    }else{
        [UIView animateWithDuration:0.2 animations:^{
            _talkingImageView.alpha=0;
        }];
    }
}
-(void) show{
    
    [_DEVICE_WINDOW addSubview:self];
    
    [UIView animateWithDuration:0.25 animations:^{
        
        self.alpha = 1;
        
    }];
}

-(void) hide{
    
    [self removeFromSuperview];
}

@end
