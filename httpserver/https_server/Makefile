# Makefile written by RichieMay,in Chengdu City,Sichuan Province,China.
# @configure_input@

# Copyright (C) 2014, Inc.
# This Makefile is commercial software;

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

#OPTION:
#	RELEASE:	1 or 0
#	BITS:		64 or 32 or other(arm)

#Begin
########################################################################################

# 目标文件名
TARGET := https_server

TARGET_OUT_PUT := ../output

#依赖项目路径
PROGRAM_DIRS = 

#依赖库路径
LIB_DIRS = 

#静态/动态依赖库
STATIC_LIBS := 
DYNAMIC_LIBS := -lstdc++ -lrt -ldl -pthread -lssl -lcrypto -ljson
#-pthread 

# 源文件类型
SRC_FILTER := %.cpp

# 头文件类型
INC_FILTER := %.h

# 当前目录
ROOT_PATH  :=  ./ ../src

PUBLIC_INCS := /home/jwy/workspace/openssl/include/
# PUBLIC_INCS += /usr/include/json/
# 编译器
CC := g++

# 编译参数		
CFLAGS := 

# 宏定义
MACRONAMES := -D_GLIBCXX_USE_CXX11_ABI=0 -DWITH_OPENSSL

#arm/32/64位平台
BITS := arm
PLATFORM := 
ifeq ($(BITS), 32)
	PLATFORM := -m32
else ifeq ($(BITS), 64)
	PLATFORM := -m64
else
	PLATFORM := 
endif
CFLAGS += $(PLATFORM)

#输出目录
OUTPUT := Debug

# 编译选项
RELEASE := 0
ifeq ($(RELEASE), 1)
	CFLAGS += -O2
	OUTPUT := Release
	MACRONAMES += -DNDEBUG
else
	CFLAGS += -O0
endif

CFLAGS += -g
CFLAGS += -Wall
CFLAGS += $(MACRONAMES)

LIBS := -Wl,-Bstatic
LIBS += $(STATIC_LIBS)
LIBS += -Wl,-Bdynamic
LIBS += $(DYNAMIC_LIBS)
########################################################################################

rwildcard = $(wildcard $1$2) $(foreach dir,$(wildcard $1*),$(call rwildcard,$(dir)/,$2))

ALL_FILES := $(foreach path,$(ROOT_PATH),$(call rwildcard,$(path),*.*))

INC_DIRS  := $(sort $(dir $(filter $(INC_FILTER),$(ALL_FILES)))) 

SRC_DIRS  := $(sort $(dir $(filter $(SRC_FILTER),$(ALL_FILES)))) 

INCS := $(foreach path,$(INC_DIRS),-I $(path))

INCS += $(foreach path,$(PUBLIC_INCS),-I $(path))

SRCS := $(filter $(SRC_FILTER),$(ALL_FILES))

VPATH := $(SRC_DIRS)

OBJS := $(patsubst $(SRC_FILTER),%.o, $(notdir $(SRCS)))

OBJ_OUTPUT_FILES := $(addprefix $(OUTPUT)/,$(OBJS))

ALL_PROGRAM_DIRS := $(addsuffix /$(TARGET_OUT_PUT)/$(OUTPUT),$(PROGRAM_DIRS))

ALL_PROGRAM_DIRS += $(LIB_DIRS)

ALL_LIB_DIRS := $(foreach path,$(ALL_PROGRAM_DIRS),-L $(path))

ALL_LIB_DIRS += $(LIBS)

all:$(TARGET_OUT_PUT)/$(TARGET)
$(TARGET_OUT_PUT)/$(TARGET):$(OBJ_OUTPUT_FILES)
	test -d $(TARGET_OUT_PUT) || mkdir -p $(TARGET_OUT_PUT)
	$(CC) $(CFLAGS) $(INCS) -o $@ $(OBJ_OUTPUT_FILES) $(ALL_LIB_DIRS)
				
$(OUTPUT)/%.o:$(SRC_FILTER)
	test -d $(OUTPUT) || mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@ $(ALL_LIB_DIRS) 
	
%.depend:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
sinclude $(SRCS:.cpp=.d) 
	
.PHONY:clean
clean:
	rm -rf  $(TARGET_OUT_PUT)/$(TARGET) .depend
	rm -rf  $(OUTPUT) .depend
	
#End
