#*********************************************************************
#
# (C) Copyright Broadcom Corporation 2013-2014
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#*********************************************************************

#path and prefix of toolchain
#

export AR      = $(CROSS_COMPILE)ar
export AS      = $(CROSS_COMPILE)as
export CC      = $(CROSS_COMPILE)gcc
export LD      = $(CROSS_COMPILE)ld
export NM      = $(CROSS_COMPILE)nm
export OBJCOPY = $(CROSS_COMPILE)objcopy
export OBJDUMP = $(CROSS_COMPILE)objdump
export RANLIB  = $(CROSS_COMPILE)ranlib
export STRIP   = $(CROSS_COMPILE)strip

export SED     = sed
export RM      = rm
export PYTHON_H = /usr/include/python2.7

ifeq ($(findstring ocp_sai,$(SAI_BASE)),)
  export SAI_ROOT =  $(SAI_BASE)/ocp_sai
else
  export SAI_ROOT =  $(SAI_BASE)
endif

ifeq ($(PLATFORM),)
  ifneq ($(L7_PLATFORM),)
    PLATFORM = $(L7_PLATFORM)
  else
    PLATFORM = s6000-x86-trident-deb80
  endif
endif


INCLUDE_FLAGS := -I$(SAI_ROOT)/include -I$(SAI_ROOT)/include/sai -DCDP_PACKAGE -DINCLUDE_L3
CFLAGS += -Wno-unused-result -Wno-error=format -Wno-error=format-security -g -O0
CFLAGS += -fno-common -fno-strict-aliasing -funit-at-a-time -msoft-float -Wall -Werror
CFLAGS += -DSTATIC=static -DLOG_TEST -DLOG_SAI -DPRINT_TO_SYSLOG -fPIC $(INCLUDE_FLAGS)

ifeq ($(OPENNSL_INC),)
  OPENNSL_INC = $(realpath $(SAI_ROOT)/../opennsl/include)
  ifeq ($(OPENNSL_INC),)
    $(error : path to OpenNSL headers not found)
  endif
endif
CFLAGS += -I$(OPENNSL_INC)

# Fixit
VERSION := 1
RELEASE_NUMBER := 0

#
SAI_BIN := $(OUTPATH)bin
SAI_OBJS := $(OUTPATH)objects

ifeq ($(LIB_OPENNSL_PATH),)
  LIB_OPENNSL_PATH := $(realpath $(OPENNSL_INC)/../output/$(PLATFORM)/build)
  ifeq ($(LIB_OPENNSL_PATH),)
    $(error : path to OpenNSL binaries not found)
  endif
endif
ifeq ($(LIB_SWITCH_OPENNSL_PATH),)
  LIB_SWITCH_OPENNSL_PATH := $(LIB_OPENNSL_PATH)
endif

sai_src_dir:=$(SAI_ROOT)/src
vpath %.c $(realpath $(sai_src_dir))
vpath %.o $(SAI_OBJS)/
#vpath %.o ./objects/
sai_source_files:= $(wildcard $(sai_src_dir)/brcm_sai*.c)
sai_so_library := libsai.so
sai_archive := $(SAI_BIN)/libsai.a
sai_soname := $(sai_so_library).$(VERSION)
## Full name without directory
sai_so_fullname:= $(sai_soname).$(RELEASE_NUMBER)
sai_swig_so_fullname = _sai.so
sai_libname:= sai

test_source_files :=  $(wildcard $(SAI_ROOT)/test/test_adapter_host.c)
test_objects := $(addprefix $(SAI_BIN)/, $(patsubst %.c,%.o,$(test_source_files)))
sai_includes := $(wildcard ../include/*.h)

test_host := $(SAI_BIN)/test_host

objects := $(addprefix $(SAI_OBJS)/, $(notdir $(sai_source_files:.c=.o)))
opennsl_so := libopennsl.so.1

tahelper_dir := $(SAI_BASE)/ocp_sai/tahelper
ta_deliverables := _sai.so sai.py sai_cli.py api_to_arg_map.txt attrs.txt

SAI_RELEASE = sai-$(L7_REL_NUM).$(L7_VER_NUM).$(L7_MAINT_NUM).$(L7_BUILD_NUM)
SAI_RELEASE_NUMBER = $(L7_REL_NUM)$(L7_VER_NUM)$(L7_MAINT_NUM)$(L7_BUILD_NUM)
PACKAGES_DIR ?= $(abspath $(OUTPATH)/packages/)
SAI_PACKAGE = $(abspath $(PACKAGES_DIR)/$(SAI_RELEASE)).tar

.DEFAULT_GOAL := all
all:  $(sai_so_fullname)


$(objects): $(SAI_OBJS)/%.o : %.c
	mkdir -p $(SAI_OBJS)
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -fPIC -c $^ -o $@

$(sai_so_fullname): $(objects)
	mkdir -p $(SAI_BIN)
	$(CC) $(CFLAGS) -fPIC -shared  -Wl,-soname,$(sai_soname) -L$(LIB_OPENNSL_PATH) -l opennsl  -o  $(SAI_BIN)/$(sai_so_fullname) $^
# Added symbolic link so the $(test_host) link can use library name instead of file name
	cd $(SAI_BIN);ln -sf $(@F) $(basename $(basename $(basename $(@F)))).so

$(test_host): $(test_source_files:.o=.c)
	$(CC) -g -O0 $(INCLUDE_FLAGS) $(CFLAGS) $^ -L$(LIB_OPENNSL_PATH) -L$(SAI_BIN)  -l sai -l opennsl -ldl -rdynamic -o $@

swig: $(sai_so_fullname)
	-python $(tahelper_dir)/genifile.py $(SAI_ROOT)/include/sai  >| $(tahelper_dir)/sai.i
	-swig -python $(tahelper_dir)/sai.i >/dev/null 2>&1
	-$(CC) -fPIC -I$(PYTHON_H) -shared $(INCLUDE_FLAGS) $(INCLUDE_OPENNSL_FLAGS) $(tahelper_dir)/sai_wrap.c -L$(LIB_SWITCH_OPENNSL_PATH) -l:$(opennsl_so) -L$(SAI_BIN) -l:$(sai_so_fullname) -o $(sai_swig_so_fullname) >/dev/null 2>&1
	-mv $(sai_swig_so_fullname) $(tahelper_dir)/$(sai_swig_so_fullname)
	-python $(tahelper_dir)/sai_attr_dict.py $(SAI_ROOT)/include/sai >| $(tahelper_dir)/attrs.txt
	-cd $(tahelper_dir) && cp -f $(ta_deliverables) $(SAI_BIN)/. && cd -
	-$(RM) -f $(tahelper_dir)/sai_wrap.c $(tahelper_dir)/sai.i $(tahelper_dir)/sai.py $(tahelper_dir)/_sai.so


test: $(test_host)

ifneq ($(dmode),)
tar_options = --show-transformed-names -v
endif

release: all
ifneq ($(dmode),)
	@echo
	@echo PLATFORM = $(PLATFORM)
	@echo SAI_RELEASE_NUMBER = $(SAI_RELEASE_NUMBER)
	@echo
endif
	@if [ "$(SAI_RELEASE_NUMBER)" = "0000" ]; then \
		echo -e \\nError: SAI release number must be non-zero\\n; \
		exit 1; \
	fi
	@if [ -z "$(PLATFORM)" ] ; then \
		echo -e \\nError: PLATFORM not specified\\n; \
		exit 1; \
	fi
	mkdir -p $(PACKAGES_DIR)
	cd $(PACKAGES_DIR)
#
# When multiple targets are supported, this shared library will go
# into a separate platform-specific bin package.  The source code will
# go into a package that should be the same for all platforms.
#
	tar -cf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_BIN),$(SAI_RELEASE)/bin/$(L7_PLATFORM),' --exclude-vcs --exclude-backups $(wildcard $(SAI_BIN)/libsai*)
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/bin/$(PLATFORM)/Makefile
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/bin/$(PLATFORM)/package.cfg
#
# Source code packaging
#
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/src/*.c
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/src/Makefile
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/include/*.h
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/include/sai/*.h
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT)/LICENSE_APACHE20,$(SAI_RELEASE)/src/LICENSE,' $(SAI_ROOT)/LICENSE_APACHE20
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT)/LICENSE_APACHE20,$(SAI_RELEASE)/include/LICENSE,' $(SAI_ROOT)/LICENSE_APACHE20
	tar -rf $(SAI_PACKAGE) $(tar_options) --absolute-names --transform='s,$(SAI_ROOT),$(SAI_RELEASE),' $(SAI_ROOT)/LEGAL_TERMS $(SAI_ROOT)/RELEASE $(SAI_ROOT)/README.md
	bzip2 -f $(SAI_PACKAGE)


sai_lib: $(sai_so_fullname)

clean:
	@echo "Cleaning SAI files"
	$(RM) -f $(SAI_BIN)/*
	$(RM) -f $(SAI_OBJS)/*
	$(RM) -f $(tahelper_dir)/sai_wrap.c
	$(RM) -f $(tahelper_dir)/sai.i
	$(RM) -f $(tahelper_dir)/sai.py
	$(RM) -f $(tahelper_dir)/_sai.so
	$(RM) -f $(tahelper_dir)/attrs.txt


.SECONDEXPANSION:

#
# This rule builds the dependency files
#
%.d: %.c
	set -e; $(RM) -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

dump_vars:
	@echo targets             = $(targets)
	@echo CFLAGS              = $(CFLAGS)
	@echo CROSS_COMPILE       = $(CROSS_COMPILE)
	@echo LIB_OPENNSL_PATH    = $(LIB_OPENNSL_PATH)
	@echo OPENNSL_INC         = $(OPENNSL_INC)
	@echo OUTPATH             = $(OUTPATH)
	@echo PACKAGES_DIR        = $(PACKAGES_DIR)
	@echo SAI_BIN             = $(SAI_BIN)
	@echo SAI_OBJS            = $(SAI_OBJS)
	@echo SAI_PACKAGE         = $(SAI_PACKAGE)
	@echo SAI_RELEASE         = $(SAI_RELEASE)
	@echo SAI_ROOT            = $(SAI_ROOT)
	@echo "\n"
	@echo "\n"
	@echo sai_source_files    = $(sai_source_files)
	@echo "\n"
	@echo "\n"
	@echo objects             = $(objects)

.PHONY: dump_vars all swig test release sai_lib clean
