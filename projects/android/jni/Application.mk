
APP_MODULES := cds test-hdr-map test-hdr-deque test-hdr-ordlist test_hdr_pqueue 
APP_MODULES += test-hdr-queue test-hdr-set test-hdr-stack test-hdr-tree test-hdr-misc
APP_ABI := armeabi-v7a
APP_OPTIM := debug
APP_STL := gnustl_shared
APP_CPPFLAGS := -std=c++0x -frtti -pedantic -Wall -Wno-unused-local-typedefs
#APP_LDFLAGS :=
APP_GNUSTL_FORCE_CPP_FEATURES := exceptions
NDK_TOOLCHAIN_VERSION := 4.8
