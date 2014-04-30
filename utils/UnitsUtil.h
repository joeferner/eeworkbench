#ifndef UNITSUTIL_H
#define UNITSUTIL_H

#include <QString>

class UnitsUtil
{
private:
  UnitsUtil();

public:
  static QString toString(double f, const QString& unit);
  static double roundToOrderOfMagnitude(double f);

  static double MILLI;
  static double MICRO;
  static double NANO;
  static double PICO;
};

#endif // UNITSUTIL_H
