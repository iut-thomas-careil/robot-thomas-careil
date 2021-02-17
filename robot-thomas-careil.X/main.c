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
          float volts = ((float)result[2]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreDroit=34/volts -5;
          volts = ((float)result[1]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreCentre = 34 / volts - 5;
          volts = ((float)result[0]) * 3.3 / 4096 * 3.2;
          robotState.distanceTelemetreGauche = 34 / volts - 5;
        }
        if(robotState.distanceTelemetreGauche < 30){
            LED_ORANGE = 1;
        }
        else{
            LED_ORANGE = 0;
        }
        if(robotState.distanceTelemetreDroit < 30){
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

case STATE_AVANCE:
PWMSetSpeedConsigne(20, MOTEUR_DROIT);
PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
stateRobot = STATE_AVANCE_EN_COURS;
break;
case STATE_AVANCE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_GAUCHE:
PWMSetSpeedConsigne(15, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
break;
case STATE_TOURNE_GAUCHE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_GAUCHE_FI:
PWMSetSpeedConsigne(10, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_GAUCHE_FI_EN_COURS;
break;
case STATE_TOURNE_GAUCHE_FI_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_DROITE_FN:
PWMSetSpeedConsigne(0, MOTEUR_DROIT);
PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
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
PWMSetSpeedConsigne(15, MOTEUR_DROIT);
PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
break;
case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_SUR_PLACE_DROITE:
PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
break;
case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_CRENEAU:
PWMSetSpeedConsigne(12, MOTEUR_DROIT);
PWMSetSpeedConsigne(16, MOTEUR_GAUCHE);
stateRobot = STATE_CRENEAU_EN_COURS;
break;
case STATE_CRENEAU_EN_COURS:
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
if ( robotState.distanceTelemetreDroit < 25 && 
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) //Obstacle à droite
positionObstacle = OBSTACLE_A_DROITE;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche < 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) //Obstacle à gauche
positionObstacle = OBSTACLE_A_GAUCHE;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre < 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) //Obstacle en face
positionObstacle = OBSTACLE_EN_FACE;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre < 20 &&
robotState.distanceTelemetreGauche < 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) // 2-3
positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
else if(robotState.distanceTelemetreDroit < 25 &&
robotState.distanceTelemetreCentre < 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) // 3-4
positionObstacle = OBSTACLE_EN_FACE_DROITE;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche < 25 &&
robotState.distanceTelemetreFI < 15 &&
robotState.distanceTelemetreFN > 15) //1-2
positionObstacle = OBSTACLE_GAUCHE_FI;
else if(robotState.distanceTelemetreDroit < 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN < 15) // 4-5
positionObstacle = OBSTACLE_DROITE_FN;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN < 15) // 5
positionObstacle = OBSTACLE_FN;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI < 15 &&
robotState.distanceTelemetreFN > 15) // 1
positionObstacle = OBSTACLE_FI;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI < 15 &&
robotState.distanceTelemetreFN < 15) // 1-5
positionObstacle = OBSTACLE_CRENEAU;
else if(robotState.distanceTelemetreDroit < 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI < 15 &&
robotState.distanceTelemetreFN > 15) // 1-4
positionObstacle = OBSTACLE_CRENEAU;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre < 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI < 15 &&
robotState.distanceTelemetreFN > 15) // 1-3
positionObstacle = OBSTACLE_EN_FACE_DROITE;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche < 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN < 15) // 2-5
positionObstacle = OBSTACLE_CRENEAU;
else if(robotState.distanceTelemetreDroit < 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche < 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) // 2-4
positionObstacle = OBSTACLE_CRENEAU;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre < 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN < 15) // 3-5
positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
else if(robotState.distanceTelemetreDroit > 25 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 25 &&
robotState.distanceTelemetreFI > 15 &&
robotState.distanceTelemetreFN > 15) //pas d?obstacle
positionObstacle = PAS_D_OBSTACLE;
else positionObstacle = PAS_D_OBSTACLE;

//Détermination de l?état à venir du robot
if (positionObstacle == OBSTACLE_A_DROITE)
nextStateRobot = STATE_TOURNE_GAUCHE;
else if (positionObstacle == OBSTACLE_A_GAUCHE)
nextStateRobot = STATE_TOURNE_DROITE;
else if (positionObstacle == OBSTACLE_EN_FACE)
nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
else if (positionObstacle == OBSTACLE_EN_FACE_GAUCHE)
nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
else if (positionObstacle == OBSTACLE_EN_FACE_DROITE)
nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
else if (positionObstacle == OBSTACLE_GAUCHE_FI)
nextStateRobot = STATE_TOURNE_DROITE;
else if (positionObstacle == OBSTACLE_DROITE_FN)
nextStateRobot = STATE_TOURNE_GAUCHE;
else if (positionObstacle == OBSTACLE_FN)
nextStateRobot = STATE_TOURNE_GAUCHE_FI;
else if (positionObstacle == OBSTACLE_FI)
nextStateRobot = STATE_TOURNE_DROITE_FN;
else if (positionObstacle == OBSTACLE_CRENEAU)
nextStateRobot = STATE_CRENEAU;

else if (positionObstacle == PAS_D_OBSTACLE)
nextStateRobot = STATE_AVANCE;
//else if (positionObstacle == OBSTACLE_EN_FACE_GAUCHE)
//nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;

//Si l'on n'est pas dans la transition de l'étape en cours
if (nextStateRobot != stateRobot - 1)
stateRobot = nextStateRobot;
}