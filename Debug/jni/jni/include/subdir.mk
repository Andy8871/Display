################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jni/jni/include/NewVsIndicator.cpp 

OBJS += \
./jni/jni/include/NewVsIndicator.o 

CPP_DEPS += \
./jni/jni/include/NewVsIndicator.d 


# Each subdirectory must supply rules for building sources it contributes
jni/jni/include/%.o: ../jni/jni/include/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -I"E:\resources\Android\develop\adt-bundle-windows-x86-20140702\android-ndk-r10e\platforms\android-19\arch-arm" -I"E:\resources\Android\develop\adt-bundle-windows-x86-20140702\android-ndk-r10e\sources\cxx-stl\gnu-libstdc++\4.8\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


