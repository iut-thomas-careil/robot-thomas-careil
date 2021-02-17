/* 
 * File:   timer.h
 * Author: TP-EO-5
 *
 * Created on 1 février 2021, 13:21
 */

#ifndef TIMER_H
#define TIMER_H

#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

void InitTimer23 (void);
void InitTimer1 (float freq);
void InitTimer4 (float freq);
void SetFreqTimer1(float freq);
void SetFreqTimer4(float freq);

extern unsigned long timestamp ;
#endif /* TIMER_H */
