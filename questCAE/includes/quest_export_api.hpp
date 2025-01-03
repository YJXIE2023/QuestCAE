/*--------------------------------------------------
用于定义在不同编译器和操作系统环境下导出和导入符号的宏
---------------------------------------------------*/

#ifndef QUEST_QUEST_EXPORT_API_H
#define QUEST_QUEST_EXPORT_API_H

#undef QUEST_API_EXPORT
#undef QUEST_API_IMPORT

#if __WIN32
    #if defined(__MINGW32__)||defined(__MINGW64__)
        #define QUEST_API_EXPORT __attribute__((visibility("default")))
        #define QUEST_API_IMPORT __attribute__((visibility("default")))
    #else
        #define QUEST_API_EXPORT __declspec(dllexport)
        #define QUEST_API_IMPORT __declspec(dllimport)
    #endif
#else
    #define QUEST_API_EXPORT __attribute__((visibility("default")))
    #define QUEST_API_IMPORT __attribute__((visibility("default")))
#endif


#define QUEST_EXPAND(A) A

#define QUEST_API_CALL(x,T1,T2,T3,...) T3

#ifndef QUEST_API_NO_DLL
    #define QUEST_API(...)    \
        QUEST_EXPAND(QUEST_API_CALL(,##__VA_ARGS__,QUEST_API_EXPORT,QUEST_API_IMPORT))
    #define QUEST_NO_EXPORT(...)
#else
    #define QUEST_API(...)    
    #define QUEST_NO_EXPORT(...)
#endif


#undef QUEST_API_EXTERN
#ifdef __WIN32
    #if defined(__MINGW32__)||defined(__MINGW64__)
        #define QUEST_API_EXTERN extern
    #else
        #define QUEST_API_EXTERN
    #endif
#else
    #define QUEST_API_EXTERN extern
#endif


#endif //QUEST_EXPORT_API_H