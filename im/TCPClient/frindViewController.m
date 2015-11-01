//
//  frindViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-11.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "frindViewController.h"
#import "firstViewController.h"
#import "Chat1ViewController.h"
#import "Change.h"
#import "Cell1.h"
#import "Cell2.h"
#import "Photo.h"
#import "addfrindViewController.h"
#import "CreateGroupViewController.h"
#define MSSAGE 1
@interface frindViewController ()

@end

@implementation frindViewController
@synthesize isOpen,selectIndex,isChange;

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
	friendTableView.delegate=self;
    friendTableView.dataSource=self;
    
    if(!dataarray){
        dataarray=[[NSMutableArray alloc]init];
    }
    if([self respondsToSelector:@selector(data:)]){        NSNotificationCenter *notc=[NSNotificationCenter defaultCenter];
        messageArray =[[NSMutableArray alloc]initWithCapacity:0];
        isChange=YES;
        if (isChange) {
            dataarray=[[[NSUserDefaults standardUserDefaults]objectForKey:@"count"] objectForKey:@"count1"];
            isChange=NO;
        }
        [notc addObserver:self selector:@selector(data:) name:@"dataNotification" object:nil];
    }
    
    isOpen=NO;
}

-(void)reload:(NSNotification *)noti
{
    NSDictionary *dic=[noti userInfo];
    dataarray=[dic objectForKey:@"count1"];
    [friendTableView reloadData];
}
-(void)data:(NSNotification *)notification
{
    mesdict=[notification userInfo];
    [messageArray addObject:mesdict];
    [friendTableView reloadData];
    NSLog(@"mesdict--- :%@",mesdict);
}
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
    
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
//    if([dataarray count])
//        return [dataarray count];
//    else{
//        return 0;
//    }
    if (isOpen) {
        if(self.selectIndex.section ==section)
            return [dataarray count];
    }
    return 1;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    if(isOpen && selectIndex.section ==indexPath.section &&indexPath.row!=0){
    static  NSString *str=@"Cell2";
    Cell2 *cell=[tableView dequeueReusableCellWithIdentifier:str];
    
    if (cell==nil) {
        cell=[[[NSBundle mainBundle] loadNibNamed:@"cell2" owner:self options:nil]lastObject];
        
    }
    
    //cell.friendNm.text=[[dataarray objectAtIndex:indexPath.row] objectForKey:[[NSString alloc]initWithFormat:@"name%d",indexPath.row]];
    cell.headerIM.image=[Photo scaleImage:[Photo string2Image:[[NSUserDefaults standardUserDefaults] objectForKey:@"image"]] toWidth:51 toHeight:43];
    CALayer *layer=[cell.headerIM layer];
    [layer setMasksToBounds:YES];
    [layer setCornerRadius:6];
    cell.tag=1001;
    if(!messageArray.count){
        cell.topLABLE.hidden=YES;
        cell.topIM.hidden=YES;
    }
    if(![mesdict count]){
        cell.topLABLE.hidden=YES;
        cell.topIM.hidden=YES;
    }else if([cell.friendNm.text isEqualToString:[mesdict objectForKey:@"senter"]]){
        
        cell.topLABLE.text=[NSString stringWithFormat:@"%d",messageArray.count];
        
    }else
    {
        cell.topIM.hidden=YES;
    }
    return cell;
    }else
    {
        static NSString *strcell=@"cell1";
        Cell1*cell=[tableView dequeueReusableCellWithIdentifier:strcell];
        if(!cell)
        {
            cell = [[[NSBundle mainBundle]loadNibNamed:strcell owner:self options:nil]lastObject];
        }
        return cell;
    }
    
    
    
}
-(void)viewWillAppear:(BOOL)animated
{
    [friendTableView reloadData];
    if([self respondsToSelector:@selector(reload:)])
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(reload:) name:@"nscount" object:nil];
}
-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 54;
    
}
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    Chat1ViewController *vc1=[[Chat1ViewController alloc]initWithNibName:@"cView" bundle:nil];
    
//    [self presentViewController:vc1 animated:YES completion:^{
//        [messageArray removeAllObjects];
//        [friendTableView reloadData];
//    }];
    
//    Change *cha=[[Change alloc]init];
//    cha.name=[[dataarray objectAtIndex:indexPath.row]objectForKey:[[NSString alloc]initWithFormat:@"name%d",indexPath.row]];;
//    cha.playerid=*((int8_t *)[[dataarray objectAtIndex:indexPath.row]objectForKey:[[NSString alloc]initWithFormat:@"id%d",indexPath.row]]);
//    vc1.chaname=cha;
//    firstViewController *vc2=[[firstViewController alloc]init];
//    vc2.frindname=cha;
//    NSUserDefaults *defalu=[NSUserDefaults standardUserDefaults];
//    NSMutableDictionary *dic=[[NSMutableDictionary alloc]init];
//    [dic setObject:cha.name forKey:@"senter"];
//    [defalu setObject:dic forKey:@"shuju"];
//    [[NSNotificationCenter defaultCenter]postNotificationName:@"messagedata" object:nil userInfo:mesdict];
//    
//    [cha release];
//  //[vc1 release];
//    [vc2 release];
//    [dic release];
    if(indexPath.row == 0){
        if([indexPath isEqual:selectIndex])
        {
            isOpen = NO;
            [self didSelectCellRowFirstDo:NO nextDo:NO];
            selectIndex=nil;
        }else
        {
            if(!selectIndex){
                selectIndex = indexPath;
                [self didSelectCellRowFirstDo:YES nextDo:NO];
            }else
            {
                [self didSelectCellRowFirstDo:NO nextDo:YES];
            }
        }
    }else
    {
       Change *cha=[[Change alloc]init];
       cha.name=[[dataarray objectAtIndex:indexPath.row]objectForKey:[[NSString alloc]initWithFormat:@"name%d",indexPath.row]];;
       cha.playerid=*((int8_t *)[[dataarray objectAtIndex:indexPath.row]objectForKey:[[NSString alloc]initWithFormat:@"id%d",indexPath.row]]);
       vc1.chaname=cha;
       firstViewController *vc2=[[firstViewController alloc]init];
       vc2.frindname=cha;
       NSUserDefaults *defalu=[NSUserDefaults standardUserDefaults];
       NSMutableDictionary *dic=[[NSMutableDictionary alloc]init];
       [dic setObject:cha.name forKey:@"senter"];
       [defalu setObject:dic forKey:@"shuju"];
       [[NSNotificationCenter defaultCenter]postNotificationName:@"messagedata" object:nil userInfo:mesdict];

       [cha release];
  //[vc1 release];
       [vc2 release];
       [dic release];
    }
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

//折叠cell的制作方法

-(void)didSelectCellRowFirstDo:(BOOL)firstDoInsert nextDo:(BOOL)nextDoInsert{
    isOpen =firstDoInsert;
    //Cell1 *cell=(Cell1 *)[friendTableView cellForRowAtIndexPath:selectIndex];
    
    [friendTableView beginUpdates];
    
    int section = selectIndex.section;
    contentCount=[dataarray count];
    NSMutableArray *rowToInsert = [NSMutableArray array];
    
    for(NSUInteger i =1 ;i<contentCount+1 ;i++)
    {
        NSIndexPath *indexPathToInsert = [NSIndexPath indexPathForRow:i inSection:section];
        [rowToInsert addObject:indexPathToInsert];
    }
    
    if(firstDoInsert)
    {
        [friendTableView insertRowsAtIndexPaths:rowToInsert withRowAnimation:UITableViewRowAnimationTop];
    }else
    {
        [friendTableView deleteRowsAtIndexPaths:rowToInsert withRowAnimation:UITableViewRowAnimationTop];
    }
    
    //[friendTableView endUpdates];
    
    if(nextDoInsert){
        isOpen = YES;
        self.selectIndex = [friendTableView indexPathForSelectedRow];
        [self didSelectCellRowFirstDo:YES nextDo:NO];
    }
    if(isOpen){
        [friendTableView scrollToNearestSelectedRowAtScrollPosition:UITableViewScrollPositionTop animated:YES];
    }
}

//点击手势的方法

-(void)preseTap:(UILongPressGestureRecognizer*) longpress
{
    if(longpress.state == UIGestureRecognizerStateEnded){
        
        CGPoint location=[longpress locationInView:friendTableView];
        NSIndexPath *indexpath=[friendTableView indexPathForRowAtPoint:location];
        FriendIndexPath=[indexpath retain];
        
        
        UIAlertView *al=[[UIAlertView alloc]initWithTitle:@"是否删除好友" message:nil delegate:self cancelButtonTitle:@"是" otherButtonTitles:@"取消", nil];
        
        [al show];
        [al release];
    }
}

//添加手势

-(UILongPressGestureRecognizer *)addTap
{
    UILongPressGestureRecognizer *tap=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(preseTap:)];
    
    tap.cancelsTouchesInView=YES;
    tap.numberOfTouchesRequired=1;
    tap.minimumPressDuration=1;
    
    
    return [tap autorelease];
}

//alertview的delgate的方法

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex==0) {
        NSLog(@"%d",FriendIndexPath.row);
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
}

- (void)dealloc {
    [friendTableView release];
    
    [_PopupControls release];
    
    [_createGroupItem release];
    [super dealloc];
}

- (IBAction)Popcontrols:(id)sender {
    
    addfrindViewController *friend=[[addfrindViewController alloc]init];
    UINavigationController *na=[[UINavigationController alloc]initWithRootViewController:friend];
    [self presentViewController:na animated:YES completion:nil];
    
    [friend release];
    [na release];
    
}

- (IBAction)Creategroup:(id)sender {
    CreateGroupViewController *createGroupVC = [[CreateGroupViewController alloc] init];
    UINavigationController *groupNav = [[UINavigationController alloc] initWithRootViewController:createGroupVC];
   
    [self presentViewController:groupNav animated:YES completion:^{
    }];
    [createGroupVC release];
    [groupNav release];
}
@end
