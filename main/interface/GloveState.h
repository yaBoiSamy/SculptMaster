#ifndef GLOVE_STATE_H
#define GLOVE_STATE_H

#include <stdbool.h>
#include "vector3.h"

typedef struct
{
    Vector3 acc;
    Vector3 angularVelocity;

    bool touches[4];
} GloveState;

#endif