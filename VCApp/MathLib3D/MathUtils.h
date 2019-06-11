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
 * MATHLIB3D - 3D Math Library
 * C++ v1.0
 * http://mathlib3d.sf.net
 *
 * Name:	MathUtils
 * Author:	Colin Barré-Brisebois
 * Created: 2004-07-02
 * Version: $Revision: 1.6 $
 */

#ifndef __MATHLIB3D_MATHUTILS__
#define __MATHLIB3D_MATHUTILS__
#pragma once

#include "MathLib3D.h"
#include <vector>
using namespace MATHLIB3D;

namespace MATHLIB3D
{

class MathUtils
{
 public:
   static const double PI;
   static const double PI_DOUBLE;
   static const double PI_HALF;
   static const double PI_SQUARE;
   static const double E;
   static const double RAD_PER_DEG;
   static const double DEG_PER_RAD;

   static inline double Cos(const double& value);
   static inline double Sin(const double& value);
   static inline double Tan(const double& value);
   static inline double ArcCos(const double& value);
   static inline double ArcSin(const double& value);
   static inline double ArcTan(const double& value);
   static inline double DegToRad(const double& value);
   static inline double RadToDeg(const double& value);
   static inline double Abs(const double& value);
   static inline int    Floor(const double& value);
   static inline int    Ceiling(const double& value);
   static inline int    Round(const double& value);
   static inline double FloatRound1000(const double& value);
   static inline int    Sign(const double& value);
   static inline double Min(const double& value1, const double& value2);
   static inline double Max(const double& value1, const double& value2);
   static inline void   Swap(int& value1, int& value2);
   static inline bool   Odd(const int& value);
};

template <typename ML_VECTOR_T> ML_VECTOR_T GetVAvg(const std::vector<ML_VECTOR_T>& vData)
{
   ML_VECTOR_T VecAvg;
   if(!vData.empty())
   {
      for(const auto& v : vData)
      {
         VecAvg += v;
      }
      VecAvg /= (double)(vData.size());
   }

   return VecAvg;
}

template <typename ML_VECTOR_T> ML_VECTOR_T GetVMedian(const std::vector<ML_VECTOR_T>& vData)
{
   ML_VECTOR_T Median, AvgPoint;
   double      CurDist = 0.0, MinDist = (double)0xFFFFFFFF;
   int         MidID = -1;
   if(!vData.empty())
   {
      AvgPoint = GetVAvg(vData);
      for(int i = 0; i < vData.size(); i++)
      {
         CurDist = vData[i].GetDistance(AvgPoint);
         if(CurDist < MinDist)
         {
            MinDist = CurDist;
            MidID   = i;
         }
      }
      if(MidID != -1)
      {
         Median = vData[MidID];
      }
   }
   return Median;
}
} // namespace MATHLIB3D

//-------------------------------------------------------------------------------------------------
// INLINED SOURCE
//-------------------------------------------------------------------------------------------------

/**
 * Calculates the cos of a value
 * @param value value to calculate from
 * @return calculated cos value
 * @todo numerical approximation
 */
inline double MathUtils::Cos(const double& value) { return static_cast<double>(cos(value)); }

/**
 * Calculates the sin of a value
 * @param value value to calculate from
 * @return calculated sin value
 * @todo numerical approximation
 */
inline double MathUtils::Sin(const double& value) { return static_cast<double>(sin(value)); }

/**
 * Calculates the tan of a value
 * @param value value to calculate from
 * @return calculated tan value
 * @todo numerical approximation
 */
inline double MathUtils::Tan(const double& value) { return static_cast<double>(tan(value)); }

/**
 * Calculates the arccos of a value
 * @param value value to calculate from
 * @return calculated arccos value
 * @todo numerical approximation
 */
inline double MathUtils::ArcCos(const double& value) { return static_cast<double>(acos(value)); }

/**
 * Calculates the arcsin of a value
 * @param value value to calculate from
 * @return calculated arcsin value
 * @todo numerical approximation
 */
inline double MathUtils::ArcSin(const double& value) { return static_cast<double>(asin(value)); }

/**
 * Calculates the arctan of a value
 * @param value value to calculate from
 * @return calculated arctan value
 * @todo numerical approximation
 */
inline double MathUtils::ArcTan(const double& value) { return static_cast<double>(atan(value)); }

/**
 * Converts a degree value to a rad value
 * @param value value to convert
 * @return converted rad value
 */
inline double MathUtils::DegToRad(const double& value) { return (value * MathUtils::RAD_PER_DEG); }

/**
 * Converts a rad value to a degree value
 * @param value value to convert
 * @return converted deg value
 */
inline double MathUtils::RadToDeg(const double& value) { return (value * MathUtils::DEG_PER_RAD); }

/**
 * Calculates the absolute value
 * @param value value to calculate from
 * @return absolute value
 */
inline double MathUtils::Abs(const double& value) { return (((value) < 0) ? -(value) : (value)); }

/**
 * Calculates the floor value
 * @param value value to calculate from
 * @return floor value
 */
inline int MathUtils::Floor(const double& value) { return static_cast<int>(floor(value)); }

/**
 * Calculates the ceiling value
 * @param value value to calculate from
 * @return ceiling value
 */
inline int MathUtils::Ceiling(const double& value) { return static_cast<int>(ceilf(static_cast<float>(value))); }

/**
 * Calculates the round value
 * @param value value to calculate from
 * @return round value
 */
inline int MathUtils::Round(const double& value) { return ((value) > 0 ? static_cast<int>(value + 0.5f) : -static_cast<int>(0.5f - value)); }

inline double MathUtils::FloatRound1000(const double& value) { return static_cast<double>(Round(value * 10000) / 10000.0); }

/**
 * Calculates the signed value
 * @param value value to calculate from
 * @return signed value (-1 = signed, 0 = not signed)
 */
inline int MathUtils::Sign(const double& value) { return (((value) < 0) ? -1 : 1); }

/**
 * Calculates the minimum value between 2 values
 * @param value1 first value to calculate from
 * @param value2 second value to calculate from
 * @return minimum value
 */
inline double MathUtils::Min(const double& value1, const double& value2) { return (((value1) < (value2)) ? (value1) : (value2)); }

/**
 * Calculates the maximum value between 2 values
 * @param value1 first value to calculate from
 * @param value2 second value to calculate from
 * @return maximum value
 */
inline double MathUtils::Max(const double& value1, const double& value2) { return (((value1) > (value2)) ? (value1) : (value2)); }

/**
 * Swaps two int values rapidly using masks
 * @param value1 first value to switch
 * @param value2 second value to switch
 */
inline void MathUtils::Swap(int& value1, int& value2)
{
   value1 ^= value2;
   value2 ^= value1;
   value1 ^= value2;
}

/**
 * Identifies if a value is odd
 * @param value value to check if odd or not
 * @return odd (true) or not (false)
 */
inline bool MathUtils::Odd(const int& value) { return (((value)&1) ? true : false); }

#endif