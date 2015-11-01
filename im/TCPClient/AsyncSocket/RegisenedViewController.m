//
//  RegisenedViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-23.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "RegisenedViewController.h"
#import "firstViewController.h"
#import "Photo.h"
#import "firstViewController.h"

@interface RegisenedViewController ()

@end

@implementation RegisenedViewController

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
   
    resigenedPW.delegate=self;
    resigenedNM.delegate=self;
    Picker.delegate=self;
    UITapGestureRecognizer *tap=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(addHeaderView)];
    tap.numberOfTapsRequired=1;
    tap.numberOfTouchesRequired=1;
    HeaderView.userInteractionEnabled=YES;
    [HeaderView addGestureRecognizer:tap];
    
    [tap release];
}

#pragma  mark - 选择头像

-(void)addHeaderView
{
    UIActionSheet *actionsheet=[[UIActionSheet alloc]initWithTitle:nil delegate:self cancelButtonTitle:@"取消" destructiveButtonTitle:nil otherButtonTitles:@"从相册中选一张", @"照一张", nil];
    actionsheet.actionSheetStyle=UIActionSheetStyleAutomatic;
    [actionsheet showInView:self.view];
    
    [actionsheet release];
}
-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    Picker = [[UIImagePickerController alloc]init];
    Picker.allowsEditing=YES;
    Picker.delegate=self;
    if(buttonIndex==0){//选择从相册中选一张
        Picker.sourceType=UIImagePickerControllerSourceTypePhotoLibrary;
        Picker.allowsEditing=YES;
        [self presentViewController:Picker animated:YES completion:nil];
    }else if(buttonIndex == 1){//选择照一张
        Picker.sourceType=UIImagePickerControllerSourceTypeCamera;
        Picker.allowsEditing=YES;
        [self presentViewController:Picker animated:YES completion:nil];
    }else{//选择取消
        [Picker dismissViewControllerAnimated:YES completion:nil];
    }
    [Picker release];
}

#pragma mark - 得到相片

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    UIImage *imagestr=[info objectForKey:@"UIImagePickerControllerEditedImage"];
    [Picker dismissViewControllerAnimated:YES completion:^{
        HeaderView.image=[Photo scaleImage:imagestr toWidth:88  toHeight:69];
        CALayer *layer=[HeaderView layer];
        [layer setMasksToBounds:YES];
        [layer setCornerRadius:6];
        NSMutableDictionary *dic=[NSMutableDictionary dictionary];
        [dic setObject:[Photo image2String:imagestr] forKey:@"image"];
        [[NSNotificationCenter defaultCenter] postNotificationName:@"image" object:nil userInfo:dic];
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"first"];
        [[NSUserDefaults standardUserDefaults] setObject:[Photo image2String:imagestr] forKey:@"image1"];
    }];
}

- (void)goback:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark - 改变键盘的高度

-(void)viewDidAppear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(changeframe:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(changefrom:) name:UIKeyboardWillHideNotification object:nil];
}
-(void)changeframe:(NSNotification *) notification
{
    [CATransaction begin];
    [UIView animateWithDuration:0.4f animations:^{
        [self.view setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y-60, self.view.frame.size.width, self.view.frame.size.height)];
    } completion:^(BOOL finished) {
        
    }];
    [CATransaction commit];
}
-(void)changefrom:(NSNotification *) notification
{
    [CATransaction begin];
    [UIView animateWithDuration:0.4f animations:^{
        [self.view setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y+60, self.view.frame.size.width, self.view.frame.size.height)];
    } completion:^(BOOL finished) {
        
    }];
    [CATransaction commit];
}
-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}
- (void)dealloc {
    [HeaderView release];
    [super dealloc];
}


- (IBAction)Regisened:(id)sender {
    if([resigenedPW.text isEqualToString:@""]||[resigenedNM.text isEqualToString:@""]){
        UIAlertView *al=[[UIAlertView alloc]initWithTitle:@"用户名密码不能唯空" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [al show];
        [al release];
    }else{
        NSString *nameSTR=resigenedNM.text;
        NSString *passwordSTR=resigenedPW.text;
        NSUserDefaults *defaul=[NSUserDefaults standardUserDefaults];
        [self dismissViewControllerAnimated:YES completion:nil];
        [defaul setObject:nameSTR forKey:@"name"];
        [defaul setObject:passwordSTR forKey:@"password"];
    }
}
@end
