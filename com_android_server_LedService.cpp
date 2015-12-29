#define LOG_TAG "LedService"

#include <jni.h>  /* /usr/lib/jvm/java-1.7.0-openjdk-amd64/include/ */
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/misc.h>
#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <hardware/led_hal.h>


namespace android
{
	static led_device_t* led_device;
	static jint fd;
	jint ledOpen(JNIEnv *env, jobject cls)
	{
		jint err;
   		hw_module_t* module;
		hw_device_t* device;

		ALOGI("native ledOpen ...");
		/* 1. hw_get_module */
		err = hw_get_module("led", (hw_module_t const**)&module);
		/* 2. get device : module->methods->open */
		if (err == 0) {
			 err = module->methods->open(module, NULL, &device);
			    if (err == 0) {
					/* 3. call led_open */
					led_device = (led_device_t*)(device);
			        return led_device->led_open(led_device);
			    } else {
			        return -1;
			    }
		}
		return -1;
	}
	
	jint ledCtrl(JNIEnv *env, jobject cls, jint which, jint status)
	{
		ALOGI("native ledCtrl %d, %d", which, status);
		return led_device->led_ctrl(led_device, which, status);
	}
	
	void ledClose(JNIEnv *env, jobject cls)
	{
		ALOGI("native ledClose ...");
		close(fd);
	}
	
	
	static const JNINativeMethod methods[] = {
		{"native_ledCtrl", "(II)I", (void *)ledCtrl},
		{"native_ledOpen", "()I", (void *)ledOpen},
		{"native_ledClose", "()V", (void *)ledClose},
	};

int register_android_server_LedService(JNIEnv *env)
{
    return jniRegisterNativeMethods(env, "com/android/server/LedService",
            methods, NELEM(methods));
}

};

