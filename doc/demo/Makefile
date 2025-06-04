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
P := ../..
M := $(P)/.makefile

###############################################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := mylogger-demo
TARGET_DESCRIPTION := Demo of MyLogger usage
include $(M)/project/Makefile

###############################################################################
# Inform Makefile where to find header files
#
INCLUDES += $(P)/include $(P)/external/json/single_include
VPATH += $(P)/doc/demo

###############################################################################
# Make the list of files to compile
#
SRC_FILES += demo.cpp
INCLUDES := $(P)/include src
VPATH := $(P)/demo
INTERNAL_LIBS := $(call internal-lib,$(PROJECT_NAME))

###############################################################################
# Sharable information between all Makefiles
#
include $(M)/rules/Makefile

$(INTERNAL_LIBS):