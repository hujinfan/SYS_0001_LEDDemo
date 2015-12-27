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

namespace android
{
	
	static jint fd;
	jint ledOpen(JNIEnv *env, jobject cls)
	{
		fd = open("/dev/leds", O_RDWR);
		ALOGI("native ledOpen: %d", fd);
		if(fd >= 0)
			return 0;
		else
			return -1;
	}
	
	jint ledCtrl(JNIEnv *env, jobject cls, jint which, jint status)
	{
		int ret = ioctl(fd, status, which);
		ALOGI("native ledCtrl: %d, %d, %d", which, status, ret);
		return ret;
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

