
#include <jni.h>

#include <stdio.h>
#include <stdlib.h>

#include <android/log.h>


//调用 libadd.so 动态库中的方法
extern "C" {    //注意在 C++ 中调用 C 语言方法 , 需要做兼容设置
    extern int add(int a, int b);
}

extern "C" JNIEXPORT jstring JNICALL
Java_kim_hsl_cmake_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject obj) {

    //调用动态库中的函数
    int sum = add(1, 2);

    // Logcat 打印计算结果
    __android_log_print(ANDROID_LOG_INFO, "JNI_TAG", "Native Caculate (CMake Build) : %d + %d = %d", 1, 2, sum);

    //将加法运算转为字符串 , 字符串如果不够长就报错 ...
    char str[100] = "0";
    //字符串格式化
    sprintf(str, "Native Caculate : Shared Library (CMake Build) : %d + %d = %d", 1, 2, sum);

    return env->NewStringUTF(str);
}

