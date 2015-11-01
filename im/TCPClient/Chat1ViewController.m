//
//  Chat1ViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-12.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "Chat1ViewController.h"
#import "Change.h"
#import "staic.h"
#import "AsyncSocket.h"
#import "AppDelegate.h"
#import "Time.h"
#import "messageCell.h"
#import "firstViewController.h"
#import "SocketDown.h"
#import "NSData+Base64.h"
#import "NSString+Base64.h"
#import "ChatVoiceRecorderVC.h"
#import "Photo.h"
#import "UIView+Animation.h"
#import "ImageViewController.h"
#import "frindViewController.h"
#import "tbViewController.h"
#import "messageCell1.h"
#import "imageVIewCell.h"
#import "yuyinCell.h"
#import "imageViweCell1.h"
#import "yuynCell1.h"


#define padd 20
#define _width 20
#define _high 30
@interface Chat1ViewController ()

@end

@implementation Chat1ViewController
@synthesize chaname,chasocket,messagetextfiled,sendMs,Message,voice,timeArray,refresh,dataarray;

- (void)viewDidLoad
{
    [super viewDidLoad];
    Message=[[NSMutableArray alloc]initWithCapacity:0];
    chatsocket =[[AsyncSocket alloc]initWithDelegate:self];
	messagetableview.separatorStyle=UITableViewCellSeparatorStyleNone;
    UITapGestureRecognizer *tap=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(viewTapped:)];
    tap.cancelsTouchesInView=YES;
    [self.view addGestureRecognizer:tap];
    
//网络连接
    
    [chatsocket connectToHost:IP onPort:PORT error:nil];
    messagetextfiled.delegate=self;
    
//接收到的数据消息处理
    
    sendmessage=[NSMutableArray array];
    messagetableview.delegate=self;
    messagetableview.dataSource=self;
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(addData:) name:@"dataNotification" object:nil];
    voice=[[voide alloc]init];
    imagearray=[[NSMutableArray alloc]initWithCapacity:0];
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(addmessagedata:) name:@"messagedata" object:nil];
    messagetableview.bounces=YES;
    self.timeArray=[[NSMutableArray alloc]init];
    
//*********下拉刷新*************//
    
    if(_refreshHeadViewController == nil){
        EGORefreshTableHeaderView * headview=[[EGORefreshTableHeaderView alloc] initWithFrame:CGRectMake(0,0- messagetableview.bounds.size.height-180, self.view.frame.size.width, self.view.frame.size.height)];
        headview.delegate=self;
        [messagetableview addSubview:headview];
        _refreshHeadViewController=headview;
    }
    [_refreshHeadViewController refreshLastUpdatedDate];
}
-(void)reloadTableviewData
{
    _reloading=YES;
}
-(void)doneLoadingTableViewData
{
    _reloading=NO;
    [_refreshHeadViewController egoRefreshScrollViewDataSourceDidFinishedLoading:messagetableview];
}

#pragma mark - uiscrollviewdelegate methods

-(void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    [_refreshHeadViewController egoRefreshScrollViewDidScroll:scrollView];
}

-(void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
    [_refreshHeadViewController egoRefreshScrollViewDidEndDragging:scrollView];
}

#pragma mark - EGORefreshHeaderDelegate Methods

-(void)egoRefreshTableHeaderDidTriggerRefresh:(EGORefreshTableHeaderView *)view
{
    [self reloadTableviewData];
    [self performSelector:@selector(doneLoadingTableViewData) withObject:nil afterDelay:3];
}
-(BOOL)egoRefreshTableHeaderDataSourceIsLoading:(EGORefreshTableHeaderView *)view

{
    return _reloading;
}
-(NSDate*)egoRefreshTableHeaderDataSourceLastUpdated:(EGORefreshTableHeaderView *)view
{
    return [NSDate date];
}

-(void)addmessagedata:(NSNotification *) not{
    if([not userInfo])
    {
        NSDictionary *messagedic=[not userInfo];
        [Message addObject:messagedic];
    }
}

//加载tableviewcell

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [Message count];
    
}
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSMutableDictionary *dict=[Message objectAtIndex:indexPath.row];
    NSString *mes=[dict objectForKey:@"mesage"];
    UIImageView *photo;
    if ([[dict objectForKey:@"senter"] isEqualToString:@"you"]) {
        if([dict objectForKey:@"voice"])
        {
            NSString *str1=[NSString stringWithFormat:@"cell%d%d",[indexPath section],[indexPath row]];
            yuyinCell *cell=[tableView dequeueReusableCellWithIdentifier:str1];
            if(cell==nil)
            {
                cell=[[yuyinCell alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str1];
            }else{
                while ([cell.contentView.subviews lastObject]!=nil) {
                    [(UIView *)[cell.contentView.subviews lastObject] removeFromSuperview];
                }
            }
            cell.selectionStyle=UITableViewCellSelectionStyleNone;
            cell.userInteractionEnabled = YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(320-60, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo1"];
            [cell.contentView addSubview:photo];
            [cell.contentView addSubview:[self yuying:voice.recordTime from:YES withIndexRow:indexPath.row withPosition:65 filepath:indexPath.row datasour:Message]];
            return cell;
            
        }
        else  if([dict objectForKey:@"image"])
        {
            NSString *str1=[NSString stringWithFormat:@"cell%d%d",[indexPath section],[indexPath row]];
            imageVIewCell *cell=[tableView dequeueReusableCellWithIdentifier:str1];
            if(cell==nil)
            {
                cell=[[imageVIewCell alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str1];
            }else{
                while ([cell.contentView.subviews lastObject]!=nil) {
                    [(UIView *)[cell.contentView.subviews lastObject] removeFromSuperview];
                }
            }
            cell.selectionStyle=UITableViewCellSelectionStyleNone;
            cell.userInteractionEnabled = YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(320-60, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo1"];
            [cell.contentView addSubview:photo];
            
            [cell.contentView addSubview:[self Image:[dict objectForKey:@"image"] from:YES withPosition:65]];
            return cell;
            
        }else if([dict objectForKey:@"mesage"]){
            
            NSString *str1=[NSString stringWithFormat:@"cell%d%d",[indexPath section],[indexPath row]];
            messageCell *cell=[tableView dequeueReusableCellWithIdentifier:str1];
            if(cell==nil)
            {
                cell=[[messageCell alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str1];
            }else{
                while ([cell.contentView.subviews lastObject]!=nil) {
                    [(UIView *)[cell.contentView.subviews lastObject] removeFromSuperview];
                }
            }
            cell.selectionStyle=UITableViewCellSelectionStyleNone;
            cell.userInteractionEnabled = YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(320-60, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo1"];
            [cell.contentView addSubview:photo];
            
            [cell.contentView addSubview:[self bubbleview:mes from:YES withPosition:65]];
            return cell;
        }
        
    }else
    {
        if([dict objectForKey:@"mesage"])
        {
            NSString *str1=[NSString stringWithFormat:@"cell%d%d",[indexPath section],[indexPath row]];
            messageCell1 *cell1=[tableView dequeueReusableCellWithIdentifier:str1];
            if(cell1==nil)
            {
                cell1=[[messageCell1 alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str1];
            }else{
                while ([cell1.contentView.subviews lastObject]!=nil) {
                    [(UIView *)[cell1.contentView.subviews lastObject] removeFromSuperview];
                }
            }
            cell1.selectionStyle=UITableViewCellSelectionStyleNone;
            cell1.userInteractionEnabled = YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(10, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo"];
            [cell1.contentView addSubview:photo];
            [cell1.contentView addSubview:[self bubbleview:mes from:NO withPosition:65]];
            
            return cell1;
        }
        else if ([dict objectForKey:@"image"])
        {
            NSString *str1=[NSString stringWithFormat:@"cell%d%d",[indexPath section],[indexPath row]];
            imageViweCell1 *cell1=[tableView dequeueReusableCellWithIdentifier:str1];
            if(cell1==nil)
            {
                cell1=[[imageViweCell1 alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str1];
            }else{
                while ([cell1.contentView.subviews lastObject]!=nil) {
                    [(UIView *)[cell1.contentView.subviews lastObject] removeFromSuperview];
                }
            }
            cell1.selectionStyle=UITableViewCellSelectionStyleNone;
            cell1.userInteractionEnabled = YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(10, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo"];
            [cell1.contentView addSubview:photo];
            [cell1.contentView addSubview:[self Image:[dict objectForKey:@"image"] from:NO withPosition:65]];
            return cell1;
        }
        if([dict objectForKey:@"voice"])
        {
            NSString *str1=[NSString stringWithFormat:@"cell%d%d",[indexPath section],[indexPath row]];
            yuynCell1 *cell1=[tableView dequeueReusableCellWithIdentifier:str1];
            if(cell1==nil)
            {
                cell1=[[yuynCell1 alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str1];
            }
            else {
                while ([cell1.contentView.subviews lastObject]!=nil) {
                    [(UIView *)[cell1.contentView.subviews lastObject] removeFromSuperview];
                }
            }
            cell1.selectionStyle=UITableViewCellSelectionStyleNone;
            cell1.userInteractionEnabled = YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(10, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo"];
            [cell1.contentView addSubview:photo];
            [cell1.contentView addSubview:[self yuying:0 from:NO withIndexRow:indexPath.row withPosition:65 filepath:indexPath.row datasour:Message]];
            return cell1;
        }
    }
    return nil;
}
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}
-(void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    NSLog(@"播放完毕");
}

#pragma  mark 对图片的处理--------------------

-(void)changeImage
{
    ImageViewController *i=[[ImageViewController alloc]init];
    [self presentViewController:i animated:YES completion:nil];
    [[NSNotificationCenter defaultCenter]postNotificationName:@"imagedata" object:imagearray];
    
    [i release];
}
-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    NSMutableDictionary *dict=[Message objectAtIndex:indexPath.row];
    if([dict objectForKey:@"mesage"]||[dict objectForKey:@"voice"]){
        NSString *senter=[dict objectForKey:@"mesage"];
        CGSize textSize = {260.0 , 10000.0};
        CGSize size=[senter sizeWithFont:[UIFont systemFontOfSize:14.0f]constrainedToSize:textSize lineBreakMode:NSLineBreakByWordWrapping];
        return size.height+65;
    }else{
        UIImage *image = [self scaleImage:[dict objectForKey:@"image"] toScale:0.1];
        return  image.size.height+65;
    }
}
-(void)viewWillAppear:(BOOL)animated{
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(keyboardWillshow:) name:UIKeyboardWillChangeFrameNotification object:nil];
}
-(void)viewWillDisappear:(BOOL)animated{
    [[NSNotificationCenter defaultCenter]removeObserver:self name:UIKeyboardWillChangeFrameNotification object:nil];
}
-(void)keyboardWillshow:(NSNotification *)aNotifacation{
    
    //获取到键盘frame 变化之前的frame
    NSValue *keyboardBeginBounds=[[aNotifacation userInfo]objectForKey:UIKeyboardFrameBeginUserInfoKey];
    CGRect beginRect=[keyboardBeginBounds CGRectValue];
    
    //获取到键盘frame变化之后的frame
    NSValue *keyboardEndBounds=[[aNotifacation userInfo]objectForKey:UIKeyboardFrameEndUserInfoKey];
    
    CGRect endRect=[keyboardEndBounds CGRectValue];
    
    CGFloat deltaY=endRect.origin.y-beginRect.origin.y;
    //拿frame变化之后的origin.y-变化之前的origin.y，其差值(带正负号)就是我们self.view的y方向上的增量
    [CATransaction begin];
    
    [UIView animateWithDuration:0.4f animations:^{
        [self.view setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y+deltaY, self.view.frame.size.width, self.view.frame.size.height)];
        [messagetableview setContentInset:UIEdgeInsetsMake(messagetableview.contentInset.top-deltaY, 0, 0, 0)];
        
    } completion:^(BOOL finished)
    {
        if (Message.count)
        {
            [messagetableview scrollToRowAtIndexPath:[NSIndexPath indexPathForRow:Message.count-2 inSection:0] atScrollPosition:UITableViewScrollPositionBottom animated:YES];
        }
    }];
    [CATransaction commit];
}

-(void)viewTapped:(UIGestureRecognizer *)tap
{
    [self.view endEditing:YES];
}
-(BOOL)textFieldShouldReturn:(UITextField *)textField{
    [textField resignFirstResponder];
    return YES;
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void)dealloc {
    [messagetextfiled release];
    [messagetableview release];
    [[NSNotificationCenter defaultCenter]removeObserver:self];
    [super dealloc];
}

- (IBAction)sendmessage:(id)sender
{
   message=messagetextfiled.text;

//数据整理

    if([message isEqualToString:@""]||message==nil){
        UIAlertView *alert=[[UIAlertView alloc]initWithTitle:@"发送失败" message:@"发送内容为空" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
        [alert show];
        
    }else{
        [self sendMessageString:message];
        [self addDatatoDic];
    }
    messagetextfiled.text=@"";
    [messagetextfiled resignFirstResponder];
    [messagetableview reloadData];
}


#pragma  mark - 泡泡的创建方法///////////////////////////////////////////////////////////////////////////////////////////////////////////////
-(UIView *)bubbleview:(NSString *)text from:(BOOL)fromSelf withPosition:(int)position
{
    UIFont *fond=[UIFont systemFontOfSize:14];
    CGSize size=[text sizeWithFont:fond constrainedToSize:CGSizeMake(180, 20000) lineBreakMode:NSLineBreakByCharWrapping];
    
    UIView *returnview=[[UIView alloc]initWithFrame:CGRectZero];
    returnview.backgroundColor=[UIColor clearColor];
    
    //背景图片
    UIImage *image=[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:fromSelf?@"SenderAppNodeBkg_HL":@"ReceiverTextNodeBkg" ofType:@"png"]];
    
    UIImageView *bubbleImageview=[[UIImageView alloc]initWithImage:[image stretchableImageWithLeftCapWidth:floorf(image.size.width/2) topCapHeight:floorf(image.size.height/2)]];
    NSLog(@"------%f %f",size.width,size.height);
    
    //添加文本信息
    UILabel *lable=[[UILabel alloc]initWithFrame:CGRectMake(fromSelf?15:22, 20, size.width+10, size.height+10)];
    lable.font=fond;
    lable.numberOfLines=0;
    lable.lineBreakMode=NSLineBreakByCharWrapping;
    lable.text=text;
    
    bubbleImageview.frame=CGRectMake(0, 14, lable.frame.size.width+30, lable.frame.size.height+20);
    if(fromSelf)
    {
        returnview.frame=CGRectMake(320-position-(lable.frame.size.width+30), 0, lable.frame.size.width+30, lable.frame.size.height+30);
    }
    else{
        returnview.frame=CGRectMake(position, 0, lable.frame.size.width+30, lable.frame.size.height+30);
    }
    [returnview addSubview:bubbleImageview];
    [returnview addSubview:lable];
    
    [lable release];
    [bubbleImageview release];
    
    return [returnview autorelease];
}


#pragma mark - 语音泡泡

-(UIView *)yuying:(NSInteger )logntime from:(BOOL)fromself withIndexRow:(NSInteger)indexrow withPosition:(int)position filepath:(NSInteger )filepath datasour:(NSMutableArray *)array
{
    //根据语音的长度
    
    dataarray=[NSMutableArray array];
    dataarray=array;
    AVAudioPlayer *avaudio=[[AVAudioPlayer alloc]initWithContentsOfURL:[NSURL fileURLWithPath:[[dataarray objectAtIndex:filepath] objectForKey:@"voice"]] error:nil];
    NSTimeInterval  time=avaudio.duration;
    
    int  yuyinwidth=66+fromself;
    UIButton *button=[UIButton buttonWithType:UIButtonTypeCustom];
    button.tag=indexrow;
    if(fromself)
        button.frame=CGRectMake(320-position-yuyinwidth, 10, yuyinwidth, 54);
    else
        button.frame=CGRectMake(position, 10, yuyinwidth, 54);
    
    //image的偏移量
    
    UIEdgeInsets imageInset;
    imageInset.top=-10;
    imageInset.left=fromself?button.frame.size.width/3:-button.frame.size.width/3;
    button.imageEdgeInsets=imageInset;
    [button setImage:[UIImage imageNamed:fromself?@"SenderVoiceNodePlaying":@"ReceiverVoiceNodePlaying@2x"] forState:UIControlStateNormal];
    [button addTarget:self action:@selector(readvoice:) forControlEvents:UIControlEventTouchUpInside];
    button.tag=filepath;
    UIImage *bakeimage=[UIImage imageNamed:fromself?@"SenderVoiceNodeDownloading":@"ReceiverVoiceNodeDownloading"];
    bakeimage=[bakeimage stretchableImageWithLeftCapWidth:20 topCapHeight:0];
    [button setBackgroundImage:bakeimage forState:UIControlStateNormal];
    UILabel  *lable=[[UILabel alloc]initWithFrame:CGRectMake(fromself?-30:button.frame.size.width, 0, 30, button.frame.size.height)];
    lable.text=[NSString stringWithFormat:@"%d \"", (NSInteger)time];
    lable.textColor=[UIColor grayColor];
    lable.font=[UIFont systemFontOfSize:13];
    lable.backgroundColor=[UIColor clearColor];
    [button addSubview:lable];
    
    return button;
}

#pragma mark - 播放语音

-(void)readvoice:(id)sender
{
    NSInteger i=((UIButton *)sender).tag;
    NSURL *fileURL=[[NSURL alloc]initFileURLWithPath:[[dataarray objectAtIndex:i] objectForKey:@"voice"]];
    NSLog(@"fileURL =%@",fileURL);
    audioPlayer=[[AVAudioPlayer alloc]initWithContentsOfURL:fileURL error:nil];
    [audioPlayer setDelegate:self];
    NSLog(@"持续的时间 :%f",audioPlayer.duration);
    if([audioPlayer prepareToPlay])
    {
        [audioPlayer play];
        
    }
    fileURL=nil;
    
}

#pragma mark - 图片泡泡

-(UIView *)Image:(UIImage *)image from:(BOOL)fromself withPosition:(NSInteger)position
{
    UIImage *nowImage=[self scaleImage:image toScale:0.1];
    CGSize size=nowImage.size;
    UIView *returnview=[[UIView alloc]initWithFrame:CGRectZero];
    returnview.backgroundColor=[UIColor clearColor];
    
    //背景图片
    UIImage *bubbleimage=[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:fromself?@"SenderAppNodeBkg_HL":@"ReceiverTextNodeBkg" ofType:@"png"]];
    
    UIImageView  *bubbleimageview=[[UIImageView alloc]initWithImage:[bubbleimage stretchableImageWithLeftCapWidth:floorf(bubbleimage.size.width/2) topCapHeight:floorf(bubbleimage.size.height/2)]];
    
    //发送的图片
    UIImageView *imageview= [[UIImageView alloc]initWithFrame:CGRectMake(fromself?10:17, 19, size.width+10,size.height+10)];
    // imageview.contentMode=UIViewContentModeScaleAspectFit;
    imageview.image=image;
    CALayer *al=[imageview layer];
    [al setMasksToBounds:YES];
    [al setCornerRadius:6];
    //添加手势
    
    UITapGestureRecognizer*tap=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(changeImage)];
    tap.cancelsTouchesInView=YES;
    tap.numberOfTapsRequired=1;
    tap.numberOfTouchesRequired=1;
    imageview.userInteractionEnabled=YES;
    
    tapimage=image;
    [imageview addGestureRecognizer:tap];
    
    bubbleimageview.frame=CGRectMake(0, 14, imageview.frame.size.width+26, imageview.frame.size.height+20);
    
    if(fromself)
        returnview.frame=CGRectMake(320-position-(imageview.frame.size.width+30), 0, imageview.frame.size.width+30, imageview.frame.size.height+30);
    else
        returnview.frame=CGRectMake(position, 0, imageview.frame.size.width+30, imageview.frame.size.height+30);
    [returnview addSubview:bubbleimageview];
    [returnview addSubview:imageview];
    
    [tap release];
    return [returnview autorelease];
}

#pragma  mark - 图片的等比缩放

-(UIImage *)scaleImage:(UIImage *)image toScale:(float)scaleSize
{
    UIGraphicsBeginImageContext(CGSizeMake(image.size.width *scaleSize, image.size.height*scaleSize));
    [image drawInRect:CGRectMake(0, 0, image.size.width*scaleSize, image.size.height * scaleSize)];
    UIImage *scaleimage=UIGraphicsGetImageFromCurrentImageContext();
    return  scaleimage;
}

////////向服务器发送请求数据/////////////////////////


-(void)sendMessageString:(NSString *)datamessage
{
    NSUserDefaults *de=[NSUserDefaults standardUserDefaults];
    NSString *str1= [de objectForKey:@"textname"];
    struct message _message;
    _message.msgcmd=20001;
    _message.group=1;
    _message.playerID=chaname.playerid;
    strcpy(_message.playerNm, [str1 cStringUsingEncoding:NSASCIIStringEncoding]);
    _message.type=1;
    _message.strlen=strlen([datamessage cStringUsingEncoding:NSASCIIStringEncoding])+1;
    strcpy(_message.strmsg, [datamessage cStringUsingEncoding:NSASCIIStringEncoding]);
    

    struct senduser _senduser;
    _senduser.subcmd =2006;
    _senduser.length = sizeof(_message) - sizeof(_message.strmsg) + _message.strlen;
    _senduser.usercount=1;
    strcpy(_senduser.sendNm, [chaname.name cStringUsingEncoding:NSASCIIStringEncoding]);
    
    struct header _head;
    _head.flag=0;
    _head.len=sizeof(_senduser) + _senduser.length;
    _head.cmd=10006;
    

    NSOutputStream *stream1=[[NSOutputStream alloc]initToMemory];
    [stream1 setDelegate:self];
    [stream1 scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [stream1 open];
    [stream1 write:(uint8_t *)&_head maxLength:sizeof(_head)];
    [stream1 write:(uint8_t *)&_senduser maxLength:sizeof(_senduser)];
    [stream1 write:(uint8_t *)&_message maxLength:_senduser.length];
    NSData *data1=[stream1 propertyForKey:NSStreamDataWrittenToMemoryStreamKey];

    [chatsocket writeData:data1  withTimeout:-1 tag:0];
    [stream1 release];
}

//***************本人说的内容添加到数据源中****************//

-(void)addDatatoDic
{
    NSMutableDictionary *datadic=[NSMutableDictionary dictionary];
    [datadic setObject:@"you" forKey:@"senter"];
    [datadic setObject:message forKey:@"mesage"];
    [datadic setObject:[Time getCurrentTime] forKey:@"time"];
    [Message addObject:datadic];
    [messagetableview reloadData];
    
}

//**************他人传来的数据***************//

-(void)addData:(NSNotification *)notification
{
    NSDictionary *dic=[notification userInfo];
    [Message addObject:dic];
    [messagetableview reloadData];
    
}

#pragma mark ------传送语音

- (IBAction)sendTalk:(id)sender {
    [voice stopRecordWithCompletionBlock:^{
        
        if (voice.recordTime > 0.0f) {

            NSMutableDictionary *dict=[[NSMutableDictionary alloc]init];
            [dict setObject:voice.recordPath forKey:@"voice"];
            [dict setObject:[Time getCurrentTime] forKey:@"time"];
            [dict setObject:@"you" forKey:@"senter"];
            [Message addObject:dict];
            [messagetableview reloadData];
            [self sendvoice:voice.recordPath];
            NSData *data=[NSData dataWithContentsOfFile:voice.recordPath];
            NSLog(@"recode is ======%d",[data length]);
        }
    }];

 }



//****************发语音给服务器进行处理****************//

-(void)sendvoice:(NSString *)path
{
    SocketDown *socket=[[SocketDown alloc]init];
    [socket  sendVoiceToServer:path ImageNm:chaname.name playerID:chaname.playerid];
}

#pragma mark ------传送图片

- (IBAction)sendImageview:(id)sender
{
    UIActionSheet *actionsheet=[[UIActionSheet alloc]initWithTitle:nil delegate:self cancelButtonTitle:@"取消" destructiveButtonTitle:nil otherButtonTitles:@"从相册中选一张", @"照一张", nil];
    actionsheet.actionSheetStyle=UIActionSheetStyleAutomatic;
    [actionsheet showInView:self.view];
    [actionsheet release];
}

#pragma mark ------开始录音
- (IBAction)begintalk:(id)sender {
    [voice startRecordWithPath:[NSString stringWithFormat:@"%@/Documents/%@.caf",NSHomeDirectory(),[Time getCurrentTime]]];
}

- (IBAction)overTalk:(id)sender {
    [voice cancelled];
    UIAlertView * alert = [[UIAlertView alloc] initWithTitle:nil message:@"取消了" delegate:nil cancelButtonTitle:@"ok" otherButtonTitles:nil, nil];
    [alert show];
    [alert release];
}

#pragma mark --实现actionsheetdelegate--------


-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    
    imagepicker =[[UIImagePickerController alloc]init];
    imagepicker.delegate=self;
    imagepicker.allowsEditing=YES;
    if(buttonIndex==0){
        imagepicker.sourceType=UIImagePickerControllerSourceTypePhotoLibrary;
        imagepicker.allowsEditing=YES;
    }
    else{
        imagepicker.sourceType=UIImagePickerControllerSourceTypeCamera;
        imagepicker.allowsEditing=YES;
    }
    [self presentViewController:imagepicker animated:YES completion:nil];
}
-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    
    UIImage *imagestr=[info objectForKey:@"UIImagePickerControllerEditedImage"];
    [imagepicker dismissViewControllerAnimated:YES completion:^{
        [self Sendimage:imagestr];
        
        NSMutableDictionary *imagedic=[NSMutableDictionary dictionary];
        [imagedic setObject:@"you" forKey:@"senter"];
        [imagedic setObject:[Time getCurrentTime] forKey:@"time"];
        [imagedic setObject:imagestr forKey:@"image"];
        [Message addObject:imagedic];
        [messagetableview reloadData];
        
        [imagearray addObject:imagestr];
    }];
    
}

//***************发给服务器数据进行处理*****************//

-(void)Sendimage:(UIImage *)image
{
    SocketDown *socket=[[SocketDown alloc]init];
    [socket sendImageviewToServer:image Nm:chaname.name playerID:chaname.playerid];
    
}

- (IBAction)GobackFrind:(id)sender {
    
    [self dismissViewControllerAnimated:YES completion:nil];
    
}
@end
