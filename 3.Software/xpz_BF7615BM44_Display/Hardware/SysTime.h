#ifndef _SYSTIME_H_
#define _SYSTIME_H_


void SysTimeInit(void);

void Time_Basic(void);


void SysTime100us(void);


void SysTime1ms(void);

void SysTime10msPool();
void SysTime20msPool();
void SysTime100msPool();
void SysTime500msPool();

#endif
