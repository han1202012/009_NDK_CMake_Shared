# 009_NDK_CMake_Shared



<br>
<br>

**① CSDN 博客地址 :** [【Android NDK 开发】Android Studio 使用 CMake 导入动态库 ( 构建脚本路径配置 | 指定动态库查找路径 | 链接动态库 )](https://hanshuliang.blog.csdn.net/article/details/104349622)

**②  博客资源下载地址 :** [https://download.csdn.net/download/han1202012/12162546](https://download.csdn.net/download/han1202012/12162546)

**③ 示例代码 GitHub 地址 :** [https://github.com/han1202012/009_NDK_CMake_Shared](https://github.com/han1202012/009_NDK_CMake_Shared)



<br>
<br>

**参考博客 :** [【Android NDK 开发】Android Studio 使用 CMake 导入静态库 ( CMake 简介 | 构建脚本路径配置 | 引入静态库 | 指定静态库路径 | 链接动态库 )](https://hanshuliang.blog.csdn.net/article/details/104337399)


<br>
<br>

#### I . CMake 引入动态库与静态库区别

---

<br>

**1 . CMake 引入静态库 :** <font color=red>使用 add_library() 导入静态库 , <font color=blue>set_target_properties() 设置静态库路径 ; 



```shell
# 引入静态库
#       ① 参数 1 ( add ) : 设置引入的静态库名称
#       ② 参数 2 ( SHARED ) : 设置引入的函数库类型 : ① 静态库 STATIC ② 动态库 SHARED
#       ③ 参数 3 ( IMPORTED ) : 表示引入第三方静态库 , 导入静态库 , 相当于预编译静态库
#                                   后续还需要设置导入路径 , 配合该配置使用
add_library(
        # 设置引入的静态库名称
        add

        # 设置引入的函数库类型为静态库
        STATIC

        # 表示引入第三方静态库
        IMPORTED)

# 设置上述静态库的导入路径
#       设置目标属性参数 :
#           ① 参数 1 ( add ) : 要设置哪个函数库的属性
#           ② 参数 2 ( PROPERTIES ) : 设置目标属性
#           ③ 参数 3 ( IMPORTED_LOCATION ) : 设置导入路径
#           ④ 参数 4 : 配置静态库的文件路径
set_target_properties(
        # 设置目标
        add

        # 设置属性
        PROPERTIES

        # 导入路径
        IMPORTED_LOCATION

        # ${CMAKE_SOURCE_DIR} 是本 CMakeList.txt 构建脚本的路径 , 是 CMake 工具内置的变量
        #       Android CMake 也内置了一些变量 , 如 ANDROID_ABI
        ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libadd.a)
```

<br>

><font color=purple>使用上面的方式引入动态库会出现于 Android.mk 配置一样的问题 , 6.0 以上的 Android 系统在运行时出现找不到路径的问题 ;
><font color=orange>如果引用动态库 , 则不能用这种方式 , 要使用下面的动态库引入方式 ; 

<br>



**2 . CMake 引入动态库 :** <font color=red>使用 set() , 指定一个 CMAKE_CXX_FLAGS 编译器参数 , 在编译器参数后添加 -L 参数指定动态库查找目录 ; 

```shell
# 设置变量
# CMAKE_CXX_FLAGS 表示会将 C++ 的参数传给编译器
# CMAKE_C_FLAGS 表示会将 C 参数传给编译器

# 参数设置 : 传递 CMAKE_CXX_FLAGS C+= 参数给编译器时 , 在 该参数后面指定库的路径
#   CMAKE_SOURCE_DIR 指的是当前的文件地址
#   -L 参数指定动态库的查找路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}")
```

>**原理参考 :** [【Android NDK 开发】NDK 交叉编译 ( NDK 函数库目录 | Linux 交叉编译环境搭建 | 指定头文件目录 | 指定函数库目录 | 编译 Android 命令行可执行文件 ) : V . 指定编译的库文件](https://hanshuliang.blog.csdn.net/article/details/104235410#V___207)



<br>
<br>

#### II . Android Studio 中 CMake 引入动态库流程

---

<br>

**Android Studio 中 CMake 引入静态库流程 :** 

<br>

**1 . build.gradle 配置 CMake 编译选项 :** <font color=purple>在 Module 级别的 build.gradle 脚本中配置 CMake 编译选项 ; 

```java
        // I . NDK 配置 1 : 配置 AS 工程中的 C/C++ 源文件的编译


        //     defaultConfig 内部的 externalNativeBuild 配置的是配置 AS 工程的 C/C++ 源文件编译参数
        //     defaultConfig 外部的 externalNativeBuild 配置的是 CMakeList.txt 或 Android1.mk 构建脚本的路径
        externalNativeBuild {
            cmake {
                cppFlags ""

                //配置编译 C/C++ 源文件为哪几个 CPU 指令集的函数库 (arm , x86 等)
                abiFilters "armeabi-v7a"
            }
            /*ndkBuild{
                abiFilters "armeabi-v7a" *//*, "arm64-v8a", "x86", "x86_64"*//*
            }*/
        }
```

<br>

**2 . build.gradle 配置 NDK 打包选项 :** <font color=blue>在 Module 级别的 build.gradle 脚本中配置 NDK 打包选项 ; 

```java
        // II . NDK 配置 2 : 配置 AS 工程中的 C/C++ 源文件的编译


        //配置 APK 打包 哪些动态库
        //  示例 : 如在工程中集成了第三方库 , 其提供了 arm, x86, mips 等指令集的动态库
        //        那么为了控制打包后的应用大小, 可以选择性打包一些库 , 此处就是进行该配置
        ndk{
            // 打包生成的 APK 文件指挥包含 ARM 指令集的动态库
            abiFilters "armeabi-v7a" /*, "arm64-v8a", "x86", "x86_64"*/
        }
```

<br>

**3 . build.gradle 配置 CMake 构建脚本 CMakeList.txt 路径 :** <font color=red>在 Module 级别的 build.gradle 脚本中配置 Android.mk 构建脚本的路径 ;

```java
    // III . NDK 配置  : 配置 AS 工程中的 C/C++ 源文件的编译构建脚本


    // 配置 NDK 的编译脚本路径
    // 编译脚本有两种 ① CMakeList.txt ② Android1.mk
    //     defaultConfig 内部的 externalNativeBuild 配置的是配置 AS 工程的 C/C++ 源文件编译参数
    //     defaultConfig 外部的 externalNativeBuild 配置的是 CMakeList.txt 或 Android1.mk 构建脚本的路径
    externalNativeBuild {

        // 配置 CMake 构建脚本 CMakeLists.txt 脚本路径
        cmake {
            path "src/main/cpp/CMakeLists.txt"
            version "3.10.2"
        }

        // 配置 Android1.mk 构建脚本路径
        /*ndkBuild{
            //path "src/main/ndkBuild_Shared/Android.mk"
            path "src/main/ndkBuild_Static/Android.mk"
        }*/
    }
```

<br>


**4 . CMake 构建脚本 CMakeList.txt 设置动态库查找路径 :** 

```shell
# 设置变量
# CMAKE_CXX_FLAGS 表示会将 C++ 的参数传给编译器
# CMAKE_C_FLAGS 表示会将 C 参数传给编译器

# 参数设置 : 传递 CMAKE_CXX_FLAGS C+= 参数给编译器时 , 在 该参数后面指定库的路径
#   CMAKE_SOURCE_DIR 指的是当前的文件地址
#   -L 参数指定动态库的查找路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}")
```

<br>

**5 . CMake 构建脚本 CMakeList.txt 链接静态库 :** 

```shell
# 链接函数库
#       参数 1 : 本构建脚本要生成的动态库目 标
#       参数 2 ~ ... : 后面是之前预编译的动态库或静态库 , 或引入的动态库
target_link_libraries(
        native-lib

        # 表示 编译 native-lib 模块, 要链接 add 模块
        add

        ${log-lib})
```













<br>
<br>

#### III . 指定动态库查找路径

---

<br>


**导入第三方函数库路径配置 :** <font color=red>通过设置编译器参数方式实现 ; 

<br>

**① 编译器类型 :** <font color=purple>CMAKE_CXX_FLAGS 表示 C++ 编译器参数 , CMAKE_C_FLAGS 表示 C 编译器参数 ; 

**② 参数追加 :** <font color=blue>set 语句 , 在 CMAKE_CXX_FLAGS 编译器参数后 , 追加了 "-L\${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}" 内容 ; 



```shell
# 设置变量
# CMAKE_CXX_FLAGS 表示会将 C++ 的参数传给编译器
# CMAKE_C_FLAGS 表示会将 C 参数传给编译器

# 参数设置 : 传递 CMAKE_CXX_FLAGS C+= 参数给编译器时 , 在 该参数后面指定库的路径
#   CMAKE_SOURCE_DIR 指的是当前的文件地址
#   -L 参数指定动态库的查找路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}")
```

>**原理参考 :** [【Android NDK 开发】NDK 交叉编译 ( NDK 函数库目录 | Linux 交叉编译环境搭建 | 指定头文件目录 | 指定函数库目录 | 编译 Android 命令行可执行文件 ) : V . 指定编译的库文件](https://hanshuliang.blog.csdn.net/article/details/104235410#V___207)




<br>
<br>

#### IV . 链接函数库

---

<br>

**链接函数库 :** <font color=blue>这里注意第一个参数必须是要生成的动态库模块 ; 

```shell
# 链接函数库
#       参数 1 : 本构建脚本要生成的动态库目标
#       参数 2 ~ ... : 后面是之前预编译的动态库或静态库 , 或引入的动态库
target_link_libraries(
        native-lib

        # 表示 编译 native-lib 模块, 要链接 add 模块
        add

        ${log-lib})
```


<br>
<br>

#### V . Module 级别的 build.gradle 完整配置代码 

---

<br>

```java
apply plugin: 'com.android.application'

android {
    compileSdkVersion 29
    buildToolsVersion "29.0.0"
    defaultConfig {
        applicationId "kim.hsl.cmake"
        minSdkVersion 15
        targetSdkVersion 29
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"



        // I . NDK 配置 1 : 配置 AS 工程中的 C/C++ 源文件的编译


        //     defaultConfig 内部的 externalNativeBuild 配置的是配置 AS 工程的 C/C++ 源文件编译参数
        //     defaultConfig 外部的 externalNativeBuild 配置的是 CMakeList.txt 或 Android1.mk 构建脚本的路径
        externalNativeBuild {
            cmake {
                cppFlags ""

                //配置编译 C/C++ 源文件为哪几个 CPU 指令集的函数库 (arm , x86 等)
                abiFilters "armeabi-v7a"
            }
            /*ndkBuild{
                abiFilters "armeabi-v7a" *//*, "arm64-v8a", "x86", "x86_64"*//*
            }*/
        }



        // II . NDK 配置 2 : 配置 AS 工程中的 C/C++ 源文件的编译


        //配置 APK 打包 哪些动态库
        //  示例 : 如在工程中集成了第三方库 , 其提供了 arm, x86, mips 等指令集的动态库
        //        那么为了控制打包后的应用大小, 可以选择性打包一些库 , 此处就是进行该配置
        ndk{
            // 打包生成的 APK 文件指挥包含 ARM 指令集的动态库
            abiFilters "armeabi-v7a" /*, "arm64-v8a", "x86", "x86_64"*/
        }

    }



    // III . NDK 配置  : 配置 AS 工程中的 C/C++ 源文件的编译构建脚本


    // 配置 NDK 的编译脚本路径
    // 编译脚本有两种 ① CMakeList.txt ② Android1.mk
    //     defaultConfig 内部的 externalNativeBuild 配置的是配置 AS 工程的 C/C++ 源文件编译参数
    //     defaultConfig 外部的 externalNativeBuild 配置的是 CMakeList.txt 或 Android1.mk 构建脚本的路径
    externalNativeBuild {

        // 配置 CMake 构建脚本 CMakeLists.txt 脚本路径
        cmake {
            path "src/main/cpp/CMakeLists.txt"
            version "3.10.2"
        }

        // 配置 Android1.mk 构建脚本路径
        /*ndkBuild{
            //path "src/main/ndkBuild_Shared/Android.mk"
            path "src/main/ndkBuild_Static/Android.mk"
        }*/
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])
    implementation 'androidx.appcompat:appcompat:1.1.0'
    implementation 'androidx.constraintlayout:constraintlayout:1.1.3'
    testImplementation 'junit:junit:4.12'
    androidTestImplementation 'androidx.test:runner:1.2.0'
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.2.0'
}

```


<br>
<br>

#### VI . CMakeList.txt 完整配置代码 

---

<br>

```shell

# 指定 CMake 最低版本
cmake_minimum_required(VERSION 3.4.1)

# 设置函数库编译
add_library( # 参数 1 : 设置生成的动态库名称
        native-lib

        # 参数 2 : 设置生成的函数库类型 : ① 静态库 STATIC ② 动态库 SHARED
        SHARED

        # 参数 3 : 配置要编译的源文件
        native-lib.cpp)


# 使用下面的方式引入动态库会出现于 Android.mk 配置一样的问题 , 6.0 以上的 Android 系统在运行时出现找不到路径的问题

# 引入动态库
#add_library(add SHARED IMPORTED)
# 设置函数库的导入路径
#set_target_properties(add PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libadd.so)


# 打印日志信息
#       ${ANDROID_ABI} 的作用是获取当前的 CPU 指令集架构
#           当本次编译 armeabi-v7a CPU 架构时 , ${ANDROID_ABI} 值为 armeabi-v7a
#           当本次编译 x86 CPU 架构时 , ${ANDROID_ABI} 值为 x86
message("CMAKE_SOURCE_DIR : ${CMAKE_SOURCE_DIR}, ANDROID_ABI : ${ANDROID_ABI}")


# 到预设的目录查找 log 库 , 将找到的路径赋值给 log-lib
#       这个路径是 NDK 的 ndk-bundle\platforms\android-29\arch-arm\usr\lib\liblog.so
#       不同的 Android 版本号 和 CPU 架构 需要到对应的目录中查找 , 此处是 29 版本 32 位 ARM 架构的日志库
#
# 可以不配置 :
#       可以不进行该配置, 直接在后面的 target_link_libraries 中链接 log 也不会出错
find_library(
        log-lib

        log)

# 打印日志库位置
message(${log-lib})


# 设置变量
# CMAKE_CXX_FLAGS 表示会将 C++ 的参数传给编译器
# CMAKE_C_FLAGS 表示会将 C 参数传给编译器

# 参数设置 : 传递 CMAKE_CXX_FLAGS C+= 参数给编译器时 , 在 该参数后面指定库的路径
#   CMAKE_SOURCE_DIR 指的是当前的文件地址
#   -L 参数指定动态库的查找路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}")



# 链接函数库
#       参数 1 : 本构建脚本要生成的动态库目标
#       参数 2 ~ ... : 后面是之前预编译的动态库或静态库 , 或引入的动态库
target_link_libraries(
        native-lib

        # 表示 编译 native-lib 模块, 要链接 add 模块
        add

        ${log-lib})
```



<br>
<br>

#### VII . 博客资源

---

<br>

**博客相关资源 :**

<br>

**① CSDN 博客地址 :** [【Android NDK 开发】Android Studio 使用 CMake 导入动态库 ( 构建脚本路径配置 | 指定动态库查找路径 | 链接动态库 )](https://hanshuliang.blog.csdn.net/article/details/104349622)

**②  博客资源下载地址 :** [https://download.csdn.net/download/han1202012/12162546](https://download.csdn.net/download/han1202012/12162546)

**③ 示例代码 GitHub 地址 :** [https://github.com/han1202012/009_NDK_CMake_Shared](https://github.com/han1202012/009_NDK_CMake_Shared)

