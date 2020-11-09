#ifndef __KEY_H
#define __KEY_H

#define  KEY_NONE   0		// 无键
#define  KEY_UP     1		// 上键
#define  KEY_DOWN   2		// 下键
#define  KEY_LEFT   3		// 左键
#define  KEY_RIGHT  4		// 右键
#define  KEY_SEL    5		// 设置
#define  KEY_ESC    6	  // 退出

void InitKey(void);
void ScanKey(void);

#endif


