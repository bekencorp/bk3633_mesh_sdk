/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef TRI_TUPLE_DEFAULT_H_
#define TRI_TUPLE_DEFAULT_H_

/* default UUID for identifying the unprovisioned node */
#if 1
// 602 1565d0497400c999e984cffa4da4fdf9 78da07c11ca8
#define DEFAULT_PID 602
#define DEFAULT_SECRET "1565d0497400c999e984cffa4da4fdf9"
//#define DEFAULT_MAC "\x72\x62\x12\x4D\x23\xDC"  // 3AB7104D23DC
//#define DEFAULT_MAC "\x08\x8B\x12\x4D\x23\xDC"  // 088B124D23DC
//#define DEFAULT_MAC "\x09\x8B\x12\x4D\x23\xDC"  // 098B124D23DC
//#define DEFAULT_MAC "\x0A\x8B\x12\x4D\x23\xDC"  // 0A8B124D23DC
//#define DEFAULT_MAC "\x0B\x8B\x12\x4D\x23\xDC"  // 0B8B124D23DC
//#define DEFAULT_MAC "\x0C\x8B\x12\x4D\x23\xDC"  // 0C8B124D23DC
//#define DEFAULT_MAC "\x0D\x8B\x12\x4D\x23\xDC"  // 0D8B124D23DC
//#define DEFAULT_MAC "\x0E\x8B\x12\x4D\x23\xDC"  // 0E8B124D23DC
//#define DEFAULT_MAC "\x0F\x8B\x12\x4D\x23\xDC"  // 0F8B124D23DC
//#define DEFAULT_MAC "\x10\x8B\x12\x4D\x23\xDC"  // 108B124D23DC
#define DEFAULT_MAC "\x11\x8B\x12\x4D\x23\xDC"    // 118B124D23DC
//#define DEFAULT_MAC "\x12\x8B\x12\x4D\x23\xDC"  // 128B124D23DC
//#define DEFAULT_MAC "\x13\x8B\x12\x4D\x23\xDC"  // 138B124D23DC
#endif

#if 0
// 602 eddf3dd0b16684f0907b0ec71303a982 78da07fa41f5
#define DEFAULT_PID 602
#define DEFAULT_SECRET "eddf3dd0b16684f0907b0ec71303a982"
#define DEFAULT_MAC "78da07fa41f5"
#endif

#if 0
// 602 df97e6ee511b839f15cba8fb94a68811 78da07fa41f6
#define DEFAULT_PID 602
#define DEFAULT_SECRET "df97e6ee511b839f15cba8fb94a68811"
#define DEFAULT_MAC "78da07fa41f6"
#endif

#if 0
// 602 ac478f8cf28331dc89510062544b450e 78da07fa41f8
#define DEFAULT_PID 602
#define DEFAULT_SECRET "ac478f8cf28331dc89510062544b450e"
#define DEFAULT_MAC "78da07fa41f8"
#endif


#if 0
#define DEFAULT_PID 761
#define DEFAULT_SECRET "8a99315d87da5d24db777cb7a0f9d687"
#define DEFAULT_MAC { 0xd2, 0x0a, 0x02, 0x3a, 0x9e, 0x10 }
#endif

#endif // TRI_TUPLE_DEFAULT_H_