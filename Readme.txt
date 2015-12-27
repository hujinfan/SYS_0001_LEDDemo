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