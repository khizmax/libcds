
    CDS_LIBRARY_VERSION := 2.0.0
    LOCAL_PATH := $(call my-dir)
    CDS_ROOT_DIR = $(LOCAL_PATH)/../../..

    BOOST_LIBS := -lboost_thread -lboost_system
    BOOST_LIB_PATH := $(BOOST_ROOT)/stage32-android/lib

    CDS_TESTHDR_INCLUDE := $(LOCAL_PATH)/../../.. \
                        $(LOCAL_PATH)/../../../tests \
                        $(LOCAL_PATH)/../../../tests/test-hdr \
                        $(BOOST_ROOT)
# libcds
    include $(CLEAR_VARS)

    LOCAL_MODULE    := cds
    LOCAL_MODULE_FILENAME = libcds-$(CDS_LIBRARY_VERSION)
    include $(CDS_ROOT_DIR)/projects/source.libcds.mk
    LOCAL_SRC_FILES := $(CDS_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../.. $(BOOST_ROOT)
    LOCAL_CPP_FEATURES := exceptions

    include $(BUILD_SHARED_LIBRARY)

#test-hdr-map
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-map
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_MAP:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_MAP:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-deque
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-deque
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_DEQUE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_DEQUE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-ordlist
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-ordlist
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_ORDLIST:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_ORDLIST:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-pqueue
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-pqueue
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_PQUEUE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
#    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_PQUEUE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-queue
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-queue
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_QUEUE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_QUEUE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-set
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-set
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_SET:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_SET:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-stack
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-stack
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_STACK:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_STACK:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-tree
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-tree
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_TREE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_TREE:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)

#test-hdr-misc
    include $(CLEAR_VARS)

    LOCAL_MODULE    := test-hdr-misc
    include $(CDS_ROOT_DIR)/projects/source.test-common.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.mk
    include $(CDS_ROOT_DIR)/projects/source.test-hdr.offsetof.mk
    LOCAL_SRC_FILES := $(CDS_TESTCOMMON_SOURCES:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_SRC_FILES += $(CDS_TESTHDR_MISC:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
#    LOCAL_SRC_FILES += $(CDS_TESTHDR_OFFSETOF_MISC:%.cpp=$(CDS_ROOT_DIR)/%.cpp)
    LOCAL_C_INCLUDES := $(CDS_TESTHDR_INCLUDE)
    LOCAL_SHARED_LIBRARIES := cds
    LOCAL_CPP_FEATURES := exceptions
    LOCAL_CPPFLAGS := -Wno-invalid-offsetof -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
    LOCAL_LDLIBS   := $(BOOST_LIBS)
    LOCAL_LDFLAGS  := -L$(BOOST_LIB_PATH)
    include $(BUILD_EXECUTABLE)
