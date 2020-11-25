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
#	BITS:		64 or 32

#Begin
########################################################################################


# 项目目录[模块Makefile的目录]
PROGRAM_DIRS :=  http_server \
                 https_server
########################################################################################
all:
	@for dir in $(PROGRAM_DIRS);do \
	$(MAKE) -C $$dir || exit 1;\
	done
.PHONY:mkcert
mkcert:
	chmod 755 ./package.sh
	./package.sh mkcert
.PHONY:clean
clean:
	@for dir in $(PROGRAM_DIRS);do \
	$(MAKE) clean -C $$dir || exit 1;\
	done
		
#End
