#include "UnitsUtil.h"
#include <math.h>

float UnitsUtil::MILLI = 1e-3f;
float UnitsUtil::MICRO = 1e-6f;
float UnitsUtil::NANO = 1e-9f;
float UnitsUtil::PICO = 1e-12f;

UnitsUtil::UnitsUtil()
{
}

QString UnitsUtil::toString(float f, const QString& unit) {
  if(fabs(f) < PICO) {
    return QString("0") + unit;
  }
  if(fabs(f) < NANO) {
    return QString::number(f / PICO, 'g', 3) + "p" + unit;
  }
  if(fabs(f) < MICRO) {
    return QString::number(f / NANO, 'g', 3) + "n" + unit;
  }
  if(fabs(f) < MILLI) {
    return QString::number(f / MICRO, 'g', 3) + "u" + unit;
  }
  if(fabs(f) < 1.0f) {
    return QString::number(f / MILLI, 'g', 3) + "m" + unit;
  }
  return QString::number(f, 'g', 3) + unit;
}

float UnitsUtil::roundToOrderOfMagnitude(float f) {
  if(f >= 1) {
    return pow(10, ceil(log10(f)));
  } else {
    return pow(10, floor(log10(f)));
  }
}

