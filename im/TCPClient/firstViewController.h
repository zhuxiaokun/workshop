//
//  firstViewController.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-6.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AsyncSocket.h"
#import "AppDelegate.h"
#import "Change.h"
#import "ImData.h"
@protocol firstViewdelegate <NSObject>

-(void )newMessageDatadic:(NSMutableDictionary *) dic;

@end
@interface firstViewController : UIViewController<AsyncSocketDelegate,NSStreamDelegate,UIApplicationDelegate,UITextFieldDelegate>{
    AsyncSocket *socket;
    IBOutlet UITextField *ipTextFiled;
    NSMutableArray *_dataarray;
    //NSObject<firstViewdelegate> *friendview;
    char name [62];
    IBOutlet UIImageView *headimage;
    NSMutableData *recivedata;
    NSMutableData *ImageData;
    NSMutableData *_data;
    NSDictionary *_dic;
    NSMutableData *_inputdata;
    ImData *imdata;
    int _count;
    BOOL isfirstFour;
    uint remainint;
    
}
@property (strong ,nonatomic)UIImageView *imagehead;
@property(strong ,nonatomic)Change *frindname;
- (IBAction)connectip:(id)sender;
@property (retain, nonatomic) IBOutlet UITextField *passwordTextFiled;
@property (retain, nonatomic) IBOutlet UITextField *userIdTextFiled;
- (IBAction)Login:(id)sender;
//- (IBAction)Rergist:(id)sender;
@property(nonatomic ,unsafe_unretained) id<firstViewdelegate> delegate;
@property(nonatomic ,strong) id messageDelegate;
@property(assign)BOOL isGO;
@end
