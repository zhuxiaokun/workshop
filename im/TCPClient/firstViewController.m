//
//  firstViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-6.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "firstViewController.h"

#import "staic.h"
#import "chatViewController.h"
#import "frindViewController.h"
#import "AppDelegate.h"
#import "AsyncSocket.h"
#import "Change.h"
#import "Chat1ViewController.h"
#import "keyboard.h"
#import "Time.h"
#import "tbViewController.h"
#import "RegisenedViewController.h"
#import "Photo.h"
#import "ImData.h"
#import "chat_client_packet.h"
#import <CommonCrypto/CommonDigest.h>

# define DATA_RECIVE 1000

@interface firstViewController ()

@end

@implementation firstViewController
@synthesize userIdTextFiled,passwordTextFiled,messageDelegate,frindname,imagehead,isGO;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
       
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    imdata= [[ImData alloc]init];
    recivedata=[[NSMutableData alloc]init];
    ImageData =[[NSMutableData alloc]init];
    socket =[[AsyncSocket alloc]initWithDelegate:self];
    ipTextFiled.delegate=self;
    userIdTextFiled.delegate=self;
    passwordTextFiled.delegate=self;
    [socket connectToHost:IP onPort:PORT error:nil];
    //登陆用户图片
    UIImageView *iv=[[UIImageView alloc]initWithImage:[UIImage imageNamed:@"denglu.jpg"]];
    iv.frame=CGRectMake(0, 0, 320, 480);
    ipTextFiled.hidden=YES;
    headimage.image=[Photo scaleImage:[Photo string2Image:[[NSUserDefaults standardUserDefaults]objectForKey:@"image1"]] toWidth:90 toHeight:71];
    CALayer *layer=[headimage layer];
    [layer setMasksToBounds:YES];
    [layer setCornerRadius:6];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(presentimage:) name:@"image" object:nil];
    isGO=YES;

    ImageData =[[NSMutableData alloc]init];
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(addarray:) name:@"count" object:nil];
   
    [iv release];
}
-(void)addarray:(NSNotification *)not
{
    _dic =[not userInfo];
    _count=[[_dic objectForKey:@"Arraycount"] length];
}
-(void)presentimage:(NSNotification *)not
{
    NSDictionary *dict=[not userInfo];
    headimage.image=[Photo scaleImage:[Photo string2Image:[dict objectForKey:@"image"]] toWidth:90 toHeight:71];
    CALayer *layer=[headimage layer];
    [layer setMasksToBounds:YES];
    [layer setCornerRadius:6];
    [self.view reloadInputViews];
    [[NSUserDefaults standardUserDefaults]setBool:NO forKey:@"first"];
}
-(void)viewTapped:(UITapGestureRecognizer *)tap
{
    [self.view endEditing:YES];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void)dealloc {
    [ipTextFiled release];
    [headimage release];
    [super dealloc];
}
- (IBAction)Login:(id)sender {
    
    //************密码用户名设置*************//
    
    NSUserDefaults *deful=[NSUserDefaults standardUserDefaults];
    NSString *strNM=[deful objectForKey:@"name"];
    NSString *strPw=[deful objectForKey:@"password"];
    if([passwordTextFiled.text isEqualToString:@""]||[userIdTextFiled.text isEqualToString:@""]) {
        UIAlertView *al=[[UIAlertView alloc]initWithTitle:@"请输入用户名" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [al show];
        [al release];
    }else if(![strNM isEqualToString:userIdTextFiled.text]||![strPw isEqualToString:passwordTextFiled.text])
    {
        UIAlertView *al=[[UIAlertView alloc]initWithTitle:@"用户名或密码不正确" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        
        [al show];
        [al release];
    }else if([strNM isEqualToString:userIdTextFiled.text]&&[strPw isEqualToString:passwordTextFiled.text]){
        
        //************获得连接的节点***************//
        
        tbViewController *tb=[[tbViewController alloc]init];
        [self presentViewController:tb animated:YES completion:nil];
        isGO=NO;
        //[self sendText:self];
    }
}

- (IBAction)connectip:(id)sender {
    RegisenedViewController * rv=[[RegisenedViewController alloc]initWithNibName:@"regisenView" bundle:nil];
    [self presentViewController:rv animated:YES completion:nil];
    [rv release];
}
-(void)onSocket:(AsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port{
    NSLog(@"连接成功");
    [sock readDataWithTimeout:-1 tag:0];
}

#pragma mark - AsyncSocketDelegate

//调用这个方法就证明监听到了数据
- (void)onSocket:(AsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag{
    
    //从流中读取数据－－－－－－－－－－－－－－－－－－
    recivedata=[[NSMutableData alloc]init];
    [recivedata appendData:data];
    NSLog(@"data is %d",[recivedata length]);
    struct header head;
    struct user _users;
    struct message _message;
    struct intnational _intnational;
    NSInputStream *inputstream=[[NSInputStream alloc]initWithData:recivedata];
    [inputstream setDelegate:self];
    [inputstream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputstream open];
    
    //读取head的数据
    [inputstream read:(uint8_t *)&head maxLength:sizeof(head)];
    
    if(head.cmd!=2006&&head.cmd!=2005&&head.cmd!=2004){
        [imdata dealImageData:data socket:sock tag:tag];
    }else{
        NSInputStream *inputstream=[[NSInputStream alloc]initWithData:recivedata];
        [inputstream setDelegate:self];
        [inputstream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [inputstream open];
        
        //////////////////对接收到的聊天数据进行解析////////
        [inputstream read:(uint8_t *)&head maxLength:sizeof(head)];
        //NSLog(@"%d %d",[recivedata length],t);
        NSLog(@"%d,%d",head.cmd,head.len);
        if(head.cmd==2006){
            NSLog(@"接收到聊天数据");
            NSLog(@"%d %lu",[recivedata length],sizeof(_message));
            int Intmessage=[inputstream read:(uint8_t *)&_message maxLength:sizeof(_message)];
            NSLog(@"int message is %d %lu",Intmessage,sizeof(_message));
            Chat1ViewController *vc11=[[Chat1ViewController alloc]init];
            keyboard *change1=[[keyboard alloc]init];
            
            change1.sendMEssage=[NSString stringWithCString:_message.strmsg encoding:NSUTF8StringEncoding] ;
            vc11.sendMs=change1;
            NSMutableDictionary *dict = [NSMutableDictionary dictionary];
            [dict setObject:change1.sendMEssage forKey:@"mesage"];
            [dict setObject:[Time getCurrentTime] forKey:@"time"];
            [dict setObject:[NSString stringWithCString:_message.playerNm encoding:NSUTF8StringEncoding] forKey:@"senter"];
            NSLog(@"name is %@",[NSString stringWithCString:_intnational.userNm encoding:NSUTF8StringEncoding]);
            [[NSNotificationCenter defaultCenter]postNotificationName:@"dataNotification" object:nil userInfo:dict];
            NSUserDefaults *defaults=[NSUserDefaults standardUserDefaults];
            NSString*str=@"you";
            [defaults setObject:dict forKey:@"shujua"];
            [defaults setObject:str forKey:@"sender"];
            //NSLog(@"message is %s",_message.strmsg);
            [vc11 release];
            [change1 release];
        }
        
        //***********用户上线**************//
        
        if(head.cmd==2005 &&_dataarray!=nil){

            [inputstream read:(uint8_t *)&_users maxLength:sizeof(_users)];

            NSMutableArray *array=[NSMutableArray array];
            for(int i=0;i<_users.count;i++){
                [inputstream read:(uint8_t *)&_intnational maxLength:sizeof(_intnational)];
                NSMutableDictionary *diconuser=[NSMutableDictionary dictionary];
                [diconuser setObject:[NSString stringWithCString:_intnational.userNm encoding:NSUTF8StringEncoding ] forKey:[[NSString alloc]initWithFormat:@"name%d",i]];
                [diconuser setObject:[NSNumber numberWithInt:_intnational.userid] forKey:[[NSString alloc]initWithFormat:@"id%d",i]];
                [array addObject:diconuser];
                [_dataarray addObject:diconuser];
            }
            
            NSUserDefaults *def=[NSUserDefaults standardUserDefaults];
            [def setObject:array forKey:@"ListFrind"];
        }
        
        //---------------------------------------------//
        /////////////////denglu对数据进行处理///////////////
        
        if(head.cmd==2004){
            NSInteger _length=[recivedata length]-9;
            NSData *_llength=[recivedata subdataWithRange:NSMakeRange(9, _length)];
            [recivedata setData:_llength];
            
            [inputstream read:(uint8_t *)&head maxLength:sizeof(head)];
            [inputstream read:(uint8_t *)&_users maxLength:sizeof(_users)];
            _dataarray=[NSMutableArray array];
            NSMutableDictionary *dicuser = [NSMutableDictionary dictionary];
            if(head.cmd==2005){
                for(int i=0;i<_users.count;i++){
                    [inputstream read:(uint8_t *)&_intnational maxLength:sizeof(_intnational)];
                    [dicuser setObject:[NSString stringWithCString:_intnational.userNm encoding:NSUTF8StringEncoding ] forKey:[[NSString alloc]initWithFormat:@"name%d",i]];
                    [dicuser setObject:[NSNumber numberWithInt:_intnational.userid] forKey:[[NSString alloc]initWithFormat:@"id%d",i]];
                    [_dataarray addObject:dicuser];
                }
            }
            
            [[NSNotificationCenter defaultCenter]postNotificationName:@"message" object:nil userInfo:dicuser];
            
            recivedata=[[NSMutableData alloc]initWithCapacity:0];
        }
        
        NSMutableDictionary *dict1=[NSMutableDictionary dictionary];
        
        [dict1 setObject:_dataarray forKey:@"count1"];
        if(dict1 &&[_dataarray count]){
            [[NSUserDefaults standardUserDefaults] setObject:dict1 forKey:@"count"];
        }
        if([_dataarray count]){
            [[NSNotificationCenter defaultCenter]postNotificationName:@"nscount" object:nil userInfo:dict1];
            _dataarray=[[NSMutableArray alloc]initWithCapacity:0];
        }
        if(isGO){
            tbViewController *tb=[[tbViewController alloc]init];
            [self presentViewController:tb animated:YES completion:nil];
            isGO=NO;
            
            [tb release];
        }
        
        NSLog(@"-----------%@",_dataarray);
    }
    [sock readDataWithTimeout:-1 tag:0];
    
    [inputstream release];
}

//发送数据
- (void)sendText:(id)sender{
    //如果连接上的话
    struct header head;
    head.flag=0;
    head.len=80;
    head.cmd=s_nCCht_Login;
    //strcopy
    NSString *str=userIdTextFiled.text;
    NSUserDefaults *def=[NSUserDefaults standardUserDefaults];
    [def setObject:str forKey:@"textname"];
    struct CCht_Login _login;
    strcpy(_login.acctName,[str cStringUsingEncoding:NSASCIIStringEncoding] );
    NSLog(@"\%s",[[self md5HexDigext:passwordTextFiled.text] cStringUsingEncoding:NSASCIIStringEncoding]);
    memcpy(&_login.pwdMd5, [self md5HexDigext:passwordTextFiled.text], 16);
    //strcpy(_login.pwdMd5, [[self md5HexDigext:passwordTextFiled.text] cStringUsingEncoding:NSASCIIStringEncoding]);
    
    NSOutputStream *stream1=[[NSOutputStream alloc]initToMemory];
    [stream1 setDelegate:self];
    [stream1 scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [stream1 open];
    [stream1 write:(uint8_t *)&head maxLength:sizeof(head)];
    [stream1 write:(uint8_t *)&_login maxLength:sizeof(_login)];
    NSData *data1=[stream1 propertyForKey:NSStreamDataWrittenToMemoryStreamKey];
    [socket writeData:data1 withTimeout:-1 tag:0];
    
    //[stream1 release];
}
-(void)viewDidAppear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(changeframe:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(changefrom:) name:UIKeyboardWillHideNotification object:nil];
}
-(void)changeframe:(NSNotification *) notification
{
    [CATransaction begin];
    [UIView animateWithDuration:0.4f animations:^{
        [self.view setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y-20, self.view.frame.size.width, self.view.frame.size.height)];
    } completion:^(BOOL finished) {
        
    }];
    [CATransaction commit];
}
-(void)changefrom:(NSNotification *) notification
{
    [CATransaction begin];
    [UIView animateWithDuration:0.4f animations:^{
        [self.view setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y+20, self.view.frame.size.width, self.view.frame.size.height)];
    } completion:^(BOOL finished) {
        
    }];
    [CATransaction commit];
}
-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}


//调用这个方法就证明数据发送完成

- (void)onSocket:(AsyncSocket *)sock didWriteDataWithTag:(long)tag{
    NSLog(@"发送成功");
}


//md5加密

-(NSString *)md5HexDigext:(NSString *)password

{
    const char *original_str = [password UTF8String];
    unsigned char result[CC_MD5_DIGEST_LENGTH];
    CC_MD5(original_str, strlen(original_str), result);
    NSMutableString *hash = [NSMutableString string];
    for (int i = 0; i < 16; i++)
    {
        [hash appendFormat:@"%02X", result[i]];
    }
    NSString *mdfiveString = [hash lowercaseString];
    NSLog(@"Encryption Result = %@",mdfiveString);
    return mdfiveString;
}

@end
