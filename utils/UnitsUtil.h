#ifndef UNITSUTIL_H
#define UNITSUTIL_H

#include <QString>

class UnitsUtil {
private:
  UnitsUtil();

public:
  static QString toString(double f, const QString& unit);
  static double roundToOrderOfMagnitude(double f);

  static double MILLI;
  static double MICRO;
  static double NANO;
  static double PICO;
  static double KILO;
  static double MEGA;
  static double GIGA;
};

#endif // UNITSUTIL_H
