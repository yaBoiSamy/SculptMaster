// // Library testing file

// #include "ConvenienceLib.h"

// Vect u(1, 2, 3);
// Vect v(4, 5, 6);
// Vect w;

// void setup() {
//   Serial.begin(115200);
//   delay(500);
// }

// void loop() {
//   Serial.println("==========================");
//   w.print();
//   Serial.println(u.str());
//   u[0] = 1;
//   u[1] = 2;
//   u[2] = 3;
//   v.x = 4;
//   v.y = 5;
//   v.z = 6;
//   Serial.println(u.x);
//   Serial.println(u.y);
//   Serial.println(u.z);
//   Serial.println(u.norm());
//   Serial.println(u.angle(v));
//   Serial.println(u.normalize().str());
//   Serial.println(u.dot(v));
//   Vect cross = u.cross(v);
//   cross.print();
//   Serial.println(cross.dot(u));
//   Serial.println(cross.dot(v));
//   (u+v).print();
//   u += v;
//   u.print();
//   (v*5).print();
//   v *= 5;
//   v.print();
//   delay(1000);
// }
