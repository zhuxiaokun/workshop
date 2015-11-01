//
//  Chat1ViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-12.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AsyncSocket.h"
#import "Change.h"
#import  "keyboard.h"
#import "firstViewController.h"
#import "EGORefreshTableHeaderView.h"
#import "voide.h"



@class OHAttributedLabel;

@interface Chat1ViewController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate,UITableViewDataSource,UITableViewDelegate,NSStreamDelegate,AsyncSocketDelegate,UIImagePickerControllerDelegate,UIActionSheetDelegate,UINavigationControllerDelegate,AVAudioPlayerDelegate,EGORefreshTableHeaderDelegate>{
    
    IBOutlet UITextField *messagetextfiled;
    IBOutlet UITableView *messagetableview;
    NSMutableString *messageString;
    NSMutableArray *sendmessage;
    NSMutableArray *recivemessage;
    NSString *message;
    AsyncSocket *chatsocket;
    NSString *chatwriteName;
    NSString *str;
    UIImagePickerController *imagepicker;
    NSString *originWav;
    AVAudioRecorder *recorder;
    UIImage *tapimage;
    NSMutableArray *imagearray;
    AVAudioPlayer *audioPlayer;
    NSInteger _currentSection;
    NSInteger _currentRow;
    NSMutableArray *wp_lableArray;
    NSMutableArray *wp_rowHeights;
    EGORefreshTableHeaderView *_refreshHeadViewController;
    BOOL _reloading;
    
}
@property (nonatomic, strong) NSMutableArray *Message;
- (IBAction)sendmessage:(id)sender;
@property (strong ,nonatomic) Change *chaname;
- (IBAction)sendTalk:(id)sender;
- (IBAction)sendImageview:(id)sender;
- (IBAction)begintalk:(id)sender;
- (IBAction)overTalk:(id)sender;

@property (strong ,nonatomic) Change *chasocket;
@property (strong ,nonatomic)UITextField *messagetextfiled;
@property (strong ,nonatomic) keyboard *sendMs;
@property (strong ,nonatomic)voide *voice;
@property (strong ,nonatomic)NSMutableArray * timeArray;
@property (strong ,nonatomic)UIRefreshControl *refresh;
@property (strong ,nonatomic)NSMutableArray *dataarray;

- (IBAction)GobackFrind:(id)sender;
- (void)reloadTableviewData;
- (void)doneLoadingTableViewData;

//语音泡泡

-(UIView *)yuying:(NSInteger )logntime from:(BOOL)fromself withIndexRow:(NSInteger)indexrow withPosition:(int)position filepath:(NSInteger )filepath datasour:(NSMutableArray *) array;

//图片泡泡

-(UIView *)Image:(UIImage *)image from:(BOOL)fromself withPosition:(NSInteger)position;

//文字泡泡

-(UIView *)bubbleview:(NSString *)text from:(BOOL)fromSelf withPosition:(int)position;

//图片缩放

-(UIImage *)scaleImage:(UIImage *)image toScale:(float)scaleSize;

@end
