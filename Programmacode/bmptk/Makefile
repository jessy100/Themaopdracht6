#############################################################################
# 
# Makefile for bmptk documentation, zip file, web pages, etc.
#
# Copyright (c) 2012 .. 2014 Wouter van Ooijen (www.voti.nl)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
#
# This file makes some meta-stuff that is probably 
# probably interesting only for a bmptk maintainer.
#
# targets:
#    clean                     : total cleanup 
#    doc, docs, documentation  : create documentation (docs subdir)
#    build, examples           : build all examples
#    push                      : clean and git push (does not work??)
#    zip                       : create bmptk.zip, includes documentation
#    www                       : create webpages (www subdir, includes zip)
#    ftp                       : create webpages and ftp to website
#                                (won't work from your PC ;) )
#    th04                      : create .zip for V1TH04 course
#    cpse2                     : create .zip for V2CPSE2 course
# 
#############################################################################

.PHONY: clean doc docs documentation build all examples zip push tests

docs : doc
documentation: doc
all: doc examples tests
build: examples tests

REDIRECT := <META HTTP-EQUIV='refresh' CONTENT='0;URL=./docs/index.html'>

doc: 
		Doxygen docsrc/Doxyfile
		echo "$(REDIRECT)" >index.html
		@echo "**** documentation build completed succesfully"
		
examples:
		cd examples && make build
		@echo "**** examples build completed succesfully"

tests:
		cd tests && make build
		@echo "**** tests build completed succesfully"

zip: clean doc		
		rm -rf index.html 
		pkzip25 -add -path=relative -recurse bmptk.zip 
		@echo "**** zip build completed succesfully"
		
clean:
		rm -rf index.html docs bmptk.zip th04.zip cpse2.zip transfer
		cd examples && make clean	
		cd tests && make clean	
		@echo "**** cleanup completed succesfully"
		
push:
		make clean
		git push
		
ftp: zip
		mkdir transfer
		-cp ../_ftp_crc_s transfer
		cp -R docsrc/www/* transfer
		cp bmptk.zip transfer
		cp -R docs transfer/docs
		python \ftpbmptk.py
		cp transfer/_ftp_crc_s ..
		@echo "**** ftp transfer completed succesfully"
      
TH04LIST += license.txt license_1_0.txt
TH04LIST += Makefile Makefile.inc Makefile.local
TH04LIST += bmptk.h
TH04LIST += targets/bmptk_lpc1114* targets/cmsis/11xx/*/* 
TH04LIST += tools/* tools/image_sizes/*
TH04LIST += index.html docs/* docs/search/*
TH04LIST += examples/v1th04/*
TH04LIST += examples/v1th04/blink/* 
TH04LIST += examples/v1th04/switch/* 
TH04LIST += examples/v1th04/beep/*
TH04LIST += examples/v1th04/440Hz/*
TH04LIST += examples/v1th04/timer/*
TH04LIST += examples/v1th04/sw-uart/*
TH04LIST += examples/v1th04/servo/*
TH04LIST += examples/v1th04/ds1990/*
TH04LIST += examples/v1th04/i2c/*
TH04LIST += examples/v1th04/lcd5510/*

th04: clean doc
		pkzip25 -add -path=current th04.zip $(TH04LIST)
      
CPSE2LIST += license.txt license_1_0.txt
CPSE2LIST += Makefile Makefile.inc Makefile.local
CPSE2LIST += bmptk.h
CPSE2LIST += targets/*
CPSE2LIST += targets/cortex/* targets/cortex/cmsis/11xx/inc/* 
CPSE2LIST += targets/cortex/cmsis/lpc800/inc/* 
CPSE2LIST += targets/win/*
CPSE2LIST += tools/* tools/image_sizes/*
CPSE2LIST += hwcpp/* hwcpp/*/*
CPSE2LIST += examples/v2cpse2/*
# CPSE2LIST += examples/v2cpse2/decorators/*
CPSE2LIST += examples/v2cpse2/hello/*
CPSE2LIST += examples/v2cpse2/libraries/*
CPSE2LIST += examples/v2cpse2/libraries/graphics/*
CPSE2LIST += examples/v2cpse2/lines/*
CPSE2LIST += examples/v2cpse2/subframes/*
CPSE2LIST += examples/v2cpse2/tee/*
CPSE2LIST += examples/v2cpse2/week1-make/*
      
cpse2: clean
		pkzip25 -add -path=current cpse2.zip $(CPSE2LIST)
    
		
		