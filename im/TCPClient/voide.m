//
//  voide.m
//  TCPClient
//
//  Created by Kai Deng on 13-11-25.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import "voide.h"
#import "Voicehud.h"
#import "VoiceRecorderBaseVC.h"
#import <AVFoundation/AVFoundation.h>

#pragma mark - <DEFINES>

#define WAVE_UPDATE_FREQUENCY   0.05
@interface voide ()
{
}
@end

@implementation voide

- (void)dealloc
{
    if (self.recorder.isRecording) {
        [self.recorder stop];
    }
    self.recorder = nil;
    self.recordPath = nil;
    self.voiceHud = nil;
    self.timer = nil;
    [super dealloc];
}

-(void)startRecordWithPath:(NSString *)path
{
    NSError *err =nil;
    
    AVAudioSession *as=[AVAudioSession sharedInstance];
    [as setCategory:AVAudioSessionCategoryPlayAndRecord error:&err];
    if(err){
        NSLog(@"audioSession: %@ %d %@", [err domain], [err code], [[err userInfo] description]);
        return;
	}
    
    err =nil;
    
    [as setActive:YES error:&err];
    
    if(err){
        NSLog(@"audioSession: %@ %d %@", [err domain], [err code], [[err userInfo] description]);
        return;
	}
    NSMutableDictionary *recoderSetting=[NSMutableDictionary dictionary];
    [recoderSetting setValue:[NSNumber numberWithInt:kAudioFormatAppleIMA4] forKey:AVFormatIDKey];
    [recoderSetting setValue:[NSNumber numberWithFloat:16000.0] forKey:AVSampleRateKey];
    [recoderSetting setValue:[NSNumber numberWithInt:1] forKey:AVNumberOfChannelsKey];
    
    self.recordPath=path;
    NSURL * url=[NSURL fileURLWithPath:self.recordPath];
    err =nil;
    NSData *audiodata=[NSData dataWithContentsOfFile:[url path] options:0 error:&err];
    if(audiodata)
    {
        NSFileManager *fm=[NSFileManager defaultManager];
        [fm removeItemAtPath:[url path] error:&err];
    }
    
    err =nil;
    if(self.recorder){
        [self.recorder stop];
        self.recorder=nil;
        
    }
    self.recorder =[[[AVAudioRecorder alloc]initWithURL:url settings:recoderSetting error:&err] autorelease];
    self.recorder.meteringEnabled = YES;
    self.recorder.delegate = self;
    if ([self.recorder prepareToRecord] == YES) {
        [self.recorder record];
        [self resetTimer];
        self.timer=[NSTimer scheduledTimerWithTimeInterval:WAVE_UPDATE_FREQUENCY target:self selector:@selector(updateMeters) userInfo:nil repeats:YES];
        [self showVoiceHudOrHide:YES];
    }else{
        NSLog(@"recorder: %@ %d %@", [err domain], [err code], [[err userInfo] description]);
        UIAlertView *alert =
        [[UIAlertView alloc] initWithTitle: @"Warning"
								   message: [err localizedDescription]
								  delegate: nil
						 cancelButtonTitle:@"OK"
						 otherButtonTitles:nil];
        [alert show];
        [alert release];
        return;
    }
    
 
//    [_recorder setDelegate:self];
//   // [_recorder prepareToRecord];
//    _recorder.meteringEnabled=YES;
//    [_recorder recordForDuration:(NSTimeInterval)10];
//    self.recordTime=0;
//    [self resetTimer];
//    
//    self.timer=[NSTimer scheduledTimerWithTimeInterval:WAVE_UPDATE_FREQUENCY target:self selector:@selector(updateMeters) userInfo:nil repeats:YES];
//    [self showVoiceHudOrHide:YES];
}
-(void)stopRecordWithCompletionBlock:(void (^)())completion
{
    dispatch_async(dispatch_get_main_queue(), completion);
    [_recorder stop];
    [self resetTimer];
    [self showVoiceHudOrHide:NO];
}

#pragma  mark time update

-(void)updateMeters{
    self.recordTime+=WAVE_UPDATE_FREQUENCY;
    if(_voiceHud)
    {
        if(_recorder){
            [_recorder updateMeters];
        }
        
        float peakpower=[_recorder averagePowerForChannel:0];
        double ALPHA =0.05;
        double peakpowerforchannel=pow(10, (ALPHA *peakpower));
        [_voiceHud setProgress:peakpowerforchannel];
    }
}
-(void) showVoiceHudOrHide:(BOOL)yesOrNo{
    
    if (_voiceHud) {
        [_voiceHud hide];
        self.voiceHud = nil;
    }
    
    if (yesOrNo) {
        
        self.voiceHud = [[Voicehud alloc] init];
        [_voiceHud show];
        [_voiceHud release];
        
    }else{
        
    }
}
-(void)resetTimer
{
    if(_timer){
        [_timer invalidate];
        self.timer=nil;
        
    }
}
-(void) cancelRecording
{
    if (self.recorder.isRecording) {
        [self.recorder stop];
    }
    self.recorder = nil;
}

- (void)cancelled {
    
    [self showVoiceHudOrHide:NO];
    [self resetTimer];
    [self cancelRecording];
}

#pragma mark - LCVoiceHud Delegate

-(void) LCVoiceHudCancelAction
{
    [self cancelled];
}
@end
