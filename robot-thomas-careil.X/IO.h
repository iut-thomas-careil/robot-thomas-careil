#ifndef IO_H
#define IO_H

//Affectation des pins des LEDS
#define LED_ORANGE _LATC10 
#define LED_BLEUE _LATG7
#define LED_BLANCHE _LATG6

// Prototypes fonctions
void InitIO();

//� D f i n i t i o n s  des  pins  pour  l e s  hacheurs  moteurs
#define MOTEUR1_IN1 _LATB14
#define MOTEUR1_IN2 _LATB15

// Configuration � s p c i f i q u e  du moteur  gauche
#define MOTEUR_GAUCHE_H_IO_OUTPUT MOTEUR1_IN1
#define MOTEUR_GAUCHE_L_IO_OUTPUT MOTEUR1_IN2
#define MOTEUR_GAUCHE_L_PWM_ENABLE IOCON1bits .PENL
#define MOTEUR_GAUCHE_H_PWM_ENABLE IOCON1bits .PENH
#define MOTEUR_GAUCHE_DUTY_CYCLE PDC1

#endif /* IO_H */

