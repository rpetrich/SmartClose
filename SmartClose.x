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
	NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:@"/var/mobile/Library/Preferences/com.rpetrich.smartclose.plist"];
	for (BKProcess *process in [%c(BKProcess) _allProcesses]) {
		NSString *eventPortName = process.eventPortName;
		if (!eventPortName)
			continue;
		id temp = [settings objectForKey:[@"SCAppEnabled-" stringByAppendingString:eventPortName]];
		if (temp && ![temp boolValue])
			continue;
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
	} else if (kCFCoreFoundationVersionNumber < 1000) {
		CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(), SmartCloseNotificationReceived, SmartCloseNotificationReceived, CFSTR("com.rpetrich.smartclose"), NULL, CFNotificationSuspensionBehaviorCoalesce);
		%init();
	}
	Boolean exists = false;
	CFPreferencesGetAppBooleanValue(CFSTR("SCAppEnabled-com.apple.mobilemail"), CFSTR("com.rpetrich.smartclose"), &exists);
	if (!exists) {
		CFPreferencesSetAppValue(CFSTR("SCAppEnabled-com.apple.mobilemail"), kCFBooleanTrue, CFSTR("com.rpetrich.smartclose"));
		CFPreferencesAppSynchronize(CFSTR("com.rpetrich.smartclose"));
	}
	[pool drain];
}

static FBApplicationProcess *processForApplication(SBApplication *app)
{
	FBApplicationProcess **_process = CHIvarRef(app, _process, FBApplicationProcess *);
	return _process ? *_process : nil;
}

- (void)activator:(LAActivator *)activator receiveEvent:(LAEvent *)event
{
	event.handled = YES;
	dispatch_async(dispatch_get_main_queue(), ^{
		CFPreferencesAppSynchronize(CFSTR("com.rpetrich.smartclose"));
		SBAppSwitcherModel *model = (SBAppSwitcherModel *)[%c(SBAppSwitcherModel) sharedInstance];
		NSArray *identifiers = [model respondsToSelector:@selector(identifiers)] ? model.identifiers : model.snapshot;
		if ([identifiers count]) {
			Boolean exists = false;
			Boolean clearSwitcher = CFPreferencesGetAppBooleanValue(CFSTR("SCClearSwitcher"), CFSTR("com.rpetrich.smartclose"), &exists);
			if (!exists || clearSwitcher) {
				if ([model respondsToSelector:@selector(appsRemoved:added:)]) {
					[model appsRemoved:[NSArray arrayWithArray:identifiers] added:nil];
				} else {
					for (NSString *displayIdentifier in identifiers) {
						[model remove:displayIdentifier];
					}
				}
			}
		}
		SBUIController *uic = (SBUIController *)[%c(SBUIController) sharedInstance];
		[uic dismissSwitcherAnimated:YES];
		FBApplicationProcess *currentProcess = processForApplication(UIApp._accessibilityFrontMostApplication);
		if (kCFCoreFoundationVersionNumber < 1000) {
			notify_post("com.rpetrich.smartclose");
		} else {
			for (FBApplicationProcess *process in [(FBProcessManager *)[%c(FBProcessManager) sharedInstance] allApplicationProcesses]) {
				if (!process.nowPlayingWithAudio && !process.recordingAudio && (process != currentProcess)) {
					BKSProcess **_bksProcess = CHIvarRef(process, _bksProcess, BKSProcess *);
					if (_bksProcess) {
						[process processWillExpire:*_bksProcess];
					}
				}
			}
		}
	});
}

@end
