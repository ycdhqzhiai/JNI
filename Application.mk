APP_PLATFORM = android-24

APP_ABI := arm64-v8a

APP_CPPFLAGS += -frtti -fexceptions

#APP_ALLOW_MISSING_DEPS=true

#如何链接C++标准库,c++_static表示静态链接、c++_shared表示动态链接
APP_STL := c++_static

APP_OPTIM := release
