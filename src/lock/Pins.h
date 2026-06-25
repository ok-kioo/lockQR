#pragma once

#include <Arduino.h>

const uint8_t PINOS_DISPONIVEIS[] = {
  18,
  19,
  21,
  22,
  23,
  25,
  27,
  32,
  33
};

const int TOTAL_PINOS =
  sizeof(PINOS_DISPONIVEIS) /
  sizeof(PINOS_DISPONIVEIS[0]);