#=====================================================================
## MyLogger: A Forth for SimTaDyn.
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
TARGET = $(PROJECT)-UnitTest
DESCRIPTION = Unit tests for $(PROJECT)
BUILD_TYPE = debug

###################################################
# Location of the project directory and Makefiles
#
P := ..
M := $(P)/.makefile
include $(M)/Makefile.header

###################################################
# List of files to compile.
#
OBJS  += ILogger.o Logger.o
OBJS  += LoggerTests.o main.o

###################################################
# Project defines
#
DEFINES += -Wno-unused-function -Wno-undef

###################################################
# Compilation options.
#
PKG_LIBS += gtest gmock

###################################################
# Code coverage. Comment these lines if coveraging
# is not desired.
#
LINKER_FLAGS +=

###################################################
# Inform Makefile where to find header files
#
INCLUDES += -I../src -I../include

###################################################
# Inform Makefile where to find *.cpp and *.o files
#
VPATH += ../src ../include

###################################################
# Compile SimTaDyn unit tests
all: $(TARGET)

###################################################
# Run unit tests.
.PHONY: unit-tests
.PHONY: check
unit-tests check: $(TARGET)
	@$(call print-to,"Running","$(TARGET)","$(RAPPORT)","")
	$(SANITIZER) ./$(BUILD)/$(TARGET) $(TU_OPTIONS) || (cat $(TARGET).log; return 1)

###################################################
# Sharable informations between all Makefiles
include $(M)/Makefile.footer
