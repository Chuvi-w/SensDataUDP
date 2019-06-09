void MahonyAHRSupdate(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz)
{
   double recipNorm;
   double q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
   double hx, hy, bx, bz;
   double halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
   double halfex, halfey, halfez;
   double qa, qb, qc;

   // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
   if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
   {
      MahonyAHRSupdateIMU(gx, gy, gz, ax, ay, az);
      return;
   }

   // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
   if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
   {

      // Normalise accelerometer measurement
      recipNorm = invSqrt(ax * ax + ay * ay + az * az);
      ax *= recipNorm;
      ay *= recipNorm;
      az *= recipNorm;

      // Normalise magnetometer measurement
      recipNorm = invSqrt(mx * mx + my * my + mz * mz);
      mx *= recipNorm;
      my *= recipNorm;
      mz *= recipNorm;

      // Auxiliary variables to avoid repeated arithmetic
      q0q0 = Mah_q0 * Mah_q0;
      q0q1 = Mah_q0 * Mah_q1;
      q0q2 = Mah_q0 * Mah_q2;
      q0q3 = Mah_q0 * Mah_q3;
      q1q1 = Mah_q1 * Mah_q1;
      q1q2 = Mah_q1 * Mah_q2;
      q1q3 = Mah_q1 * Mah_q3;
      q2q2 = Mah_q2 * Mah_q2;
      q2q3 = Mah_q2 * Mah_q3;
      q3q3 = Mah_q3 * Mah_q3;

      // Reference direction of Earth's magnetic field
      hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
      hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
      bx = sqrt(hx * hx + hy * hy);
      bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));

      // Estimated direction of gravity and magnetic field
      halfvx = q1q3 - q0q2;
      halfvy = q0q1 + q2q3;
      halfvz = q0q0 - 0.5f + q3q3;
      halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
      halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
      halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

      // Error is sum of cross product between estimated direction and measured direction of field vectors
      halfex = (ay * halfvz - az * halfvy) + (my * halfwz - mz * halfwy);
      halfey = (az * halfvx - ax * halfvz) + (mz * halfwx - mx * halfwz);
      halfez = (ax * halfvy - ay * halfvx) + (mx * halfwy - my * halfwx);

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