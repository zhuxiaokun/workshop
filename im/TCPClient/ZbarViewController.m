//
//  ZbarViewController.m
//  TCPClient
//
//  Created by Kai Deng on 13-12-17.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "ZbarViewController.h"


@interface ZbarViewController ()

@end

@implementation ZbarViewController

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
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [TextFilde release];
    [imageView release];
    [TextFilde release];
    [imageView release];
    [super dealloc];
}
- (IBAction)BackView:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}



- (IBAction)ScanButton:(id)sender {
  reader=[ZBarReaderViewController new];
    reader.readerDelegate=self;
    ZBarImageScanner*scanner=reader.scanner;
    [scanner setSymbology:ZBAR_ADDON config:ZBAR_CFG_ENABLE to:0];
    [self presentViewController:reader animated:YES completion:nil];
    
    
}
-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    
    id<NSFastEnumeration>results=[info objectForKey:ZBarReaderControllerResults];
    
    ZBarSymbol*synbol=nil;
    for (synbol  in results) {
        break;
    }
    
    TextFilde.text=synbol.data;
    imageView.image=[info objectForKey:UIImagePickerControllerOriginalImage];
    [reader dismissViewControllerAnimated:YES completion:nil];
    
}
@end
