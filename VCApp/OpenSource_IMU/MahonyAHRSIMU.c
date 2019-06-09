void MahonyAHRSupdateIMU(double gx, double gy, double gz, double ax, double ay, double az)
{
   double recipNorm;
   double halfvx, halfvy, halfvz;
   double halfex, halfey, halfez;
   double qa, qb, qc;

   // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
   if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
   {

      // Normalise accelerometer measurement
      recipNorm = invSqrt(ax * ax + ay * ay + az * az);
      ax *= recipNorm;
      ay *= recipNorm;
      az *= recipNorm;

      // Estimated direction of gravity and vector perpendicular to magnetic flux
      halfvx = Mah_q1 * Mah_q3 - Mah_q0 * Mah_q2;
      halfvy = Mah_q0 * Mah_q1 + Mah_q2 * Mah_q3;
      halfvz = Mah_q0 * Mah_q0 - 0.5f + Mah_q3 * Mah_q3;

      // Error is sum of cross product between estimated and measured direction of gravity
      halfex = (ay * halfvz - az * halfvy);
      halfey = (az * halfvx - ax * halfvz);
      halfez = (ax * halfvy - ay * halfvx);

      // Compute and apply integral feedback if enabled
      if(twoKi > 0.0f)
      {
         integralFBx += twoKi * halfex * (1.0f / sampleFreq); // integral error scaled by Ki
         integralFBy += twoKi * halfey * (1.0f / sampleFreq);
         integralFBz += twoKi * halfez * (1.0f / sampleFreq);
         gx += integralFBx; // apply integral feedback
         gy += integralFBy;
         gz += integralFBz;
      }
      else
      {
         integralFBx = 0.0f; // prevent integral windup
         integralFBy = 0.0f;
         integralFBz = 0.0f;
      }

      // Apply proportional feedback
      gx += twoKp * halfex;
      gy += twoKp * halfey;
      gz += twoKp * halfez;
   }

   // Integrate rate of change of quaternion
   gx *= (0.5f * (1.0f / sampleFreq)); // pre-multiply common factors
   gy *= (0.5f * (1.0f / sampleFreq));
   gz *= (0.5f * (1.0f / sampleFreq));
   qa = Mah_q0;
   qb = Mah_q1;
   qc = Mah_q2;
   Mah_q0 += (-qb * gx - qc * gy - Mah_q3 * gz);
   Mah_q1 += (qa * gx + qc * gz - Mah_q3 * gy);
   Mah_q2 += (qa * gy - qb * gz + Mah_q3 * gx);
   Mah_q3 += (qa * gz + qb * gy - qc * gx);

   // Normalise quaternion
   recipNorm = invSqrt(Mah_q0 * Mah_q0 + Mah_q1 * Mah_q1 + Mah_q2 * Mah_q2 + Mah_q3 * Mah_q3);
   Mah_q0 *= recipNorm;
   Mah_q1 *= recipNorm;
   Mah_q2 *= recipNorm;
   Mah_q3 *= recipNorm;
}