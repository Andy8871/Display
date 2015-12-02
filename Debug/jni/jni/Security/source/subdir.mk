################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jni/jni/Security/source/CrcClass.cpp \
../jni/jni/Security/source/Des.cpp \
../jni/jni/Security/source/EncrypClass.cpp \
../jni/jni/Security/source/Rijndael.cpp \
../jni/jni/Security/source/SecurityBaseClass.cpp 

OBJS += \
./jni/jni/Security/source/CrcClass.o \
./jni/jni/Security/source/Des.o \
./jni/jni/Security/source/EncrypClass.o \
./jni/jni/Security/source/Rijndael.o \
./jni/jni/Security/source/SecurityBaseClass.o 

CPP_DEPS += \
./jni/jni/Security/source/CrcClass.d \
./jni/jni/Security/source/Des.d \
./jni/jni/Security/source/EncrypClass.d \
./jni/jni/Security/source/Rijndael.d \
./jni/jni/Security/source/SecurityBaseClass.d 


# Each subdirectory must supply rules for building sources it contributes
jni/jni/Security/source/%.o: ../jni/jni/Security/source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -I"E:\resources\Android\develop\adt-bundle-windows-x86-20140702\android-ndk-r10e\platforms\android-19\arch-arm" -I"E:\resources\Android\develop\adt-bundle-windows-x86-20140702\android-ndk-r10e\sources\cxx-stl\gnu-libstdc++\4.8\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


