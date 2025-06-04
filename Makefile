###############################################################################
## MyLogger: A basic logger.
## Copyright 2025 Quentin Quadrat <lecrapouille@gmail.com>
##
## This file is part of MyLogger.
##
## MyLogger is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## MyLogger is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

###############################################################################
# Location of the project directory and Makefiles
#
P := .
M := $(P)/.makefile

###############################################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := $(PROJECT_NAME)
TARGET_DESCRIPTION := A simple logger
include $(M)/project/Makefile

###############################################################################
# Inform Makefile where to find header files
#
INCLUDES += $(P)/include $(P)/src $(P)/external/json/single_include

###############################################################################
# Inform Makefile where to find *.cpp files
#
VPATH += $(P)/src

###############################################################################
# Project defines
#
DEFINES +=

###############################################################################
# Make the list of files to compile
#
LIB_FILES := $(wildcard $(P)/src/*.cpp)

###############################################################################
# Set Libraries
#
LINKER_FLAGS +=

###############################################################################
# Sharable information between all Makefiles
#
include $(M)/rules/Makefile

###############################################################################
# Post-build rules
#
post-build: $(TARGET_STATIC_LIB_NAME) build-viewer build-demo

.PHONY: build-viewer
build-viewer: $(TARGET_STATIC_LIB_NAME)
	$(Q)$(MAKE) --no-print-directory --directory=Viewer all

.PHONY: build-demo
build-demo: build-viewer
	$(Q)$(MAKE) --no-print-directory --directory=doc/demo all