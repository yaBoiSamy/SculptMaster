#ifndef GLOVE_FILTER_H
#define GLOVE_FILTER_H
#include "interface/Vector3.h"

typedef struct
{
    Vector3 last;
    Vector3 velocity;

    float alpha; //for smoothing
    float deadband; //for ignoring small jump
    float maxStep; // anti jump
} GloveFilter;

void GloveFilter_init(GloveFilter* f, float alpha, float deadband, float maxStep);
Vector3 GloveFilter_update(GloveFilter* f, Vector3 input, float dt);

#endif