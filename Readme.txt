(1) AIDL
1. 把 ILedService.aidl 放入虚拟机：/work/android-5.0.2/frameworks/base/core/java/android/os
2. 修改 /work/android-5.0.2/frameworks/base/Android.mk  添加一行（vi中，复制：yy,粘贴：p）
         core/java/android/os/IVibratorService.aidl \
+        core/java/android/os/ILedService.aidl \
3.# cd /work/android-5.0.2
4.# . setenv
5.# lunch
6.输入“full_tiny4412-eng”之前的数字标号
7. mmm frameworks/base （mmm命令用法：mmm <dir>）
8. cd /work/android-5.0.2/out/
9. find -name "ILedService.java"

10. 它会生成: ./out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/src/core/java/android/os/ILedService.java
11. 将产生的ILedService.java复制到Windows中

(2) Server : LedService.java (调用本地C函数来访问硬件)
             SystemServer.java（修改这个文件，ServiceManager.addService("led", new LedService());）

把新文件上传到服务器, 所在目录:
frameworks/base/services/java/com/android/server/SystemServer.java
frameworks/base/services/core/java/com/android/server/LedService.java

不需要修改 frameworks/base/services/core/Android.mk
它的内容里已经把该目录下所有JAVA文件自动包含进去了：
LOCAL_SRC_FILES += \
    $(call all-java-files-under,java)



(3) JNI : com_android_server_LedService.cpp
          onload.cpp

把新文件上传到服务器, 所在目录:
frameworks/base/services/core/jni/onload.cpp
frameworks/base/services/core/jni/com_android_server_LedService.cpp

修改 frameworks/base/services/core/jni/Android.mk :
  $(LOCAL_REL_DIR)/com_android_server_VibratorService.cpp \
+ $(LOCAL_REL_DIR)/com_android_server_LedService.cpp \


frameworks/base/services/core/Android.mk
frameworks/base/services/core/jni/Android.mk

编译：
$ mmm frameworks/base/services (编译)
$ make snod （生成system.img   out/target/product/tiny4412/system.img）
$ ./gen-img.sh
(报错：./gen-img.sh: line 31: mkimage: command not found
解决办法：sudo apt-get install u-boot-tools)
比较两个文件是否一样方法： diff file file
root@book-virtual-machine:/work/android-5.0.2# diff system.img out/target/product/tiny4412/system.img 
Binary files system.img and out/target/product/tiny4412/system.img differ


JNI  向上提供本地函数，向下加载HAL文件并调用HAL函数
HAL  负责访问驱动执行硬件操作

JNI和HAL都是用C语言写的，JNI来加载HAL的实质：就是如何使用dlopen函数来加载动态库

分析hardware.c、com_android_server_lights_LightsService.cpp文件：
e:\analyse\android_system_code\external\chromium_org\third_party\hwcplus\src\hardware.c


hw_get_module("led")
	
	1.模块名-->文件名
	hw_get_module_by_class(led, NULL, module)
		name = "led"
			property_get()      得到属性xxx，xxx是某个属性
			hw_module_exists()  判断是否存在led.xxx.so

	2.加载
			load(class_id, path, module);

				dlopen(filename)
				dlsym("HMI") 从SO文件中获得名为HMI的hw_module_t结构体
				strcmp(id, hmi->id) 判断名字是否一致(hmi->id, "led") 
总结：
	JNI怎么使用HAL (com_android_server_LedService.cpp   ---JNI文件)
	a.hw_get_module()获得一个hw_module_t结构体
	b.调用 module->methods->open(module, device_name, &device) 
	  获得一个hw_device_t结构体
      并且把hw_device_t结构体转换为设备自定义的结构体
	
	HAL怎么写（led_hal.c led_hal.h      ---HAL文件）
	a.实现一个名为HMI的hw_module_t结构体
	b.实现一个open函数，它会根据name返回一个自定义的结构体（如：led_device_t）
	  这个设备自定义的结构体的第一个成员是hw_device_t结构体
	  还可以定义设备相关的成员
	c.实现led_device_t结构体
	led_hal.c 参考vibrator.c


int hw_get_module(const char *id, const struct hw_module_t **module)
int hw_get_module_by_class(const char *class_id, const char *inst,
                           const struct hw_module_t **module)

static int hw_module_exists(char *path, size_t path_len, const char *name,
                            const char *subname)
led.XXX.so
led.tiny4412.so
led.exynos4.so
led.default.so
				
ro.hardware.%s
它用来判断"name" ."subname".so文件是否存在
查找的目录：
a.HAL_LIBRARY_PATH     //shell@tiny4412:/ $ echo $HAL_LIBRARY_PATH
b. /vendor/lib/hw      //开发板也没有此目录
c. /system/lib/hw      //.so文件在此目录下  shell@tiny4412:/ $ ls -ld /system/lib/hw/ 

property_get()：涉及到Android的属性系统
属性<键，值> <name,value>         从属性系统中根据名字获得一个值
shell@tiny4412:/ $ getprop ro.hardware
tiny4412
shell@tiny4412:/ $ getprop ro.product.board
tiny4412
shell@tiny4412:/ $ getprop ro.board.platform
exynos4



V2:
(3) JNI: 重新上传  (以下目录都是在/work/android-5.0.2/)
frameworks/base/services/core/jni/com_android_server_LedService.cpp

(4) HAL: led_hal.h
         led_hal.c 
把新文件上传到服务器, 所在目录:
hardware/libhardware/include/hardware/led_hal.h
hardware/libhardware/modules/led/led_hal.c  (led目录是自己创建的)
# cp ../vibrator/Android.mk ./              （复制上一级vibrator目录下的Android.mk，并修改）
hardware/libhardware/modules/led/Android.mk

Android.mk内容如下:
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := led.default
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_C_INCLUDES := hardware/libhardware
LOCAL_SRC_FILES := led_hal.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)


编译：
# . setenv
# lunch
输入“full_tiny4412-eng”之前的数字标号15
$ mmm frameworks/base/services
$ mmm hardware/libhardware/modules/led
$ make snod
$ ./gen-img.sh


打印信息简介：
a. 有三类打印信息: app, system, radio
   程序里使用 ALOGx, SLOGx, RLOGx来打印
b. x表示6种打印级别，有:
  V    Verbose
  D    Debug
  I    Info
  W    Warn
  E    Error
  F    Fatal

比如: 
#define LOG_TAG "LedHal"
ALOGI("led_open : %d", fd);

c. 打印出来的格式为:
      I/LedHal  ( 1987): led_open : 65
 (级别) LOG_TAG  进程号   打印信息  

d. 使用 logcat 命令查看  
   logcat LedHal:I *:S （表示过滤出LedHal信息内容，打印级别要>=I, * 表示LedHal信息全部打印 ，S -->表示其它信息不打印）
   
将编译出来的syste.img文件烧写到开发板，运行程序，并执行以下命令显示如下： 
shell@tiny4412:/ $ logcat LedHal:I *:S
--------- beginning of main
--------- beginning of system
I/LedHal  ( 1990): led_open: 65
I/LedHal  ( 1990): led_ctrl: 0, 1, 0
I/LedHal  ( 1990): led_ctrl: 1, 1, 0
I/LedHal  ( 1990): led_ctrl: 2, 1, 0
I/LedHal  ( 1990): led_ctrl: 3, 1, 0

程序正常运行，代码成功！
   
上传本次版本代码到GitHub：
1.git add -A
2.git commit -m "v3, add hal base on jni"
3.git tag v3
4.git push origin master
5.git push origin --tags

