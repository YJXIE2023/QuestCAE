/*----------------------------------------
控制字体颜色的宏
----------------------------------------*/

#ifndef QUEST_COLOR_UTILITIES_HPP
#define QUEST_COLOR_UTILITIES_HPP

#ifndef _WIN32
    #define RST "\x1B[0m"
    #define KRED "\x1B[31m"
    #define KGRN "\x1B[32m"
    #define KYEL "\x1B[33m"
    #define KBLU "\x1B[34m"
    #define KMAG "\x1B[35m"
    #define KCYN "\x1B[36m"
    #define KWHT "\x1B[37m"

    #define FRED(x) KRED x RST      // 红色字体
    #define FGRN(x) KGRN x RST      // 绿色字体
    #define FYEL(x) KYEL x RST      // 黄色字体
    #define FBLU(x) KBLU x RST      // 蓝色字体
    #define FMAG(x) KMAG x RST      // 紫红色字体
    #define FCYN(x) KCYN x RST      // 青色字体
    #define FWHT(x) KWHT x RST      // 白色字体

    #define BOLDFONT(x) "\x1B[1m" x RST    // 加粗字体
    #define FAINTFONT(x) "\x1B[2m" x RST    // 淡色字体
    #define ITAFONT(x) "\x1B[3m" x RST    // 斜体字体
    #define UNDL(x) "\x1B[4m" x RST    // 下划线字体
    #define INVFONT(x) "\x1B[7m" x RST    // 反显字体
    #define STRFONT(x) "\x1B[9m" x RST    // 带删除线字体
#else
    #define RST ""
    #define KRED ""
    #define KGRN ""
    #define KYEL ""
    #define KBLU ""
    #define KMAG ""
    #define KCYN ""
    #define KWHT ""

    #define FRED(x) KRED x RST
    #define FGRN(x) KGRN x RST
    #define FYEL(x) KYEL x RST
    #define FBLU(x) KBLU x RST
    #define FMAG(x) KMAG x RST
    #define FCYN(x) KCYN x RST
    #define FWHT(x) KWHT x RST

    #define BOLDFONT(x) "" x RST
    #define FAINTFONT(x) "" x RST
    #define ITAFONT(x) "" x RST
    #define UNDL(x) "" x RST
    #define INVFONT(x) "" x RST
    #define STRFONT(x) "" x RST
#endif

#endif // COLOR_UTILITIES_HPP