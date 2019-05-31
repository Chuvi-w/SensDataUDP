#pragma once
#include "MathLib/MathLib3D.h"

#define MATH_PI (MATHLIB3D::MathUtils::PI)
using Vec3D           = MATHLIB3D::Vector3D;
const Vec3D NullVec3D = Vec3D(0.0, 0.0, 0.0);
using Vec2            = MATHLIB3D::Vector2D;

inline Vec3D DEG2RAD(Vec3D val) { return val * MATH_PI / 180.0; }

inline Vec3D RAD2DEG(Vec3D val) { return val * 180.0 / MATH_PI; }

inline double DEG2RAD(double val) { return val * MATH_PI / 180.0; }
inline double RAD2DEG(double val) { return val * 180.0 / MATH_PI; }
