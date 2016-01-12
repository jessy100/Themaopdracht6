#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2013
# 
#############################################################################

# Specify the location of the bmptk library
BMPTK      := ../bmptk

# Specify target chip or board or system, default is nds
TARGET     := db103

# Specify the project name, the default is main (.cpp, .c, or .asm)
# this is also the main .cpp file to compile
# the default is main.cpp (or main.c, or main.asm)
PROJECT    := 

# Specify project-specific files (other than the main, if any)
OBJECTS    := 
SOURCES    :=  libserial.cpp  pRTOS.cpp	libfiber.cpp
HEADERS    :=  libserial.h 		pRTOS.h   libfiber.h
LIBRARIES  := 
SEARCH     := ../Software/libserial ../Software/pRTOS/source

# specify (extra) results you want to get (if any)
RESULTS    := 

# The Makefile.inc does all the work
include $(BMPTK)/Makefile.inc
