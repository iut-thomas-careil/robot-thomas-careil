/* 
 * File:   CB_TX1.h
 * Author: TABLE 6
 *
 * Created on 9 avril 2021, 16:25
 */

#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char* , int);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);
void SendOne();
unsigned char CB_TX1_IsTranmitting(void);
int CB_TX1_GetDataSize(void);
int CB_TX1_RemainingSize(void);

#endif	/* CB_TX1_H */

