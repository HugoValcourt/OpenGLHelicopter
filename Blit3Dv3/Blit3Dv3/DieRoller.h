#pragma once
#include <random> //need for random number generation

extern std::mt19937 rng; //keep one global instance of your random device (per thread)

void Seed();

float RollDNFloat(float min, float max);
int RollDNInt(int min, int max);