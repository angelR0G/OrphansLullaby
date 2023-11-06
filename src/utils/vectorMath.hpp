#pragma once

#define PI      3.1415927

struct vectorMath {
    static void normalizeVector2D(float*, float*);
    static void normalizeVector2D(double*, double*);
    static void normalizeVector3D(float*, float*, float*);
    static void normalizeVector3D(double*, double*, double*);
    static void getPerpendicularVector(const float, const float, float*, float*);
    static void getPerpendicularVector(const double, const double, double*, double*);
};