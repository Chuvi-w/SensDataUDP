#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>
#include <endian.h>
#define LOG_TAG "simplejni native.cpp"

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

template <typename T>
inline void bswap(T &s)
{

    switch (sizeof(T))
    {
        case 2: {auto res = __swap16(*(uint16_t *)&s); s= *(T *)&res; break; }
        case 4: {auto res = __swap32(*(uint32_t *)&s); s= *(T *)&res;break; }
        case 8: {auto res = __swap64(*(uint64_t *)&s); s= *(T *)&res;break; }
        default: return;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_sensdataudp_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

typedef struct PointerData_s
{
    jint  nID;
    jint  PointerId;
   jfloat X;
   jfloat Y;
   jint   ToolType;
   jfloat Size;
   jfloat ToolMajor;
   jfloat ToolMinor;
   jfloat TouchMajor;
   jfloat TouchMinor;
   jfloat Pressure;
   jfloat Orientation;
}PointerData_t;
typedef  struct MotionEvent_s
{
    jlong   IsEndian;
    jlong   EventTime;
    jlong   DownTime;
    jint    Action;
    jint    ActionMasked;
    jfloat  XPrecision;
    jfloat  YPrecision;
    jfloat  RawX;
    jfloat  RawY;
    jint    ActionIndex;
    jint    PointerCount;
}MotionEvent_t;
extern "C" JNIEXPORT void JNICALL
#define OffValue(val) int off_##val=offsetof(MotionEvent_t,val);
Java_com_example_sensdataudp_MainActivity_ParseTouchEvent(JNIEnv *env, jobject obj/* this */,jbyteArray array)
{
/*
    OffValue(EventTime);
    OffValue(DownTime);
    OffValue(Action);
    OffValue(ActionMasked);
    OffValue(XPrecision);
    OffValue(YPrecision);
    OffValue(RawX);
    OffValue(RawY);
    OffValue(ActionIndex);
    OffValue(PointerCount);
    */
    jbyte* bufferPtr = env->GetByteArrayElements(array,NULL);
    jsize lengthOfArray = env->GetArrayLength(array);
    MotionEvent_t ev= *reinterpret_cast<MotionEvent_t*>(bufferPtr);
    PointerData_t *PtrData= reinterpret_cast<PointerData_t*>(bufferPtr+sizeof(MotionEvent_t));
    PointerData_t *evPtr;
    auto IsEndian=ev.IsEndian;
    if(IsEndian!=1)
    {
        bswap(ev.IsEndian);
        bswap(ev.EventTime);
        bswap(ev.DownTime);
        bswap(ev.Action);
        bswap(ev.ActionMasked);
        bswap(ev.XPrecision);
        bswap(ev.YPrecision);
        bswap(ev.RawX);
        bswap(ev.RawY);
        bswap(ev.ActionIndex);
        bswap(ev.PointerCount);
    }
    evPtr=new PointerData_t[ev.PointerCount];
    for(int i=0;i<ev.PointerCount;i++)
    {
        evPtr[i]=PtrData[i];
        if(!IsEndian)
        {
            bswap(evPtr[i].nID);
            bswap(evPtr[i].PointerId);
            bswap(evPtr[i].X);
            bswap(evPtr[i].Y);
            bswap(evPtr[i].ToolType);
            bswap(evPtr[i].Size);
            bswap(evPtr[i].ToolMajor);
            bswap(evPtr[i].ToolMinor);
            bswap(evPtr[i].TouchMajor);
            bswap(evPtr[i].TouchMinor);
            bswap(evPtr[i].Pressure);
            bswap(evPtr[i].Orientation);
        }
    }
    delete []evPtr;
    env->ReleaseByteArrayElements(array, bufferPtr, 0);
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    if(vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4)!=JNI_OK)
    {
        return 0;
    }

    return JNI_VERSION_1_2;
}