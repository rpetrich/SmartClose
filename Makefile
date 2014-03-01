TWEAK_NAME = SmartClose
SmartClose_FILES = SmartClose.x
SmartClose_FRAMEWORKS = UIKit
SmartClose_LIBRARIES = activator

IPHONE_ARCHS = armv7 arm64

ADDITIONAL_CFLAGS = -std=c99
TARGET_IPHONEOS_DEPLOYMENT_VERSION = 6.0

TWEAK_TARGET_PROCESS = backboardd

include framework/makefiles/common.mk
include framework/makefiles/tweak.mk
