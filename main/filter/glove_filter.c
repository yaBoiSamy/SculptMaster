#include "glove_filter.h"

static float clampf(float v, float min, float max)
{
    if (v < min) return min;
    if (v > max) return max;
    return v;
}
//Fonction utiles pour la manipulation de vecteurs
static Vector3 vec_sub(Vector3 a, Vector3 b)
{
    Vector3 r = {a.x - b.x, a.y - b.y, a.z - b.z};
    return r;
}

static Vector3 vec_add(Vector3 a, Vector3 b)
{
    Vector3 r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return r;
}

static Vector3 vec_scale(Vector3 v, float s)
{
    Vector3 r = {v.x * s, v.y * s, v.z * s};
    return r;
}

static float absf(float v)
{
    return v < 0 ? -v : v;
}

void GloveFilter_init(GloveFilter* f, float alpha, float deadband, float maxStep)
{
    f->last = (Vector3){0,0,0};
    f->velocity = (Vector3){0,0,0};

    f->alpha = alpha;
    f->deadband = deadband;
    f->maxStep = maxStep;
}

Vector3 GloveFilter_update(GloveFilter* f, Vector3 input, float dt)
{
    Vector3 delta = vec_sub(input, f->last);

    // ---------------------------
    // 1. DEAD BAND (anti jitter)
    // ---------------------------
    if (absf(delta.x) < f->deadband) delta.x = 0;
    if (absf(delta.y) < f->deadband) delta.y = 0;
    if (absf(delta.z) < f->deadband) delta.z = 0;

    Vector3 target = vec_add(f->last, delta);

    // ---------------------------
    // 2. CLAMP (anti spike)
    // ---------------------------
    Vector3 step = vec_sub(target, f->last);

    step.x = clampf(step.x, -f->maxStep, f->maxStep);
    step.y = clampf(step.y, -f->maxStep, f->maxStep);
    step.z = clampf(step.z, -f->maxStep, f->maxStep);

    Vector3 clamped = vec_add(f->last, step);

    // ---------------------------
    // 3. LOW PASS FILTER
    // ---------------------------
    Vector3 output;
    output.x = f->last.x + f->alpha * (clamped.x - f->last.x);
    output.y = f->last.y + f->alpha * (clamped.y - f->last.y);
    output.z = f->last.z + f->alpha * (clamped.z - f->last.z);

    // ---------------------------
    // 4. VELOCITY (optional tracking)
    // ---------------------------
    f->velocity = vec_scale(vec_sub(output, f->last), 1.0f / (dt + 0.0001f));

    f->last = output;

    return output;
}
