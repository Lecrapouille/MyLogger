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
TARGET = MyLoggerExample
DESCRIPTION = Example for $(PROJECT)
BUILD_TYPE = debug

###################################################
# Location of the project directory and Makefiles
#
P=..
M=$(P)/.makefile
include $(M)/Makefile.header

###################################################
# List of files to compile.
#
OBJS = main.o

###################################################
# Project defines
#
DEFINES += \
  -DPROJECT_TEMP_DIR=\"/tmp/$(TARGET)/\"

###################################################
# Link against MyLogger library (make install is
# needed).
PKG_LIBS += mylogger

###################################################
# Compile tutorials
all: $(TARGET)

###################################################
# Sharable informations between all Makefiles
include $(M)/Makefile.footer
