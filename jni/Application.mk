#调用C++ GNUSTL 库
APP_STL := gnustl_static

#开启编译器的异常捕获
APP_CPPFLAGS := -fexceptions

APP_CPPFLAGS += -std=c++11