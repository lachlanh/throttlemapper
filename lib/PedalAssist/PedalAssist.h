#ifndef PEDALASSIST_H
#define PEDALASSIST_H

float calculateCadence (long edgeInterval, int cadenceMagnets);
float calculateThrottleDuty(float cadence, float targetDuty);

#endif
