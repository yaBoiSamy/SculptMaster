#ifndef Vector_h
#define Vector_h

#include "Arduino.h"

class Vect {
  public:
    // Components
    double x; double y; double z;

    // Constructor
    Vect(double _x = 0, double _y = 0, double _z = 0);

    // Other methods
    double dot(const Vect& other) const;
    Vect cross(const Vect& other) const;
    double norm() const;
    double angle(const Vect& other) const;
    Vect normalize() const;
    String str() const;

    // Operator overloads
    double& operator[](int index);
    const double& operator[](int index) const;
    Vect operator+(const Vect& other) const;
    Vect operator-(const Vect& other) const;
    Vect operator*(double other) const;
    Vect& operator+=(const Vect& other);
    Vect& operator-=(const Vect& other);
    Vect& operator*=(double other);
    void print() const;
};

#endif