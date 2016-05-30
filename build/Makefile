# Makefile 
# 

.PHONY: all
.SUFFIXES:.cpp .c .ro

# make_distrib.pl substitutes the correct version
VERSION=2.2.0

# boost_thread lib used by test application. 
# You may change the library name
LD_BOOST_THREAD_LIB=-lboost_thread -lboost_system 

ifeq ($(platform),mingw)
    LD_TEST_COMMON_LIBS=-lcds -lrtm
    LD_TEST_COMMON_DEBUG_LIBS=-lcds-debug -lrtm
    BASE_OPT = -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I..
    # -Wa,--hash-size=2048
    CPP_BUILD_CDS_OPT=-DCDS_BUILD_LIB
else
ifeq ($(platform),darwin)
    LD_TEST_COMMON_LIBS=-lcds
    LD_TEST_COMMON_DEBUG_LIBS=-lcds-debug
else
    LD_TEST_COMMON_LIBS=-lcds -lpthread -lrt
    LD_TEST_COMMON_DEBUG_LIBS=-lcds-debug -lpthread -lrt
endif
    BASE_OPT = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -D_FILE_OFFSET_BITS=64 -I..
    CPP_BUILD_CDS_OPT=
endif

COMP_OPT = -c $(CFLAGS) $(BASE_OPT)
CPP_COMP_OPT = -MMD -std=c++11 -c $(CXXFLAGS) $(BASE_OPT)

COMPILER_ROOT = $(shell dirname `dirname \`which 	$(CXX)\``)

LD_OPTS = $(LDFLAGS)



####################################
# cds library

include ../projects/source.libcds.mk
CDS_OBJS := $(addprefix $(OBJ_PATH)/,$(notdir $(CDS_SOURCES)))
CDS_OBJS := $(CDS_OBJS:%.cpp=%.o)
CDS_OBJS_DEPS := $(CDS_OBJS:%.o=%.d)
CDS_SOURCES := $(CDS_SOURCES:%.cpp=../%.cpp)

ifeq ($(platform),mingw)
    CDS_DEBUG_LIB=libcds-debug.dll
    CDS_RELEASE_LIB=libcds.dll
else
ifeq ($(platform),darwin)
    CDS_DEBUG_LIB=libcds-debug.dylib
    CDS_RELEASE_LIB=libcds.dylib
else
    CDS_DEBUG_LIB=libcds-debug.so
    CDS_RELEASE_LIB=libcds.so
endif
endif

-include $(CDS_OBJS_DEPS)
$(CDS_OBJS): $(OBJ_PATH)/%.o: ../src/%.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<

ifeq ($(platform),mingw)
$(CDS_DEBUG_LIB): $(BIN_PATH)/$(CDS_DEBUG_LIB)

$(BIN_PATH)/$(CDS_DEBUG_LIB) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) -Wl,--out-implib,$(BIN_PATH)/$(CDS_DEBUG_LIB).a $(CDS_OBJS) -o $@ $(LDLIBS)

$(CDS_RELEASE_LIB) : $(BIN_PATH)/$(CDS_RELEASE_LIB)

$(BIN_PATH)/$(CDS_RELEASE_LIB) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_OBJS) -Wl,--out-implib,$(BIN_PATH)/$(CDS_RELEASE_LIB).a -o $@ $(LDLIBS)
    
debug : $(CDS_DEBUG_LIB)
release : $(CDS_RELEASE_LIB)
else
$(CDS_DEBUG_LIB).$(VERSION) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_PLATFORM_DEBUG_LDFLAGS) $(CDS_OBJS) -o $@ $(LDLIBS)
	mv ./$(CDS_DEBUG_LIB).$(VERSION) $(BIN_PATH)/$(CDS_DEBUG_LIB).$(VERSION)
	ln -sf $(CDS_DEBUG_LIB).$(VERSION) $(BIN_PATH)/$(CDS_DEBUG_LIB)

$(CDS_RELEASE_LIB).$(VERSION) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_OBJS) $(CDS_PLATFORM_RELEASE_LDFLAGS) -o $@ $(LDLIBS)
	mv ./$(CDS_RELEASE_LIB).$(VERSION) $(BIN_PATH)/$(CDS_RELEASE_LIB).$(VERSION)
	ln -sf $(CDS_RELEASE_LIB).$(VERSION) $(BIN_PATH)/$(CDS_RELEASE_LIB)
    
debug : $(CDS_DEBUG_LIB).$(VERSION)
release : $(CDS_RELEASE_LIB).$(VERSION)
    
endif
    
all: debug release

##########################################
# Make tests

OBJ_TEST_PATH=$(OBJ_PATH)

include ../projects/source.test-common.mk
CDS_TESTCOMMON_SOURCES := $(CDS_TESTCOMMON_SOURCES:%.cpp=../%.cpp)
TEST_COMMON_OBJS := $(CDS_TESTCOMMON_SOURCES:%.cpp=%.o)
TEST_COMMON_OBJS_DEPS := $(TEST_COMMON_OBJS:%.o=%.d)

TEST_COMMONHDR_SRC_DIR=../tests
-include $(TEST_COMMON_OBJS_DEPS)
$(TEST_COMMON_OBJS) : %.o : %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@


TEST_SRC_DIR=../tests/unit
TEST_DATA_DIR=`pwd`/../tests/data

CDSUNIT_COMMON_FILE=

include ../projects/source.unit.misc.mk
CDSUNIT_MISC_SOURCES := $(CDSUNIT_MISC_SOURCES:%.cpp=../%.cpp)
CDSUNIT_MISC_OBJS := $(CDSUNIT_MISC_SOURCES:%.cpp=%.o)

TEST_OBJ_FILE := $(CDSUNIT_COMMON_FILE) $(CDSUNIT_MISC_OBJS)
TEST_OBJ_FILE_DEPS := $(TEST_OBJ_FILE:%.o=%.d)

-include $(TEST_OBJ_FILE_DEPS)
$(TEST_OBJ_FILE): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TEST_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@

CDSUNIT_MISC_EXE=$(BIN_PATH)/cdsu-misc
CDSUNIT_EXE_FILES= $(CDSUNIT_MISC_EXE)

ifeq ($(platform),mingw)
make_test : $(CDSUNIT_EXE_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	cp -f $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
else
make_test : $(CDSUNIT_EXE_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	ln -sf $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
endif

$(CDSUNIT_MISC_EXE) : $(CDSUNIT_MISC_OBJS) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_MISC_OBJS) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) -o $@ $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) $(LDLIBS)


CDSUNIT_MISC_EXE_DBG=$(CDSUNIT_MISC_EXE)-d
CDSUNIT_EXE_DBG_FILES= $(CDSUNIT_MISC_EXE_DBG)

ifeq ($(platform),mingw)
make_debug_test : $(CDSUNIT_EXE_DBG_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	cp -f $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
else
make_debug_test : $(CDSUNIT_EXE_DBG_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	ln -sf $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
endif

$(CDSUNIT_MISC_EXE_DBG) : $(CDSUNIT_MISC_OBJS) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_MISC_OBJS) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) -o $@ $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) $(LDLIBS)

test: make_test 
test_debug: make_debug_test

##########################################
#
clean: 
	rm -f $(OBJ_PATH)/debug/*
	rm -f $(OBJ_PATH)/release/*
	rm -f $(TEST_COMMON_OBJS) $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_OBJ_FILE)
	rm -f $(TEST_COMMON_OBJS_DEPS) $(TESTHDR_OBJS_DEPS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN_DEPS) $(TEST_OBJ_FILE_DEPS)
	rm -f $(BIN_PATH)/libcds*
	rm -f $(BIN_PATH)/cdsu-*

