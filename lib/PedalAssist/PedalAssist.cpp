#include "PedalAssist.h"

//TODO LH turn this into a class with constants set on constructor

float calculateCadence (long edgeInterval, int cadenceMagnets) {
  float cadence = 0.0;
  if (edgeInterval > 0)
  {
    //this calculation always gives multiples of 5.. think it is faulty
    //cadence = (1000 / edgeInterval) * (60 / 12); //should give rpm this is getting truncated somehow.. 

    cadence = 60000 / (edgeInterval * cadenceMagnets); 
  }
  else
  {
    cadence = 0.0;
  }
  return cadence;
}

float calculateThrottleDuty(float cadence, float targetDuty) {
  //TODO LH get implementation here
}
