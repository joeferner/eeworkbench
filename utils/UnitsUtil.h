#ifndef UNITSUTIL_H
#define UNITSUTIL_H

#include <QString>

class UnitsUtil
{
private:
  UnitsUtil();

public:
  static QString toString(float f, const QString& unit);
  static float roundToOrderOfMagnitude(float f);

  static float MILLI;
  static float MICRO;
  static float NANO;
  static float PICO;
};

#endif // UNITSUTIL_H
