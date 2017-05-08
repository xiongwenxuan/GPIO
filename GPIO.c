#include     <stdio.h>
#include     <stdlib.h>  
#include     <unistd.h>  
#include     <sys/types.h>  
#include     <sys/stat.h>  
#include     <sys/ioctl.h>
#include     <fcntl.h>  
#include     <termios.h>  
#include     <errno.h>  
#include     <jni.h>  
#include     <android/log.h>


  
#define LOGI(...)((void)__android_log_print(ANDROID_LOG_INFO, "", __VA_ARGS__))//在logcat上打印信息用   
//#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)   
//#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define JNIREG_CLASS "com/fiscat/armpos/serialport/GPIOHelper"//指定要注册的类
#define RGC_IOC_MAGIC   0xE9

#define REACH_GPIO_SET_HIGH   _IOW(RGC_IOC_MAGIC,10, __u32)

#define REACH_GPIO_SET_LOW    _IOW(RGC_IOC_MAGIC,11, __u32)

#define REACH_GPIO_STATUS_GET _IOW(RGC_IOC_MAGIC,12, __u32)

static int getErrorno(int result)
{
	  if(result >= 0) return result;
	  return errno > 0 ? -1 * errno : errno;
}

/*
 * Class:     com.fiscat.armpos.serialport.GPIOHelper
 * Method:    native_open
 * Signature: ()INT
 * Open
 */  
JNIEXPORT jint JNICALL native_open
  (JNIEnv *env, jobject thiz, jstring path)
{
    int fd;
    /* Opening device */  
    {  
        LOGI("open...");
        jboolean iscopy;  
        const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
        fd = open(path_utf, O_RDONLY, O_NONBLOCK);
        LOGI("open() fd = %d", fd);
        (*env)->ReleaseStringUTFChars(env, path, path_utf);  
        if (fd == -1)
        {  
            /* Throw an exception */  
            LOGI("Cannot open port");  
            /* TODO: throw an exception */  
        }
    }
    return fd;
}


/*
 * Class:     com.fiscat.armpos.serialport.GPIOHelper
 * Method:    native_close
 * Close 
 */  
JNIEXPORT void JNICALL native_close
  (JNIEnv *env, jobject thiz, jint fd)
{
    close(fd);
}


/*
 * Class:     com.fiscat.armpos.serialport.GPIOHelper
 * Method:    native_Setgpio
 * Signature: ()INT
 * Operation
 */  
JNIEXPORT jint JNICALL native_Setgpio
  (JNIEnv *env, jobject thiz, jint fd, jint gpioPort, int gpioLevel)
{
   return ioctl(fd, gpioLevel == 0 ? REACH_GPIO_SET_LOW : REACH_GPIO_SET_HIGH, gpioPort);
}

/*
 * Class:     com.fiscat.armpos.serialport.GPIOHelper
 * Method:    setDataBits
 * Signature: ()INT
 * Operation
 */  
JNIEXPORT jint JNICALL native_Getgpio
  (JNIEnv *env, jobject thiz, jint fd, jint gpioPort)
{
   int ret = ioctl(fd, REACH_GPIO_STATUS_GET, &gpioPort);
   if(ret < 0) return ret;
   return gpioPort;
}


static JNINativeMethod gMethods[] = {
        { "open", "(Ljava/lang/String;)I",(void*)native_open},
        { "close", "(I)V",(void*) native_close},
        { "setgpio", "(III)I",(void*)native_Setgpio},
        { "getgpio", "(II)I",(void*)native_Getgpio}
};  
  
/*
* Register several native methods for one class.
*/
static int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	clazz = (*env)->FindClass(env, className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}
	if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}
  
/*
* Register native methods for all classes we know about.
*/
static int registerNatives(JNIEnv* env)
{
	if (!registerNativeMethods(env, JNIREG_CLASS, gMethods, 
                                 sizeof(gMethods) / sizeof(gMethods[0])))
		return JNI_FALSE;

	return JNI_TRUE;
}
  
/*
* Set some test stuff up.
*
* Returns the JNI version on success, -1 on failure.
*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{

        JNIEnv* env = NULL;
	jint result = -1;

    LOGI("JNI_OnLoad->GetEnv...");

        jint ret = (*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6);   
	if (ret != JNI_OK) {
                LOGI("JNI_OnLoad->GetEnv error.%d", ret);   
		return -1;
	}
        if(env == NULL)
        {
           LOGI("JNI_OnLoad->env is NULL");
           return -1;   
        }

    LOGI("JNI_OnLoad->registerNatives...");
	if (!registerNatives(env)) {//注册
                LOGI("JNI_OnLoad->registerNatives ERROR");
		return -1;
	}
	/* success -- return valid version number */
	result = JNI_VERSION_1_6;

	return result;

}

