#include "UnitsUtil.h"
#include <math.h>

double UnitsUtil::MILLI = 1e-3f;
double UnitsUtil::MICRO = 1e-6f;
double UnitsUtil::NANO = 1e-9f;
double UnitsUtil::PICO = 1e-12f;
double UnitsUtil::KILO = 1e3f;
double UnitsUtil::MEGA = 1e6f;
double UnitsUtil::GIGA = 1e9f;

UnitsUtil::UnitsUtil() {
}

QString UnitsUtil::toString(double f, const QString& unit) {
  QString str;
  if(f < 1 && f > -1) {
    QString prefix;
    if(fabs(f) < PICO) {
      return "0" + unit;
    } else if(fabs(f) < NANO) {
      prefix = "p";
      str.sprintf("%.3f", f / PICO);
    } else if(fabs(f) < MICRO) {
      prefix = "n";
      str.sprintf("%.3f", f / NANO);
    } else if(fabs(f) < MILLI) {
      prefix = QChar(0xB5);
      str.sprintf("%.3f", f / MICRO);
    } else if(fabs(f) < 1.0f) {
      prefix = "m";
      str.sprintf("%.3f", f / MILLI);
    } else {
      prefix = "";
      str.sprintf("%.3f", f);
    }
    while(str.endsWith("0")) {
      str = str.remove(str.length() - 1, 1);
    }
    if(str.endsWith(".")) {
      str = str.remove(str.length() - 1, 1);
    }
    str += prefix;
  } else {
    if(fabs(f) > GIGA) {
      str.sprintf("%.3fk", f / GIGA);
    } else if(fabs(f) > MEGA) {
      str.sprintf("%.3fk", f / MEGA);
    } else if(fabs(f) > KILO) {
      str.sprintf("%.3fk", f / KILO);
    } else {
      str.sprintf("%.3f", f);
    }
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

