#include "vectorMath.hpp"
#include <cmath>

void vectorMath::normalizeVector2D(float* x, float* y) {
    // Calculate vector module
    float mod = std::sqrt((*x)*(*x) + (*y)*(*y));

    // Calculate unit vector
    *x /= mod;
    *y /= mod;

    return;
}

void vectorMath::normalizeVector2D(double* x, double* y) {
    // Calculate vector module
    double mod = std::sqrt((*x)*(*x) + (*y)*(*y));

    // Calculate unit vector
    *x /= mod;
    *y /= mod;

    return;
}

void vectorMath::normalizeVector3D(float* x, float* y, float* z) {
    // Calculate vector module
    float mod = std::sqrt((*x)*(*x) + (*y)*(*y) + (*z)*(*z));

    // Calculate unit vector
    *x /= mod;
    *y /= mod;
    *z /= mod;

    return;
}

void vectorMath::normalizeVector3D(double* x, double* y, double* z) {
    // Calculate vector module
    double mod = std::sqrt((*x)*(*x) + (*y)*(*y) + (*z)*(*z));

    // Calculate unit vector
    *x /= mod;
    *y /= mod;
    *z /= mod;

    return;
}

void vectorMath::getPerpendicularVector(const float x, const float y, float* px, float* py) {
    *px = y;
    *py = -x;
}

void vectorMath::getPerpendicularVector(const double x, const double y, double* px, double* py) {
    *px = y;
    *py = -x;
}