//
//  groupChatViewController.m
//  TCPClient
//
//  Created by Kai Deng on 14-2-24.
//  Copyright (c) 2014年 PK. All rights reserved.
//

#import "groupChatViewController.h"
#import "groupMembersViewController.h"
#import "messageCell.h"
#import "Time.h"
#import "Chat1ViewController.h"
#import "yuyinCell.h"
#import "yuynCell1.h"
#import "messageCell1.h"
#import "imageViweCell1.h"
#import "imageVIewCell.h"

@interface groupChatViewController ()

@end

@implementation groupChatViewController
@synthesize Voide;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.title=@"群信息";
    
//点击空白区域键盘回收
    
    UITapGestureRecognizer *tap=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(viewTapped:)];
    tap.cancelsTouchesInView=YES;
    [self.view addGestureRecognizer:tap];

    
	UIButton *btu=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    btu.frame=CGRectMake(0, 10, 70, 50);
    [btu setTitle:@"群" forState:UIControlStateNormal];
    UIBarButtonItem *bar=[[UIBarButtonItem alloc]initWithCustomView:btu];
    self.navigationItem.leftBarButtonItem=bar;
    [btu addTarget:self action:@selector(goback) forControlEvents:UIControlEventTouchUpInside];
    
    
    UIButton *btu1=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    btu1.frame=CGRectMake(240, 10, 70, 50);
    [btu1 setTitle:@"群成员" forState:UIControlStateNormal];
    UIBarButtonItem *bar1=[[UIBarButtonItem alloc]initWithCustomView:btu1];
    [btu1 addTarget:self action:@selector(GoTogroupmembers) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.rightBarButtonItem=bar1;
    
    groupChattableview = [[UITableView alloc]initWithFrame:CGRectMake(0, 0, 320, [UIScreen mainScreen].bounds.size.height ) style:UITableViewStylePlain];
    groupChattableview.delegate=self;
    groupChattableview.dataSource=self;
    groupChattableview.separatorStyle=UITableViewCellSeparatorStyleNone;
    
    
//语音button
    UINavigationBar *navbar=[[UINavigationBar alloc]initWithFrame:CGRectMake(0, 450, 320, 40)];
    navbar.backgroundColor=[UIColor clearColor];
    
    UIButton *voidebutton=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    voidebutton.frame=CGRectMake(0,5, 40, 20);
    [voidebutton setTitle:@"语音" forState:UIControlStateNormal];
    [voidebutton addTarget:self action:@selector(Recodevoice) forControlEvents:UIControlEventTouchDown];
    [voidebutton addTarget:self action:@selector(sendvoice) forControlEvents:UIControlEventTouchUpInside];
    
//图片button
    
    UIButton *imagrbutton=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    imagrbutton.frame=CGRectMake(45,5, 40, 20);
    [imagrbutton setTitle:@"图片" forState:UIControlStateNormal];
    [imagrbutton addTarget:self action:@selector(chooseimage) forControlEvents:UIControlEventTouchDown];
    
//写文字区域
    
    textfiled=[[UITextField alloc]initWithFrame:CGRectMake(90, 5, 180, 20)];
    textfiled.font=[UIFont systemFontOfSize:14.0];
    textfiled.keyboardType=UIKeyboardAppearanceDefault
    ;
    textfiled.textAlignment=NSTextAlignmentLeft;
    textfiled.delegate=self;
    textfiled.backgroundColor=[UIColor lightGrayColor];
    textfiled.placeholder=@"请输入内容...";
    
//发送按钮
    
    UIButton *pressbutton=[UIButton buttonWithType:UIButtonTypeRoundedRect];
    pressbutton.frame=CGRectMake(275,5, 40, 20);
    [pressbutton setTitle:@"发送" forState:UIControlStateNormal];
    [pressbutton addTarget:self action:@selector(SendContent) forControlEvents:UIControlEventTouchDown];
    
    
    [navbar addSubview:pressbutton];
    [navbar addSubview:imagrbutton];
    [navbar addSubview:voidebutton];
    [navbar addSubview:textfiled];
    [self.view addSubview:groupChattableview];
   
    groupMessage=[[NSMutableArray alloc]init];
    chatView=[[Chat1ViewController alloc]init];
    Voide =[[voide alloc]init];
    
    [self.view addSubview:navbar];
    
    [tap release];
    [textfiled release];
    [navbar release];
    [bar release];
}

#pragma mark - 开始录音

//录音

-(void)Recodevoice
{
     [Voide startRecordWithPath:[NSString stringWithFormat:@"%@/Documents/%@.caf",NSHomeDirectory(),[Time getCurrentTime]]];
}

//发送语音

-(void)sendvoice
{
    
    //结束录音
    
    [Voide cancelled];
    
    [Voide stopRecordWithCompletionBlock:^{
        
        if (Voide.recordTime > 0.0f) {
            
          
            NSMutableDictionary *dict=[[NSMutableDictionary alloc]init];
            [dict setObject:Voide.recordPath forKey:@"voice"];
            [dict setObject:[Time getCurrentTime] forKey:@"time"];
            [dict setObject:@"you" forKey:@"senter"];
            [groupMessage addObject:dict];
            [groupChattableview reloadData];
         
          
            
        }
        
    }];
}

#pragma mark - 选择图片

-(void)chooseimage
{
    UIActionSheet *actionsheet=[[UIActionSheet alloc]initWithTitle:nil delegate:self cancelButtonTitle:@"取消" destructiveButtonTitle:nil otherButtonTitles:@"从相册中选一张", @"照一张", nil];
    actionsheet.actionSheetStyle=UIActionSheetStyleAutomatic;
    [actionsheet showInView:self.view];
    
    [actionsheet release];
}

#pragma mark - 发送内容

-(void)SendContent
{
  
    NSString * message=[NSString stringWithString:textfiled.text];
    if([message isEqualToString:@""]||message==nil){
        UIAlertView *alert=[[UIAlertView alloc]initWithTitle:@"发送失败" message:@"发送内容为空" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
        [alert show];
        [alert release];
        
    }else{
        
        NSMutableDictionary *datadic=[NSMutableDictionary dictionary];
        [datadic setObject:@"you" forKey:@"senter"];
        [datadic setObject:message forKey:@"mesage"];
        [datadic setObject:[Time getCurrentTime] forKey:@"time"];
        [groupMessage addObject:datadic];
        [groupChattableview reloadData];

        
        
    }
    
    textfiled.text=@"";
    [textfiled resignFirstResponder];
    [groupChattableview reloadData];
    
}


#pragma mark - 键盘的回收

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    
    return YES;
}

-(void)viewTapped:(UIGestureRecognizer *)tap
{
    [self.view endEditing:YES];
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
        [groupChattableview setContentInset:UIEdgeInsetsMake(groupChattableview.contentInset.top-deltaY, 0, 0, 0)];
        
    } completion:^(BOOL finished) {
        if (groupMessage.count) {
            [groupChattableview scrollToRowAtIndexPath:[NSIndexPath indexPathForRow:groupMessage.count-2 inSection:0] atScrollPosition:UITableViewScrollPositionBottom animated:YES];
        }
    }];
    [CATransaction commit];
    
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [groupMessage count];
}
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    NSMutableDictionary *dict=[groupMessage objectAtIndex:indexPath.row];
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
            //cell.imageContentView.userInteractionEnabled=YES;
            photo=[[UIImageView alloc]initWithFrame:CGRectMake(320-60, 10, 50, 50)];
            photo.image=[UIImage imageNamed:@"photo1"];
            [cell.contentView addSubview:photo];
            [cell.contentView addSubview:[chatView yuying:Voide.recordTime from:YES withIndexRow:indexPath.row withPosition:65 filepath:indexPath.row datasour:groupMessage]];
            
            
            
            
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
            
            [cell.contentView addSubview:[chatView Image:[dict objectForKey:@"image"] from:YES withPosition:65]];
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
            
            [cell.contentView addSubview:[chatView bubbleview:mes from:YES withPosition:65]];
            return cell;
        }

    }
    
    return nil;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSMutableDictionary *dict=[groupMessage objectAtIndex:indexPath.row];
    if([dict objectForKey:@"mesage"]||[dict objectForKey:@"voice"]){
        NSString *senter=[dict objectForKey:@"mesage"];
        CGSize textSize = {260.0 , 10000.0};
        CGSize size=[senter sizeWithFont:[UIFont systemFontOfSize:14.0f]constrainedToSize:textSize lineBreakMode:NSLineBreakByWordWrapping];
        return size.height+65;
    }else{
        UIImage *image = [chatView scaleImage:[dict objectForKey:@"image"] toScale:0.1];
        return  image.size.height+65;
    }
}

//群好友点击响应事件

-(void)GoTogroupmembers
{
    groupMembersViewController *gmembers=[[groupMembersViewController alloc] init];
    UINavigationController *na=[[UINavigationController alloc]initWithRootViewController:gmembers];
    [self presentViewController:na animated:YES completion:nil];
    
    [gmembers release];
    [na release];
}
-(void)goback
{
    [self dismissViewControllerAnimated:YES completion:nil];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma  mark - 实现actionsheetdelgate方法和imagedelgate方法


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
    [imagepicker release];
}
-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    
    UIImage *imagestr=[info objectForKey:@"UIImagePickerControllerEditedImage"];
    [imagepicker dismissViewControllerAnimated:YES completion:^{
       // [self Sendimage:imagestr];
        
        NSMutableDictionary *imagedic=[NSMutableDictionary dictionary];
        [imagedic setObject:@"you" forKey:@"senter"];
        [imagedic setObject:[Time getCurrentTime] forKey:@"time"];
        [imagedic setObject:imagestr forKey:@"image"];
        [groupMessage addObject:imagedic];
        [groupChattableview reloadData];
        
       // [imagearray addObject:imagestr];
    }];
    
}



@end
