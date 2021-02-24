#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO(); 
    
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;
    
    InitTimer1(150);
    InitTimer23();
    InitTimer4(1000);
    InitPWM();
    InitADC1();
    
    robotState.vitesseGaucheCommandeCourante = 0;
    robotState.vitesseDroiteCommandeCourante = 0;
    
    unsigned int * result = ADCGetResult();
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    
    while (1) {
        if(ADCIsConversionFinished() == 1){
          ADCClearConversionFinishedFlag();
          result = ADCGetResult();
          unsigned int *result = ADCGetResult ();
          float volts = ((float)result[1]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreDroit=34/volts -5;
          volts = ((float)result[2]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreCentre = 34 / volts - 5;
          volts = ((float)result[4]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreGauche = 34 / volts - 5;
          volts = ((float)result[3]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreFI = 34 / volts - 5;
          volts = ((float)result[0]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreFN = 34 / volts - 5;
        }
        if(robotState.distanceTelemetreFI < 30){
            LED_ORANGE = 1;
        }
        else{
            LED_ORANGE = 0;
        }
        if(robotState.distanceTelemetreFN < 30){
            LED_BLANCHE = 1;
        }
        else{
            LED_BLANCHE = 0;
        }
        if(robotState.distanceTelemetreCentre < 30){
            LED_BLEUE = 1;
        }
        else{
            LED_BLEUE = 0;
        }
    } // fin main
}

unsigned char stateRobot;
unsigned int rdm=0;
unsigned int v=0;

void OperatingSystemLoop(void)
{
switch (stateRobot)
{
case STATE_ATTENTE:
timestamp = 0;
PWMSetSpeedConsigne(0, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_ATTENTE_EN_COURS;

case STATE_ATTENTE_EN_COURS:
if (timestamp > 1000)
stateRobot = STATE_AVANCE;
break;

case STATE_TOURNE_GAUCHE:
PWMSetSpeedConsigne(15, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
break;
case STATE_TOURNE_GAUCHE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_AVANCE: 
rdm++;
PWMSetSpeedConsigne(v, MOTEUR_DROIT);
PWMSetSpeedConsigne(v, MOTEUR_GAUCHE);
stateRobot = STATE_AVANCE_EN_COURS;
break;
case STATE_AVANCE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_GAUCHE_FI:
PWMSetSpeedConsigne(12, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_GAUCHE_FI_EN_COURS;
break;
case STATE_TOURNE_GAUCHE_FI_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_DROITE_FN:
PWMSetSpeedConsigne(0, MOTEUR_DROIT);
PWMSetSpeedConsigne(12, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_DROITE_FN_EN_COURS;
break;
case STATE_TOURNE_DROITE_FN_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_DROITE:
PWMSetSpeedConsigne(0, MOTEUR_DROIT);
PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_DROITE_EN_COURS;
break;
case STATE_TOURNE_DROITE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_SUR_PLACE_GAUCHE:
PWMSetSpeedConsigne(12, MOTEUR_DROIT);
PWMSetSpeedConsigne(-12, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
break;
case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_SUR_PLACE_DROITE:
PWMSetSpeedConsigne(-12, MOTEUR_DROIT);
PWMSetSpeedConsigne(12, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
break;
case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_COULOIR:
PWMSetSpeedConsigne(12, MOTEUR_DROIT);
PWMSetSpeedConsigne(12, MOTEUR_GAUCHE);
stateRobot = STATE_COULOIR_EN_COURS;
break;
case STATE_COULOIR_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

default :
stateRobot = STATE_ATTENTE;
break;
}
}

unsigned char nextStateRobot=0;

void SetNextRobotStateInAutomaticMode()
{
unsigned char positionObstacle = PAS_D_OBSTACLE;

//Détermination de la position des obstacles en fonction des télémètres
if ((robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN > 15)  //Obstacle en face 3
    ||
    (robotState.distanceTelemetreDroit < 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche < 25 ) //Obstacle en face trop près 7 8
    ||
    (robotState.distanceTelemetreDroit < 25 && robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreGauche < 25 )) //Obstacle tout autour 6 16
    ||
    (robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreFI < 15 && robotState.distanceTelemetreFN < 15)) // Obstacle face et extreme 19 20 23
    positionObstacle = OBSTACLE_EN_FACE;

else if((robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche < 25 && robotState.distanceTelemetreFN > 15) //Obstacle à gauche, à gauche et FI 2 10
        ||       
        (robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreGauche < 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN > 15) //Obstacle à gauche et face 11
        ||
        (robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreFI < 15 && robotState.distanceTelemetreFN > 15)) //Obstacle à gauche, face et FI 15 17 22
        ||
        (robotState.distanceTelemetreDroit < 25 && robotState.distanceTelemetreFI < 15) // 24 27 29 31
        positionObstacle = OBSTACLE_A_GAUCHE;

else if((robotState.distanceTelemetreDroit < 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN > 15) //Obstacle à droite 4
        ||
        (robotState.distanceTelemetreDroit < 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN < 15) //Obstacle à droite et FN 13
        ||
        (robotState.distanceTelemetreDroit < 25 && robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN > 15) //Obstacle à droite et face   12
        ||
        (robotState.distanceTelemetreCentre < 30 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN < 15 )) //Obstacle à droite, face et FN 14 18 21
        ||
        (robotState.distanceTelemetreGauche < 25 && robotState.distanceTelemetreFN < 15) // 25 26 28 30
        positionObstacle = OBSTACLE_A_DROITE;

else if(robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI < 15 && robotState.distanceTelemetreFN < 15) // Obstacle Couloir 9
positionObstacle = OBSTACLE_COULOIR;

else if(robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI < 15 && robotState.distanceTelemetreFN > 15) // Obstacle FI 1
positionObstacle = OBSTACLE_FI;

else if(robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN < 15) // Obstacle FN 5
positionObstacle = OBSTACLE_FN;

else if(robotState.distanceTelemetreDroit > 25 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 25 && robotState.distanceTelemetreFI > 15 && robotState.distanceTelemetreFN > 15) //pas d'obstacle 32
positionObstacle = PAS_D_OBSTACLE;

//Détermination de l?état à venir du robot
if (positionObstacle == OBSTACLE_A_DROITE)
nextStateRobot = STATE_TOURNE_GAUCHE;
else if (positionObstacle == OBSTACLE_A_GAUCHE)
nextStateRobot = STATE_TOURNE_DROITE;
else if (positionObstacle == OBSTACLE_EN_FACE)
{
    if(rdm % 2 == 0) nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    else nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
}
else if (positionObstacle == OBSTACLE_FN)
nextStateRobot = STATE_TOURNE_GAUCHE_FI;
else if (positionObstacle == OBSTACLE_FI)
nextStateRobot = STATE_TOURNE_DROITE_FN;
else if (positionObstacle == OBSTACLE_COULOIR)
nextStateRobot = STATE_COULOIR;
else if (positionObstacle == PAS_D_OBSTACLE)
{ // Rampe de vitesse
    if(robotState.distanceTelemetreDroit < 35 && robotState.distanceTelemetreCentre < 40 && robotState.distanceTelemetreGauche < 35 && robotState.distanceTelemetreFI < 25 && robotState.distanceTelemetreFN < 25)
        v = 15;
    else if(robotState.distanceTelemetreDroit < 50 && robotState.distanceTelemetreCentre < 60 && robotState.distanceTelemetreGauche < 50 && robotState.distanceTelemetreFI < 40 && robotState.distanceTelemetreFN < 40)
        v = 20;
    else if(robotState.distanceTelemetreDroit >= 50 && robotState.distanceTelemetreCentre >= 60 && robotState.distanceTelemetreGauche >= 50 && robotState.distanceTelemetreFI >= 40 && robotState.distanceTelemetreFN >= 40)
        v = 25;
nextStateRobot = STATE_AVANCE;
}

//Si l'on n'est pas dans la transition de l'étape en cours
if (nextStateRobot != stateRobot - 1)
stateRobot = nextStateRobot;
}
