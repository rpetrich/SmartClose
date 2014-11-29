#import <Foundation/Foundation.h>

@interface SBAppSwitcherModel : NSObject {
    NSMutableArray *_recentDisplayIdentifiers;
    NSTimer *_saveTimer;
}
+ (SBAppSwitcherModel *)sharedInstance;
- (id)snapshot;
- (void)appsRemoved:(NSArray *)removedApps added:(NSArray *)addedApps;
- (NSArray *)identifiers;
- (void)remove:(NSString *)identifier;
- (void)addToFront:(NSString *)identifier;
- (id)_recentsFromPrefs;
- (void)_saveRecents;
- (void)_saveRecentsDelayed;
- (void)_invalidateSaveTimer;
- (void)dealloc;
- (id)init;
@end

@interface SBAppSwitcherModel (iOS8)
@property (nonatomic, readonly) NSArray *snapshotOfFlattenedArrayOfAppIdentifiersWhichIsOnlyTemporary;
@end

@interface SBUIController : NSObject
+ (SBUIController *)sharedInstance;
- (void)dismissSwitcherAnimated:(BOOL)animated;
@end

struct BKProcessTimes {
    NSTimeInterval execTime;
    NSTimeInterval beginUserCPUElapsedTime;
    NSTimeInterval beginSystemCPUElapsedTime;
    NSTimeInterval beginIdleCPUElapsedTime;
    NSTimeInterval beginApplicationCPUElapsedTime;
};

@class BKSMachSendRight, BKPortDeathWatcher;

@interface BKProcess : NSObject /*<BKProcessAssertionDelegate>*/ {
    dispatch_queue_t _queue;
    NSString *_jobLabel;
    BOOL _running;
    int _suspendedPriorityBand;
    int _backgroundPriorityBand;
    int _currentPriorityBand;
    NSTimeInterval _timedAssertionPermittedDuration;
    NSTimeInterval _timedAssertionReferenceTime;
    BOOL _timedAssertionDeadlineActive;
    NSTimeInterval _allowIdleSleepReferenceTime;
    NSTimeInterval _suspendedUntilUnlockReferenceTime;
    dispatch_group_t _timerLifecycleGroup;
    dispatch_source_t _backgroundPermissionCheckTimer;
    dispatch_source_t _backgroundPermissionExpirationWarningTimer;
    dispatch_source_t _taskCompletionAllowIdleSleepTimer;
    NSSet *_allowedLockedFilePaths;
    pid_t _pid;
    NSString *_eventPortName;
    unsigned int _eventPort;
    unsigned int _taskNamePort;
    BKSMachSendRight *_taskPort;
    BKPortDeathWatcher *_taskMonitor;
    struct BKProcessTimes _times;
    double _unsuspendLimitStart;
    long long _unsuspendLimitCount;
    NSMutableSet *_assertions;
    NSMutableSet *_limitlessAssertions;
    NSMutableSet *_watchdogAssertions;
    unsigned int _powerAssertionID;
    unsigned int _hasWatchdogAssertionsOut:1;
    BOOL _supportsVoIP;
    BOOL _supportsUnboundedTaskCompletion;
    BOOL _supportsNetworkAuthentication;
    BOOL _supportsNewsstand;
    BOOL _supportsBackgroundContentFetching;
    unsigned long long _priorityBandUserData;
    unsigned long long _lastPriorityBandUserData;
    unsigned int _nowPlayingWithAudio:1;
    unsigned int _recordingAudio:1;
    unsigned int _receivingPacketsFromAccessory:1;
    unsigned int _frontmost:1;
    unsigned int _taskSuspended:1;
    unsigned int _terminateOnSuspension:1;
    unsigned int _usesSocketMonitoring:1;
    unsigned int _supportsBackgroundTaskAssertions:1;
    unsigned int _supportsSuspendOnLock:1;
    unsigned int _assertionsWantForegroundResourcePriority:1;
    unsigned int _isViewService:1;
    unsigned int _hasViewServiceBeenSuspended:1;
    unsigned int _shouldSuspendOnSleep:1;
    unsigned int _throttleSetForTheFirstTime:1;
    unsigned int _uiThrottled:1;
    unsigned int _cpuThrottled:1;
    int _priority;
}

+ (unsigned long long)_bitForProcessAssertionReason:(unsigned int)arg1;
+ (void)hostwideUserElapsedCPUTime:(double *)arg1 systemElapsedCPUTime:(double *)arg2 idleElapsedCPUTime:(double *)arg3;
+ (void)deleteAllJobs;
+ (void)cancelTaskCompletionAllowIdleSleep;
+ (void)scheduleTaskCompletionAllowIdleSleep;
+ (void)resumeIfSuspendedForSleep;
+ (void)shutdownSuspendedProcessSockets;
+ (void)enumerateAllProcessesWithBlock:(id)arg1;
+ (int)defaultSuspendedLoweredPriorityBand;
+ (int)defaultSuspendedPriorityBand;
+ (int)defaultBackgroundLoweredPriorityBand;
+ (int)defaultBackgroundPriorityBand;
+ (void)watchForProcessTermination:(id)arg1 bundleID:(NSString *)bundleIdentifier;
+ (id)launchedProcessWithBundleIdentifier:(id)arg1 path:(id)arg2 containerPath:(id)arg3 arguments:(id)arg4 environment:(id)arg5 standardOutputPath:(id)arg6 standardErrorPath:(id)arg7 machServices:(id)arg8 threadPriority:(long long)arg9 frontmost:(BOOL)arg10 systemApp:(BOOL)arg11 backgroundPriorityBand:(int)arg12 suspendedPriorityBand:(int)arg13 waitForDebugger:(BOOL)arg14 disableASLR:(BOOL)arg15 allowedLockedFilePaths:(id)arg16 terminateOnSuspension:(BOOL)arg17 queue:(id)arg18;
+ (BKProcess *)processForPid:(pid_t)pid;
+ (NSArray *)_allProcesses;
+ (void)initialize;
+ (void)_logJetsamPriorities;
@property (readonly) int priority; // @synthesize priority=_priority;
@property BOOL supportsBackgroundContentFetching; // @synthesize supportsBackgroundContentFetching=_supportsBackgroundContentFetching;
@property BOOL supportsNewsstand; // @synthesize supportsNewsstand=_supportsNewsstand;
@property BOOL supportsNetworkAuthentication; // @synthesize supportsNetworkAuthentication=_supportsNetworkAuthentication;
@property BOOL supportsUnboundedTaskCompletion; // @synthesize supportsUnboundedTaskCompletion=_supportsUnboundedTaskCompletion;
@property BOOL supportsVoIP; // @synthesize supportsVoIP=_supportsVoIP;
@property (readonly) unsigned int taskNamePort; // @synthesize taskNamePort=_taskNamePort;
@property (readonly) NSString *eventPortName; // @synthesize eventPortName=_eventPortName;
@property (readonly) unsigned int eventPort; // @synthesize eventPort=_eventPort;
@property (readonly) pid_t pid; // @synthesize pid=_pid;
@property (copy) NSString *jobLabel; // @synthesize jobLabel=_jobLabel;
@property (copy) NSSet *allowedLockedFilePaths; // @synthesize allowedLockedFilePaths=_allowedLockedFilePaths;
- (id)processAssertionDelegateDescription:(id)arg1;
- (void)processAssertionInvalidated:(id)arg1;
- (void)processAssertion:(id)arg1 preventSuspendOnSleepChanged:(BOOL)arg2;
- (void)processAssertion:(id)arg1 wantsForegroundResourcePriorityCountChange:(BOOL)arg2;
- (void)processAssertion:(id)arg1 preventIdleSleepChanged:(BOOL)arg2;
- (void)processAssertion:(id)arg1 preventCPUThrottleDownChanged:(BOOL)arg2;
- (void)processAssertion:(id)arg1 preventUIThrottleDownChanged:(BOOL)arg2;
- (void)processAssertion:(id)arg1 preventTaskSuspendChanged:(BOOL)arg2;
- (void)removeWatchdogAssertion:(id)arg1;
- (void)addWatchdogAssertion:(id)arg1;
- (void)_checkWatchdogAssertionsCount;
@property (readonly) BOOL hasWatchdogAssertionsOut;
- (void)_resumeIfSuspendedForSleep;
- (void)_systemWillSleep:(id)arg1;
- (void)_updateBackgroundPermissionCheckTimer;
- (id)_setupOneShotTimer:(double)arg1 withBlock:(id)arg2;
- (void)_cancelTimers;
@property (readonly) double remainingAllowedAssertionDuration;
- (BOOL)hasAssertionForReason:(unsigned int)arg1;
- (BOOL)hasAssertions;
- (id)assertions;
- (void)removeAssertion:(id)arg1;
- (void)addAssertion:(id)arg1;
- (void)_updateAssertionNeedsWithReason:(unsigned int)arg1;
- (unsigned int)_calculateAssertionStateWithPowerAssertionNames:(id *)arg1 priorityBandData:(unsigned long long *)arg2;
- (BOOL)_hasViewServiceAssertion;
- (long long)noteExitedForForceQuit:(BOOL)arg1;
- (id)_crashReportThermalsInfo;
- (id)_crashReportCPUUsageInfo;
- (void)markWatchdogCPUTimes;
@property (readonly) NSTimeInterval execTime;
@property (readonly) NSTimeInterval elapsedCPUTime;
@property (readonly, getter=isBeingPtraced) BOOL beingPtraced;
- (BOOL)_isBeingDebugged;
@property (readonly, getter=isBeingDebugged) BOOL beingDebugged;
- (void)_forceCrashReportWithReason:(int)arg1 description:(id)arg2;
- (void)watchForTerminationWithHandler:(id)arg1;
- (void)waitToExecOrExit;
- (void)killWithSignal:(int)arg1;
- (BOOL)_setPriority:(int)arg1 ofResource:(int)arg2;
- (BOOL)_taskShutdownSockets:(int)arg1;
- (BOOL)_taskResumeForReason:(unsigned int)arg1;
- (BOOL)_taskSuspend;
- (void)_updatePriorityBand;
- (int)_currentPriorityBand;
- (void)dispatchSharedLockCheck;
- (void)_notifyOfExpirationWarning;
- (void)watchdogTerminateWithReason:(int)arg1 format:(id)arg2;
@property (readonly) NSSet *lockedFilePathsIgnoringAllowed;
- (BOOL)taskCompletionShouldPreventIdleSleep;
- (void)_cancelTaskCompletionAllowIdleSleep;
- (void)_scheduleTaskCompletionAllowIdleSleep;
- (BOOL)_resumeForReason:(unsigned int)arg1;
- (BOOL)_suspend;
- (BOOL)_isWorkspaceLocked;
@property (readonly) int currentPriorityBand;
@property int suspendedPriorityBand;
@property int backgroundPriorityBand;
@property (getter=isReceivingPacketsFromAccessory) BOOL receivingPacketsFromAccessory;
@property (getter=isNowPlayingWithAudio) BOOL nowPlayingWithAudio;
@property (getter=isFrontmost) BOOL frontmost;
@property (readonly) BOOL hasResumeAssertion;
@property (getter=isRecordingAudio) BOOL recordingAudio;
@property BOOL isViewService;
- (void)setAssertionsWantForegroundResourcePriority:(BOOL)arg1;
- (BOOL)assertionsWantForegroundResourcePriority;
@property BOOL supportsSuspendOnLock;
@property BOOL supportsBackgroundTaskAssertions;
@property BOOL usesSocketMonitoring;
- (NSString *)description;
- (NSString *)debugDescription;
@property (readonly) NSString *name;
@property (retain) BKSMachSendRight *taskPort;
@property (getter=isRunning) BOOL running;
- (void)dealloc;
- (id)initWithPid:(pid_t)pid frontmost:(BOOL)frontmost backgroundPriorityBand:(int)backgroundPriorityBand suspendedPriorityBand:(int)suspendedPriorityBand jobLabel:(NSString *)jobLabel eventPortName:(NSString *)eventPortName allowedLockedFilePaths:(NSArray *)allowedLockedFilePaths terminateOnSuspension:(BOOL)terminateOnSuspension execTime:(NSTimeInterval)execTime queue:(dispatch_queue_t)queue;

@end

@protocol BKProcessAssertionDelegate;
@protocol OS_xpc_object;

@interface BKProcessAssertion : NSObject {
    id <BKProcessAssertionDelegate> _delegate;
    NSObject<OS_xpc_object> *_client;
    unsigned int _reason;
    NSString *_identifier;
    NSLock *_lock;
    pid_t _ownerPID;
    unsigned int _savedFlags;
    unsigned int _preventSuspend:1;
    unsigned int _preventThrottleDownCPU:1;
    unsigned int _preventThrottleDownUI:1;
    unsigned int _preventIdleSleep:1;
    unsigned int _preventingIdleSleep:1;
    unsigned int _allowIdleSleepOverrideEnabled:1;
    unsigned int _wantsForegroundResourcePriority:1;
    unsigned int _preventSuspendOnSleep:1;
}

+ (BOOL)isEligibleForLoweredJetsamPriority:(unsigned int)arg1;
+ (BOOL)isRateLimitedReason:(unsigned int)reason;
+ (BOOL)isValidProcessAssertionReason:(unsigned int)arg1;
+ (NSTimeInterval)GetTaskCompletionMustTerminateDuration;
@property (nonatomic, assign) id <BKProcessAssertionDelegate> delegate;
@property (retain) NSObject<OS_xpc_object> *client;
@property (readonly, assign) unsigned int reason;
@property (readonly, assign) pid_t ownerPID;
@property (retain, nonatomic) NSString *identifier;
- (void)invalidate;
- (unsigned int)assertionState;
- (void)restoreSavedFlags;
- (void)saveFlagsAndApplyNewFlags:(unsigned int)arg1;
- (void)_setSavedFlags:(unsigned int)arg1;
- (unsigned int)_savedFlags;
- (BOOL)_hasSavedFlags;
- (void)setFlags:(unsigned int)arg1;
- (unsigned int)flags;
@property (assign) BOOL preventSuspendOnSleep;
@property (assign) BOOL wantsForegroundResourcePriority;
- (BOOL)wantsForegroundResourcePriority_withoutLocking;
@property (assign) BOOL preventIdleSleep;
- (BOOL)shouldReallyPreventIdleSleep;
@property (assign) BOOL allowIdleSleepOverrideEnabled;
- (BOOL)_lock_shouldActuallyPreventIdleSleep;
@property (assign) BOOL preventThrottleDownUI;
@property (assign) BOOL preventThrottleDownCPU;
@property (assign) BOOL preventSuspend;
- (BOOL)isValid;
@property (readonly, assign) BOOL hasFixedStartTime;
@property (readonly, assign, getter=isTimeLimited) BOOL timeLimited;
@property (readonly, assign) NSTimeInterval permittedBackgroundDuration;
- (NSString *)description;
- (id)initWithClient:(id)client ownerPID:(pid_t)pid reason:(unsigned int)reason name:(NSString *)name flags:(unsigned int)flags;
- (id)initWithReason:(unsigned int)reason identifier:(NSString *)identifier;

@end

@class FBProcessState;

@interface FBProcess : NSObject
@property (readonly, copy, nonatomic) FBProcessState *state;
@property (readonly, copy, nonatomic) NSString *jobLabel;
@property (readonly, copy, nonatomic) NSString *bundleIdentifier;
@property (readonly, copy, nonatomic) NSString *name;
@end

@interface FBApplicationProcess : FBProcess
@property (nonatomic, getter=isConnectedToExternalAccessory) BOOL connectedToExternalAccessory;
@property (nonatomic, getter=isNowPlayingWithAudio) BOOL nowPlayingWithAudio;
@property (nonatomic, getter=isRecordingAudio) BOOL recordingAudio;
@end

