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

template <typename T> class CMinMax
{
 public:
   CMinMax() : m_bFirstVal(true), m_Count(0) {}
   //    void AddElement(const T &val)
   //    {
   //       if(m_bFirstVal)
   //       {
   //          m_Min = val;
   //          m_Max = val;
   //          m_bFirstVal = false;
   //       }
   //       else
   //       {
   //          if(val < m_Min)
   //          {
   //             m_Min = val;
   //          }
   //          if(m_Max < val)
   //          {
   //             m_Max = val;
   //          }
   //
   //       }
   //    }

   void AddElement(T val)
   {
      if(m_bFirstVal)
      {
         m_Min       = val;
         m_Max       = val;
         m_Sum       = val;
         m_Count     = 1;
         m_bFirstVal = false;
      }
      else
      {
         if(val < m_Min)
         {
            m_Min = val;
         }
         if(m_Max < val)
         {
            m_Max = val;
         }
         m_Sum += val;
         m_Count++;
      }
   }
   T      GetSum() const { return m_Sum; }
   size_t GetCount() const { return m_Count; }

   T GetMin() const { return m_Min; }
   T GetMax() const { return m_Max; }

   void Reset()
   {
      m_Count     = 0;
      m_bFirstVal = true;
   }

 private:
   T      m_Min;
   T      m_Max;
   T      m_Sum;
   size_t m_Count;
   bool   m_bFirstVal;
};