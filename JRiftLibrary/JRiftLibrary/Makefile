#############################################################################
#
# Filename    : Makefile
# Content     : Makefile for building linux / OSX versions of: jriftlibrary
# Instruction : You'll need both 64 bit and 32 libs available. Navigate in a 
#               shell to the directory where this Makefile is located and enter:
#
#               make                builds the release versions for both i386 
#                                   and amd64
#               make 32             builds the i386 version only
#               make 64             builds the amd64 version only
#
#               make clean          delete intermediate release object files 
#                                   and the library files for both the i386 and
#                                   amd64 versions
#               make clean32        delete intermediate release object files 
#                                   and the library files for the i386 version 
#                                   only
#               make clean64        delete intermediate release object files 
#                                   and the library files for the amd64 version 
#                                   only
#
#############################################################################
UNAME := $(shell uname)

OCULUS_SDK:=..
CPP:=g++
CPPFLAGS:=-O2
LDFLAGS:=-Wl,-O1,--sort-common,--as-needed,-z,relro

ifeq ($(UNAME), Darwin)

#MAC_SDK:=/Developer/SDKs/MacOSX10.7.sdk
MAC_SDK:=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk
MAC_MIN_VER:=10.7

all: libJRiftLibrary64.jnilib libJRiftLibrary.jnilib

32: libJRiftLibrary.jnilib

64: libJRiftLibrary64.jnilib

JRiftLibrary.o: JRiftLibrary.cpp
	g++ \
	-fno-rtti \
	-m32 \
	-isysroot ${MAC_SDK} \
	-mmacosx-version-min=${MAC_MIN_VER} \
	"-I${MAC_SDK}/usr/include" \
	"-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers" \
	"-I${MAC_SDK}/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	"-I${OCULUS_SDK}/LibOVR/Src" \
	-c JRiftLibrary.cpp \
	-o $@

JRiftLibrary64.o: JRiftLibrary.cpp
	g++ \
	-fno-rtti \
	-m64 \
	-isysroot ${MAC_SDK} \
	-mmacosx-version-min=${MAC_MIN_VER} \
	"-I${MAC_SDK}/usr/include" \
	"-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers" \
	"-I${MAC_SDK}/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	"-I${OCULUS_SDK}/LibOVR/Src" \
	-c JRiftLibrary.cpp \
	-o $@

libJRiftLibrary.jnilib: JRiftLibrary.o ${OCULUS_SDK}/LibOVR/OSX/Release/i386/libOVR.a
	g++ \
		-dynamiclib \
		-fno-rtti \
		-m32 \
		-framework ApplicationServices \
		-framework IOKit \
		-framework OpenGL \
		-L ${OCULUS_SDK}/LibOVR/Lib/OSX/Release/i386/ \
		-o $@ \
		JRiftLibrary.o \
		-lovr

libJRiftLibrary64.jnilib: JRiftLibrary64.o ${OCULUS_SDK}/LibOVR/Lib/OSX/Release/x86_64/libOVR.a 
	g++ \
		-dynamiclib \
		-fno-rtti \
		-m64 \
		-framework ApplicationServices \
		-framework IOKit \
		-framework OpenGL \
		-L ${OCULUS_SDK}/LibOVR/Lib/OSX/Release/x86_64/ \
		-o $@ \
		JRiftLibrary64.o \
		-lovr

${OCULUS_SDK}/LibOVR/Lib/OSX/Release/x86_64/libOVR.a:
	make ARCH=64 -C ${OCULUS_SDK}/LibOVR
${OCULUS_SDK}/LibOVR/OSX/Release/i386/libOVR.a : 
	make ARCH=32 -C ${OCULUS_SDK}/LibOVR
.PHONY: ${OCULUS_SDK}/LibOVR/Lib/OSX/Release/x86_64/libOVR.a ${OCULUS_SDK}/LibOVR/OSX/Release/i386/libOVR.a		
else
ifeq ($(UNAME), Linux)

LINUX_JDK_HOME:=/usr/lib/jvm/default

all: libJRiftLibrary64.so libJRiftLibrary.so

32: libJRiftLibrary.so

64: libJRiftLibrary64.so

JRiftLibrary.o: JRiftLibrary.cpp
	$(CPP) $(CFLAGS) \
	-m32 \
	-fPIC \
	-fno-rtti \
	"-I${LINUX_JDK_HOME}/include/" \
	"-I${LINUX_JDK_HOME}/include/linux/" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	"-I${OCULUS_SDK}/LibOVR/Src" \
	-c $< \
	-o $@

JRiftLibrary64.o: JRiftLibrary.cpp
	$(CPP) $(CFLAGS) \
	-m64 \
	-fPIC \
	-fno-rtti \
	"-I${LINUX_JDK_HOME}/include/" \
	"-I${LINUX_JDK_HOME}/include/linux/" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	"-I${OCULUS_SDK}/LibOVR/Src" \
	-c $< \
	-o $@

libJRiftLibrary.so: JRiftLibrary.o ${OCULUS_SDK}/LibOVR/Linux/Release/i386/libOVR.a 
	$(CPP) $(CFLAGS) $(LDFLAGS) \
		-m32 \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I${LINUX_JDK_HOME}/include/" \
		"-I${LINUX_JDK_HOME}/include/linux/" \
		-L ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/i386/ \
		$< \
		-lOculusVR -ludev -lXinerama

libJRiftLibrary64.so: JRiftLibrary64.o ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/libOVR.a 
	$(CPP) $(CFLAGS) $(LDFLAGS) \
		-m64 \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I${LINUX_JDK_HOME}/include/" \
		"-I${LINUX_JDK_HOME}/include/linux/" \
		-L ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/ \
		$< \
		-lOculusVR -ludev -lXinerama

${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/libOVR.a:
	make ARCH=64 -C ${OCULUS_SDK}/LibOVR
${OCULUS_SDK}/LibOVR/Linux/Release/i386/libOVR.a : 
	make ARCH=32 -C ${OCULUS_SDK}/LibOVR
.PHONY: ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/libOVR.a ${OCULUS_SDK}/LibOVR/Linux/Release/i386/libOVR.a
endif
endif

clean: cleanout clean32 clean64

clean32: cleanout
	make clean ARCH=32 -C ${OCULUS_SDK}/LibOVR

clean64: cleanout
	make clean ARCH=64 -C ${OCULUS_SDK}/LibOVR

cleanout:
	rm -f *.o
	rm -f a.out
	rm -f *.jnilib
	rm -rf *.so
