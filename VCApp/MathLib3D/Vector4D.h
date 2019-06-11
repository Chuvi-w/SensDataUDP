/*
   This file is part of mathlib3d-cpp.
   mathlib3d-cpp is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   mathlib3d-cpp is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with j2dcg; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * MATHLIB4D - 4D Math Library
 * C++ v1.0
 * http://mathlib3d.sf.net
 *
 * Name:	Vector4D
 * Author:	Colin Barré-Brisebois
 * Created: 2004-07-02
 * Version: $Revision: 1.5 $
 */

#ifndef __MATHLIB3D_VECTOR4D__
#define __MATHLIB3D_VECTOR4D__

#include "MathLib3D.h"
using namespace MATHLIB3D;

namespace MATHLIB3D
{

class Vector4D
{
 public:
   /* Constructors */
   /**
    * Default constructor of the Vector4D class
    * W value is 1 by default.
    */
   Vector4D() : x(0), y(0), z(0), w(1) {}

   /**
    * Default constructor of the Vector4D class
    * @param x x coordinate value
    * @param y y coordinate value
    * @param z z coordinate value
    * @param w w coordinate value
    */
   Vector4D(const double& x, const double& y, const double& z, const double& w) : x(x), y(y), z(z), w(w) {}

   /**
    * Copy constructor of the Vector4D class
    * @param vector vector to copy
    */
   Vector4D(const Vector4D* pVector) : x(pVector->x), y(pVector->y), z(pVector->z), w(pVector->w) {}

   /**
    * Copy constructor of the Vector4D class
    * @param vector vector to copy
    */
   Vector4D(const Vector4D& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}

   /* Get Methods - Original Vector Not Modified */
   inline const double GetMagnitude() const;
   inline const double GetDot(const Vector4D& vector) const;
   inline const double GetAngle(const Vector4D& vector) const;
   inline Vector4D     GetNormalized() const;
   inline Vector4D     GetInverted() const;
   inline Vector4D     GetHomogenized() const;
   inline Vector4D     GetProjected(const Vector4D& vector) const;



   /* Set Methods - Original Vector Modified */
   inline Vector4D& SetMagnitude(const double& magnitude);
   inline Vector4D& SetNormalized();
   inline Vector4D& SetInverted();
   inline Vector4D& SetHomogenized();
   inline Vector4D& SetProjected(const Vector4D& vector);
   inline Vector4D& SetProjected(const Vector4D& vector1, const Vector4D& vector2);

   /* Operator replacements */
   inline Vector4D  GetAdd(const Vector4D& vector) const;
   inline Vector4D& SetAdd(const Vector4D& vector);
   inline Vector4D& SetAdd(const Vector4D& vector1, const Vector4D& vector2);

   inline Vector4D  GetSub(const Vector4D& vector) const;
   inline Vector4D& SetSub(const Vector4D& vector);
   inline Vector4D& SetSub(const Vector4D& vector1, const Vector4D& vector2);

   inline Vector4D  GetMul(const double& scalar) const;
   inline Vector4D& SetMul(const double& scalar);
   inline Vector4D& SetMul(const Vector4D& vector, const double& scalar);

   inline Vector4D  GetDiv(const double& scalar) const;
   inline Vector4D& SetDiv(const double& scalar);
   inline Vector4D& SetDiv(const Vector4D& vector, const double& scalar);

   inline const bool equals(const Vector4D& vector) const;
   inline const bool notEquals(const Vector4D& vector) const;

   /* Operators */
   inline Vector4D   operator+(const Vector4D& vector) const;
   inline Vector4D   operator-(const Vector4D& vector) const;
   inline Vector4D   operator*(const double& scalar) const;
   inline Vector4D   operator/(const double& scalar) const;
   inline Vector4D&  operator+=(const Vector4D& vector);
   inline Vector4D&  operator-=(const Vector4D& vector);
   inline Vector4D&  operator*=(const double& scalar);
   inline Vector4D&  operator/=(const double& scalar);
   inline Vector4D   operator-() const;
   inline const bool operator==(Vector4D& vector) const;
   inline const bool operator!=(Vector4D& vector);

 public:
   double x;
   double y;
   double z;
   double w;
};
} // namespace MATHLIB3D

//-------------------------------------------------------------------------------------------------
// INLINED SOURCE
//-------------------------------------------------------------------------------------------------

/**
 * Magnitude of the vector (4D)
 * @return magnitude of the vector
 */
inline const double Vector4D::GetMagnitude() const { return static_cast<double>(sqrt(x * x + y * y + z * z + w * w)); }

/**
 * Dot product of two vectors (4D)
 * @param vector second vector on which the dot product is applied
 * @return dot product value
 */
inline const double Vector4D::GetDot(const Vector4D& vector) const { return (x * vector.x + y * vector.y + z * vector.z + w * vector.w); }

/**
 * Angle between two vectors (4D)
 * @param vector second vector of the pair, where the angle is calculated
 * @return angle between the two vectors
 */
inline const double Vector4D::GetAngle(const Vector4D& vector) const
{
   const double magnitudesProduct = GetMagnitude() * vector.GetMagnitude();

   if(magnitudesProduct != 0.0f)
      return static_cast<double>(acos(GetDot(vector) / magnitudesProduct));
   else
      return 0.0f;
}

/**
 * Normalizes an INSTANCE of the vector (4D)
 * @return reference to a new normalized Vector4D
 */
inline Vector4D Vector4D::GetNormalized() const
{
   const double magnitude = GetMagnitude();

   if(magnitude != 0.0f)
      return Vector4D(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
   else
      return (*this);
}

/**
 * Inverts the vector
 * @return reference to THIS inverted Vector4D
 */
inline Vector4D Vector4D::GetInverted() const { return Vector4D(-x, -y, -z, -w); }

/**
 * Homogenizes the vector (4D)
 * @return reference to THIS homogenized Vector4D
 */
inline Vector4D Vector4D::GetHomogenized() const
{
   if(w != 0.0f)
      return (Vector4D(x / w, y / w, z / w, 1.0f));
   else
      return (*this);
}

/**
 * Projects a instance of the vector onto another (4D)
 * @param vector vector on which the projection is done
 * @return reference to a new normalized Vector4D
 */
inline Vector4D Vector4D::GetProjected(const Vector4D& vector) const
{
   const double selfDot = vector.GetDot(vector);

   if(selfDot != 0.0f)
      return (vector * (GetDot(vector) / (selfDot)));
   else
      return (*this);
}

/**
 * Changes the magnitude of the vector
 * @param new magnitude value
 * @return new vector
 */
inline Vector4D& Vector4D::SetMagnitude(const double& magnitude)
{
   const double currentMagnitude = GetMagnitude();

   if(currentMagnitude != 0.0f)
   {
      const double magnitudeRatio = magnitude / currentMagnitude; // Faster, but not nice.
      x *= magnitudeRatio;
      y *= magnitudeRatio;
      z *= magnitudeRatio;
      w *= magnitudeRatio;
   }

   return (*this);
}

/**
 * Normalizes the vector (4D)
 * @return reference to THIS normalized Vector4D
 */
inline Vector4D& Vector4D::SetNormalized()
{
   const double magnitude = GetMagnitude();

   if(magnitude != 0.0f)
   {
      x /= magnitude;
      y /= magnitude;
      z /= magnitude;
      w /= magnitude;
   }

   return (*this);
}

/**
 * Homogenizes the vector (4D)
 * @return reference to THIS homogenized Vector4D
 */
inline Vector4D& Vector4D::SetHomogenized()
{
   if(w != 0.0f)
   {
      x /= w;
      y /= w;
      z /= w;
      w /= 1.0f;
   }

   return (*this);
}

/**
 * Inverts the vector, but keeps the W parameter unchanged (4D)
 * @return reference to THIS inverted Vector4D
 */
inline Vector4D& Vector4D::SetInverted()
{
   x = -x;
   y = -y;
   z = -z;
   return (*this);
}

/**
 * Projects the vector onto another (4D)
 * @param vector vector on which the projection is done
 * @return reference to THIS projected Vector4D
 */
inline Vector4D& Vector4D::SetProjected(const Vector4D& vector)
{
   *this = GetProjected(vector);
   return (*this);
}

/**
 * ADD operator replacement
 * @param vector vector to sum
 * @return summation result
 */
inline Vector4D Vector4D::GetAdd(const Vector4D& vector) const { return Vector4D(this->x + vector.x, this->y + vector.y, this->z + vector.z, this->w + vector.w); }

/**
 * ADD_EQUAL operator replacement
 * @param vector vector to add to the current vector
 * @return summation result
 */
inline Vector4D& Vector4D::SetAdd(const Vector4D& vector)
{
   this->x += vector.x;
   this->y += vector.y;
   this->z += vector.z;
   this->w += vector.w;

   return (*this);
}

/**
 * ADD_EQUAL operator replacement
 * @param vector1 first vector to add
 * @param vector2 second vector to add
 * @return summation result
 */
inline Vector4D& Vector4D::SetAdd(const Vector4D& vector1, const Vector4D& vector2)
{
   this->x = vector1.x + vector2.x;
   this->y = vector1.y + vector2.y;
   this->z = vector1.z + vector2.z;
   this->w = vector1.w + vector2.w;

   return (*this);
}

/**
 * SUB operator replacement
 * @param vector vector to sub
 * @return substraction result
 */
inline Vector4D Vector4D::GetSub(const Vector4D& vector) const { return Vector4D(this->x - vector.x, this->y - vector.y, this->z - vector.z, this->w - vector.w); }

/**
 * SUB_EQUAL operator replacement
 * @param vector vector to sub to the current vector
 * @return substraction result
 */
inline Vector4D& Vector4D::SetSub(const Vector4D& vector)
{
   this->x -= vector.x;
   this->y -= vector.y;
   this->z -= vector.z;
   this->w -= vector.w;

   return (*this);
}

/**
 * SUB_EQUAL operator replacement
 * @param vector1 first vector to sub
 * @param vector2 second vector to sub
 * @return substraction result
 */
inline Vector4D& Vector4D::SetSub(const Vector4D& vector1, const Vector4D& vector2)
{
   this->x = vector1.x - vector2.x;
   this->y = vector1.y - vector2.y;
   this->z = vector1.z - vector2.z;
   this->w = vector1.w - vector2.w;

   return (*this);
}

/**
 * MULTIPLY operator replacement
 * @param scalar scalar value multiplied by the vector
 * @return scaled vector
 */
inline Vector4D Vector4D::GetMul(const double& scalar) const { return Vector4D(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar); }

/**
 * MULTIPLY_EQUAL operator replacement
 * @param scalar scalar multiplied by the current vector
 * @return scaled vector
 */
inline Vector4D& Vector4D::SetMul(const double& scalar)
{
   this->x *= scalar;
   this->y *= scalar;
   this->z *= scalar;
   this->w *= scalar;

   return (*this);
}

/**
 * MULTIPLY_EQUAL operator replacement
 * @param vector1 first vector to multiply
 * @param scalar scalar to multiply to the first vector
 * @return scaled vector
 */
inline Vector4D& Vector4D::SetMul(const Vector4D& vector, const double& scalar)
{
   this->x = vector.x * scalar;
   this->y = vector.y * scalar;
   this->z = vector.z * scalar;
   this->w = vector.w * scalar;

   return (*this);
}

/**
 * DIVIDE operator replacement
 * @param scalar scalar value that divides the vector
 * @return scaled vector
 */
inline Vector4D Vector4D::GetDiv(const double& scalar) const { return Vector4D(this->x / scalar, this->y / scalar, this->z / scalar, this->w / scalar); }

/**
 * DIVIDE_EQUAL operator replacement
 * @param scalar scalar multiplied by the current vector
 * @return scaled vector
 */
inline Vector4D& Vector4D::SetDiv(const double& scalar)
{
   this->x /= scalar;
   this->y /= scalar;
   this->z /= scalar;
   this->w /= scalar;

   return (*this);
}

/**
 * MULTIPLY_EQUAL operator replacement
 * @param vector1 first vector to multiply
 * @param scalar scalar to multiply to the first vector
 * @return scaled vector
 */
inline Vector4D& Vector4D::SetDiv(const Vector4D& vector, const double& scalar)
{
   this->x = vector.x / scalar;
   this->y = vector.y / scalar;
   this->z = vector.z / scalar;
   this->w = vector.w / scalar;

   return (*this);
}

/**
 * EQUAL_EQUAL operator
 * @param vector vector to compare
 * @return equal value
 */
inline const bool Vector4D::equals(const Vector4D& vector) const { return ((this->x == vector.x) && (this->y == vector.y) && (this->z == vector.z) && (this->w == vector.w)); }

/**
 * NOT_EQUAL operator
 * @param vector vector to compare
 * @return not equal value
 */
inline const bool Vector4D::notEquals(const Vector4D& vector) const { return ((this->x != vector.x) && (this->y != vector.y) && (this->z != vector.z) && (this->w != vector.w)); }

/**
 * ADD operator
 * @param vector vector to sum
 * @return summation result
 */
inline Vector4D Vector4D::operator+(const Vector4D& vector) const { return Vector4D(x + vector.x, y + vector.y, z + vector.z, w + vector.w); }

/**
 * MINUS operator
 * @param vector vector to minus(sic)
 * @return minus result
 */
inline Vector4D Vector4D::operator-(const Vector4D& vector) const { return Vector4D(x - vector.x, y - vector.y, z - vector.z, w - vector.w); }

/**
 * MULTIPLY operator
 * @param scalar scalar value divided by the vector
 * @return scaled vector
 */
inline Vector4D Vector4D::operator*(const double& scalar) const { return Vector4D(x * scalar, y * scalar, z * scalar, w * scalar); }

/**
 * DIVIDE operator
 * @param scalar scalar value divided by the vector
 * @return scaled vector
 */
inline Vector4D Vector4D::operator/(const double& scalar) const
{
   if(scalar != 0.0f)
      return Vector4D(x / scalar, y / scalar, z / scalar, w / scalar);
   else
      return (*this);
}

/**
 * ADD_EQUAL operator
 * @param vector vector to add to the current vector
 * @return summation result
 */
inline Vector4D& Vector4D::operator+=(const Vector4D& vector)
{
   x += vector.x;
   y += vector.y;
   z += vector.z;
   w += vector.w;
   return (*this);
}

/**
 * MINUS_EQUAL operator
 * @param vector vector to minus(sic) to the current vector
 * @return minus result
 */
inline Vector4D& Vector4D::operator-=(const Vector4D& vector)
{
   x -= vector.x;
   y -= vector.y;
   z -= vector.z;
   w -= vector.w;
   return (*this);
}

/**
 * MULTIPLY_EQUAL operator
 * @param scalar scalar multiplied by the current vector
 * @return scaled result
 */
inline Vector4D& Vector4D::operator*=(const double& scalar)
{
   x *= scalar;
   y *= scalar;
   z *= scalar;
   w *= scalar;
   return (*this);
}

/**
 * DIVIDE_EQUAL operator
 * @param scalar scalar divided by the current vector
 * @return scaled result
 */
inline Vector4D& Vector4D::operator/=(const double& scalar)
{
   if(scalar != 0.0f)
   {
      x /= scalar;
      y /= scalar;
      z /= scalar;
      w /= scalar;
   }

   return (*this);
}

/**
 * SIGN operator, inverts all but new instance keeps W the same
 * @return signed result, but W stays the same
 */
inline Vector4D Vector4D::operator-() const { return Vector4D(-x, -y, -z, -w); }

/**
 * EQUAL_EQUAL operator
 * @param vector vector to compare
 * @return equal value
 */
inline const bool Vector4D::operator==(Vector4D& vector) const { return ((x == vector.x) && (y == vector.y) && (z == vector.z) && (w == vector.w)) ? true : false; }

/**
 * NOT_EQUAL operator
 * @param vector vector to compare
 * @return not equal value
 */
inline const bool Vector4D::operator!=(Vector4D& vector) { return ((x == vector.x) && (y == vector.y) && (z == vector.z) && (w == vector.w)) ? true : false; }

/**
 * Global MULTIPLY operator
 * @param scalar scalar value to multiply with the second parameter
 * @param vector vector to multiply the scalar value with
 * @return scaled vector instance
 */
inline Vector4D operator*(double scalar, Vector4D& vector) { return Vector4D(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar); }

/**
 * Global DIVIDE operator
 * @param scalar scalar value to divide with the second parameter
 * @param vector vector to divide the scalar value with
 * @return scaled vector instance
 */
inline Vector4D operator/(double scalar, Vector4D& vector) { return Vector4D(vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w / scalar); }

#endif