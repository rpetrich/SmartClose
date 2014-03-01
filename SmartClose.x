#import <UIKit/UIKit.h>
#import <notify.h>
#import <libactivator/libactivator.h>
#import <CaptainHook/CaptainHook.h>

#import "Headers.h"

static NSInteger mustTerminateImmediately;

%hook BKProcessAssertion

+ (NSTimeInterval)GetTaskCompletionMustTerminateDuration
{
	return mustTerminateImmediately ? 0.0 : %orig();
}

- (NSTimeInterval)permittedBackgroundDuration
{
	NSTimeInterval result = %orig();
	return (mustTerminateImmediately || isinf(result)) ? 0.0 : result;
}

%end

__attribute__((visibility("hidden")))
@interface SmartClose : NSObject <LAListener>
@end

static void SmartCloseNotificationReceived(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo)
{
	//mustTerminateImmediately++;
	for (BKProcess *process in [%c(BKProcess) _allProcesses]) {
		NSLog(@" %@", process);
		if (process.running && !process.frontmost && !process.nowPlayingWithAudio && !process.recordingAudio && !process.isViewService && !process.beingDebugged) {
			for (BKProcessAssertion *assertion in process.assertions) {
				NSLog(@"  %@ (%ld)", assertion, (long)assertion.reason);
				//[process removeAssertion:assertion];
				if (assertion.reason == 10000)
					goto skip;
			}
			NSMutableSet **_assertions = CHIvarRef(process, _assertions, NSMutableSet *);
			if (_assertions) {
				NSMutableSet *backup = *_assertions;
				*_assertions = nil;
				[process _updateAssertionNeedsWithReason:0];
				*_assertions = backup;
			} else {
				[process _notifyOfExpirationWarning];
			}
skip:
			continue;
		}
	}
	//mustTerminateImmediately--;
}

@implementation SmartClose

+ (void)load
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (LASharedActivator.runningInsideSpringBoard) {
		SmartClose *listener = [[self alloc] init];
		[LASharedActivator registerListener:listener forName:@"com.rpetrich.smartclose"];
	} else {
		CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(), SmartCloseNotificationReceived, SmartCloseNotificationReceived, CFSTR("com.rpetrich.smartclose"), NULL, CFNotificationSuspensionBehaviorCoalesce);
		%init();
	}
	[pool drain];
}

- (void)activator:(LAActivator *)activator receiveEvent:(LAEvent *)event
{
	SBAppSwitcherModel *model = (SBAppSwitcherModel *)[%c(SBAppSwitcherModel) sharedInstance];
	NSArray *identifiers = model.identifiers;
	if ([identifiers count]) {
		event.handled = YES;
		[model appsRemoved:[NSArray arrayWithArray:identifiers] added:nil];
	}
	SBUIController *uic = (SBUIController *)[%c(SBUIController) sharedInstance];
	[uic dismissSwitcherAnimated:YES];
	notify_post("com.rpetrich.smartclose");
}

@end
