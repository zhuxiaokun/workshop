//
//  voide.h
//  TCPClient
//
//  Created by Kai Deng on 13-11-25.
//  Copyright (c) 2013年 PK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
@class Voicehud;

@interface voide : NSObject<AVAudioRecorderDelegate,AVAudioPlayerDelegate>
{
}
@property(nonatomic,strong) NSString * recordPath;
@property(nonatomic) float recordTime;
@property(nonatomic,strong) Voicehud * voiceHud;
@property(nonatomic,strong)NSTimer *timer;
@property(nonatomic,strong) AVAudioRecorder *recorder;

-(void) startRecordWithPath:(NSString *)path;
-(void) stopRecordWithCompletionBlock:(void (^)())completion;
-(void) cancelled;

@end
