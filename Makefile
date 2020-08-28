##=====================================================================
## MyLogger: A basic logger.
## Copyright 2018-2019 Quentin Quadrat <lecrapouille@gmail.com>
##
## This file is part of MyLogger.
##
## MyLogger is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## MyLogger is distributedin the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
##=====================================================================

###################################################
# Project definition
#
PROJECT = MyLogger
TARGET = $(PROJECT)
DESCRIPTION = A basic logger
BUILD_TYPE = release

###################################################
# Location of the project directory and Makefiles
#
P := .
M := $(P)/.makefile
include $(M)/Makefile.header

###################################################
# Inform Makefile where to find header files
#
INCLUDES += \
  -I$(P)/include -I$(P)/src

###################################################
# Inform Makefile where to find *.cpp and *.o files
#
VPATH += \
  $(P)/src

###################################################
# Make the list of compiled files
#
OBJS = File.o Path.o ILogger.o Logger.o

###################################################
# Project defines
#
DEFINES +=

###################################################
# Set Libraries. For knowing which libraries
# is needed please read the external/README.md file.
# lreadline: for interactive prompt
# ldl: for loading symbols in shared libraries
#
LINKER_FLAGS +=

###################################################
# Compile the project
all: $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET) $(PKG_FILE)

###################################################
# Compile and launch unit tests and generate the code coverage html report.
.PHONY: unit-tests
unit-tests:
	@$(call print-simple,"Compiling unit tests")
	@$(MAKE) -C tests coverage

###################################################
# Compile and launch unit tests and generate the code coverage html report.
.PHONY: check
check: unit-tests

###################################################
# Install project. You need to be root.
.PHONY: install
install: $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET) pkg-config
	@$(call INSTALL_DOCUMENTATION)
	@$(call INSTALL_PROJECT_LIBRARIES)
	@$(call INSTALL_PROJECT_HEADERS)

###################################################
# Uninstall the project. You need to be root. FIXME: to be updated
#.PHONY: uninstall
#uninstall:
#	@$(call print-simple,"Uninstalling",$(PREFIX)/$(TARGET))
#	@rm $(PROJECT_EXE)
#	@rm -r $(PROJECT_DATA_ROOT)

###################################################
# Clean the whole project.
.PHONY: veryclean
veryclean: clean
	@rm -fr cov-int $(PROJECT).tgz *.log foo 2> /dev/null
	@(cd tests && $(MAKE) -s clean)
	@$(call print-simple,"Cleaning","$(PWD)/doc/html")
	@rm -fr $(THIRDPART)/*/ doc/html 2> /dev/null

###################################################
# Sharable informations between all Makefiles
include $(M)/Makefile.footer
