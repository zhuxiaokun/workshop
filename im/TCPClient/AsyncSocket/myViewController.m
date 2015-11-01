//
//  myViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-16.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "myViewController.h"
#import "tbViewController.h"
#import "shaViewController.h"
@interface myViewController ()

@end

@implementation myViewController

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
    
	self.navigationItem.title=@"设置";
    moretableview =[[UITableView alloc]initWithFrame:CGRectMake(0, 70, SCREEN_WIDTH, SCREEN_HEIGHT - 20 - 44) style:UITableViewStylePlain];
    moretableview.delegate=self;
    moretableview.dataSource=self;
    moretableview.bounces=NO;
    but = [UIButton buttonWithType:UIButtonTypeCustom];
    but.frame = CGRectMake(10, 0, 310, 50);
    [but setTitle:@"退出" forState:UIControlStateNormal];
    [but setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [but addTarget:self action:@selector(goout) forControlEvents:UIControlEventTouchUpInside];
    datasoure=[[NSArray alloc]initWithObjects:@"二维码扫瞄",@"摇一摇" ,nil];
    self.view.backgroundColor=[UIColor whiteColor];
    moretableview.backgroundColor=[UIColor whiteColor];
    
    UINavigationBar *bar=[[UINavigationBar alloc]initWithFrame:CGRectMake(0, 20, 320, 44)];
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = CGRectMake(10, 10, 50, 30);
    [button addTarget:self action:@selector(Back) forControlEvents:UIControlEventTouchUpInside];
    [button setTitle:@"返回" forState:UIControlStateNormal];
    [button setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [self.view addSubview:bar];
    [bar addSubview:button];
    [self.view addSubview:moretableview];
    
    [bar release];
    [moretableview release];
}
-(void)Back{
    tbViewController *vc=[[tbViewController alloc]init];
    [self presentViewController:vc animated:YES completion:nil];
    [vc release];
}

-(void)goout
{
    exit(0);
}
-(NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    NSString *str=@"设置";
    return str;
}
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSLog(@"datasoure is %@",datasoure);
    return [datasoure count];
}
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return  1;
}
-(UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
{
    return but;
}
-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 30;
}
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *str=@"cell";
    UITableViewCell *cell=[tableView dequeueReusableHeaderFooterViewWithIdentifier:str];
    if(cell == nil){
        cell=[[[UITableViewCell alloc]initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:str]autorelease];
    }
    
    cell.textLabel.text=[datasoure objectAtIndex:indexPath.row];
    
    return cell;
    
}
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.row==0){
        
    }else{
        shaViewController *sv=[[shaViewController alloc]init];
        [self presentViewController:sv animated:YES completion:nil];
        [sv release];
    }
}

- (void)dealloc {
    [moretableview release];
    [datasoure release];
    [super dealloc];
}
- (IBAction)back:(id)sender {
}
@end
