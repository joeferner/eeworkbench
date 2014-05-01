#include "UnitsUtil.h"
#include <math.h>

double UnitsUtil::MILLI = 1e-3f;
double UnitsUtil::MICRO = 1e-6f;
double UnitsUtil::NANO = 1e-9f;
double UnitsUtil::PICO = 1e-12f;

UnitsUtil::UnitsUtil()
{
}

QString UnitsUtil::toString(double f, const QString& unit) {
  QString str;
  if(fabs(f) < PICO) {
    return "0" + unit;
  } else if(fabs(f) < NANO) {
    str.sprintf("%.2fp", f / PICO);
  } else if(fabs(f) < MICRO) {
    str.sprintf("%.2fn", f / NANO);
  } else if(fabs(f) < MILLI) {
    str.sprintf("%.2fu", f / MICRO);
  } else if(fabs(f) < 1.0f) {
    str.sprintf("%.2fm", f / MILLI);
  } else {
    str.sprintf("%.2f", f);
  }
  while(str.endsWith("0")) {
    str = str.remove(str.length() - 1, 1);
  }
  if(str.endsWith(".")) {
    str = str.remove(str.length() - 1, 1);
  }
  return str + unit;
}

double UnitsUtil::roundToOrderOfMagnitude(double f) {
  if(f >= 1) {
    return pow(10, ceil(log10(f)));
  } else {
    return pow(10, floor(log10(f)));
  }
}

