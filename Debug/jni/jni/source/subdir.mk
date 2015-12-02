################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jni/jni/source/ActiveTestShow.cpp \
../jni/jni/source/Arithmetic.cpp \
../jni/jni/source/AutoLog.cpp \
../jni/jni/source/BaseDataStream.cpp \
../jni/jni/source/BaseDisplay.cpp \
../jni/jni/source/BaseTroubleCode.cpp \
../jni/jni/source/Binary.cpp \
../jni/jni/source/BitStream.cpp \
../jni/jni/source/CommWithEcu.cpp \
../jni/jni/source/Controller.cpp \
../jni/jni/source/DataStreamCallback.cpp \
../jni/jni/source/DataStreamShow.cpp \
../jni/jni/source/Database.cpp \
../jni/jni/source/Des.cpp \
../jni/jni/source/Display.cpp \
../jni/jni/source/DisplayDiagnoseCommunication.cpp \
../jni/jni/source/EcuCommException.cpp \
../jni/jni/source/EncryptFile.cpp \
../jni/jni/source/Information.cpp \
../jni/jni/source/Log.cpp \
../jni/jni/source/MenuShow.cpp \
../jni/jni/source/MenuTree.cpp \
../jni/jni/source/MultiSelectShow.cpp \
../jni/jni/source/NewVsIndicator.cpp \
../jni/jni/source/Profile.cpp \
../jni/jni/source/ReceiveFrame.cpp \
../jni/jni/source/RunEnvironment.cpp \
../jni/jni/source/RunLog.cpp \
../jni/jni/source/SendFrame.cpp \
../jni/jni/source/SendReceivePacket.cpp \
../jni/jni/source/ShortTestShow.cpp \
../jni/jni/source/SpecialFuncTestShow.cpp \
../jni/jni/source/StdDataStream.cpp \
../jni/jni/source/TroubleCodeShow.cpp \
../jni/jni/source/VehicleInfo.cpp \
../jni/jni/source/rijndael.cpp \
../jni/jni/source/tinystr.cpp \
../jni/jni/source/tinyxml.cpp \
../jni/jni/source/tinyxmlerror.cpp \
../jni/jni/source/tinyxmlparser.cpp 

OBJS += \
./jni/jni/source/ActiveTestShow.o \
./jni/jni/source/Arithmetic.o \
./jni/jni/source/AutoLog.o \
./jni/jni/source/BaseDataStream.o \
./jni/jni/source/BaseDisplay.o \
./jni/jni/source/BaseTroubleCode.o \
./jni/jni/source/Binary.o \
./jni/jni/source/BitStream.o \
./jni/jni/source/CommWithEcu.o \
./jni/jni/source/Controller.o \
./jni/jni/source/DataStreamCallback.o \
./jni/jni/source/DataStreamShow.o \
./jni/jni/source/Database.o \
./jni/jni/source/Des.o \
./jni/jni/source/Display.o \
./jni/jni/source/DisplayDiagnoseCommunication.o \
./jni/jni/source/EcuCommException.o \
./jni/jni/source/EncryptFile.o \
./jni/jni/source/Information.o \
./jni/jni/source/Log.o \
./jni/jni/source/MenuShow.o \
./jni/jni/source/MenuTree.o \
./jni/jni/source/MultiSelectShow.o \
./jni/jni/source/NewVsIndicator.o \
./jni/jni/source/Profile.o \
./jni/jni/source/ReceiveFrame.o \
./jni/jni/source/RunEnvironment.o \
./jni/jni/source/RunLog.o \
./jni/jni/source/SendFrame.o \
./jni/jni/source/SendReceivePacket.o \
./jni/jni/source/ShortTestShow.o \
./jni/jni/source/SpecialFuncTestShow.o \
./jni/jni/source/StdDataStream.o \
./jni/jni/source/TroubleCodeShow.o \
./jni/jni/source/VehicleInfo.o \
./jni/jni/source/rijndael.o \
./jni/jni/source/tinystr.o \
./jni/jni/source/tinyxml.o \
./jni/jni/source/tinyxmlerror.o \
./jni/jni/source/tinyxmlparser.o 

CPP_DEPS += \
./jni/jni/source/ActiveTestShow.d \
./jni/jni/source/Arithmetic.d \
./jni/jni/source/AutoLog.d \
./jni/jni/source/BaseDataStream.d \
./jni/jni/source/BaseDisplay.d \
./jni/jni/source/BaseTroubleCode.d \
./jni/jni/source/Binary.d \
./jni/jni/source/BitStream.d \
./jni/jni/source/CommWithEcu.d \
./jni/jni/source/Controller.d \
./jni/jni/source/DataStreamCallback.d \
./jni/jni/source/DataStreamShow.d \
./jni/jni/source/Database.d \
./jni/jni/source/Des.d \
./jni/jni/source/Display.d \
./jni/jni/source/DisplayDiagnoseCommunication.d \
./jni/jni/source/EcuCommException.d \
./jni/jni/source/EncryptFile.d \
./jni/jni/source/Information.d \
./jni/jni/source/Log.d \
./jni/jni/source/MenuShow.d \
./jni/jni/source/MenuTree.d \
./jni/jni/source/MultiSelectShow.d \
./jni/jni/source/NewVsIndicator.d \
./jni/jni/source/Profile.d \
./jni/jni/source/ReceiveFrame.d \
./jni/jni/source/RunEnvironment.d \
./jni/jni/source/RunLog.d \
./jni/jni/source/SendFrame.d \
./jni/jni/source/SendReceivePacket.d \
./jni/jni/source/ShortTestShow.d \
./jni/jni/source/SpecialFuncTestShow.d \
./jni/jni/source/StdDataStream.d \
./jni/jni/source/TroubleCodeShow.d \
./jni/jni/source/VehicleInfo.d \
./jni/jni/source/rijndael.d \
./jni/jni/source/tinystr.d \
./jni/jni/source/tinyxml.d \
./jni/jni/source/tinyxmlerror.d \
./jni/jni/source/tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
jni/jni/source/%.o: ../jni/jni/source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -I"E:\resources\Android\develop\adt-bundle-windows-x86-20140702\android-ndk-r10e\platforms\android-19\arch-arm" -I"E:\resources\Android\develop\adt-bundle-windows-x86-20140702\android-ndk-r10e\sources\cxx-stl\gnu-libstdc++\4.8\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


