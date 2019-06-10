void MadgwickAHRSupdate_Fixed(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz)
{
   double recipNorm;
   double s0, s1, s2, s3;
   double qDot1, qDot2, qDot3, qDot4;
   double hx, hy;
   double _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _8bx, _8bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

   // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
   if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
   {
      MadgwickAHRSupdateIMU_Fixed(gx, gy, gz, ax, ay, az);
      return;
   }

   // Rate of change of quaternion from gyroscope
   qDot1 = 0.5f * (-Madg_Fix_q1 * gx - Madg_Fix_q2 * gy - Madg_Fix_q3 * gz);
   qDot2 = 0.5f * (Madg_Fix_q0 * gx + Madg_Fix_q2 * gz - Madg_Fix_q3 * gy);
   qDot3 = 0.5f * (Madg_Fix_q0 * gy - Madg_Fix_q1 * gz + Madg_Fix_q3 * gx);
   qDot4 = 0.5f * (Madg_Fix_q0 * gz + Madg_Fix_q1 * gy - Madg_Fix_q2 * gx);

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
      _2q0mx = 2.0f * Madg_Fix_q0 * mx;
      _2q0my = 2.0f * Madg_Fix_q0 * my;
      _2q0mz = 2.0f * Madg_Fix_q0 * mz;
      _2q1mx = 2.0f * Madg_Fix_q1 * mx;
      _2q0   = 2.0f * Madg_Fix_q0;
      _2q1   = 2.0f * Madg_Fix_q1;
      _2q2   = 2.0f * Madg_Fix_q2;
      _2q3   = 2.0f * Madg_Fix_q3;
      _2q0q2 = 2.0f * Madg_Fix_q0 * Madg_Fix_q2;
      _2q2q3 = 2.0f * Madg_Fix_q2 * Madg_Fix_q3;
      q0q0   = Madg_Fix_q0 * Madg_Fix_q0;
      q0q1   = Madg_Fix_q0 * Madg_Fix_q1;
      q0q2   = Madg_Fix_q0 * Madg_Fix_q2;
      q0q3   = Madg_Fix_q0 * Madg_Fix_q3;
      q1q1   = Madg_Fix_q1 * Madg_Fix_q1;
      q1q2   = Madg_Fix_q1 * Madg_Fix_q2;
      q1q3   = Madg_Fix_q1 * Madg_Fix_q3;
      q2q2   = Madg_Fix_q2 * Madg_Fix_q2;
      q2q3   = Madg_Fix_q2 * Madg_Fix_q3;
      q3q3   = Madg_Fix_q3 * Madg_Fix_q3;

      // Reference direction of Earth's magnetic field
      hx   = mx * q0q0 - _2q0my * Madg_Fix_q3 + _2q0mz * Madg_Fix_q2 + mx * q1q1 + _2q1 * my * Madg_Fix_q2 + _2q1 * mz * Madg_Fix_q3 - mx * q2q2 - mx * q3q3;
      hy   = _2q0mx * Madg_Fix_q3 + my * q0q0 - _2q0mz * Madg_Fix_q1 + _2q1mx * Madg_Fix_q2 - my * q1q1 + my * q2q2 + _2q2 * mz * Madg_Fix_q3 - my * q3q3;
      _2bx = sqrt(hx * hx + hy * hy);
      _2bz = -_2q0mx * Madg_Fix_q2 + _2q0my * Madg_Fix_q1 + mz * q0q0 + _2q1mx * Madg_Fix_q3 - mz * q1q1 + _2q2 * my * Madg_Fix_q3 - mz * q2q2 + mz * q3q3;
      _4bx = 2.0f * _2bx;
      _4bz = 2.0f * _2bz;
      _8bx = 2.0f * _4bx;
      _8bz = 2.0f * _4bz;

      // Gradient decent algorithm corrective step
      s0        = -_2q2 * (2.0f * (q1q3 - q0q2) - ax) + _2q1 * (2.0f * (q0q1 + q2q3) - ay) + -_4bz * Madg_Fix_q2 * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (-_4bx * Madg_Fix_q3 + _4bz * Madg_Fix_q1) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + _4bx * Madg_Fix_q2 * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
      s1        = _2q3 * (2.0f * (q1q3 - q0q2) - ax) + _2q0 * (2.0f * (q0q1 + q2q3) - ay) + -4.0f * Madg_Fix_q1 * (2.0f * (0.5 - q1q1 - q2q2) - az) + _4bz * Madg_Fix_q3 * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (_4bx * Madg_Fix_q2 + _4bz * Madg_Fix_q0) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * Madg_Fix_q3 - _8bz * Madg_Fix_q1) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
      s2        = -_2q0 * (2.0f * (q1q3 - q0q2) - ax) + _2q3 * (2.0f * (q0q1 + q2q3) - ay) + (-4.0f * Madg_Fix_q2) * (2.0f * (0.5 - q1q1 - q2q2) - az) + (-_8bx * Madg_Fix_q2 - _4bz * Madg_Fix_q0) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (_4bx * Madg_Fix_q1 + _4bz * Madg_Fix_q3) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * Madg_Fix_q0 - _8bz * Madg_Fix_q2) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
      s3        = _2q1 * (2.0f * (q1q3 - q0q2) - ax) + _2q2 * (2.0f * (q0q1 + q2q3) - ay) + (-_8bx * Madg_Fix_q3 + _4bz * Madg_Fix_q1) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (-_4bx * Madg_Fix_q0 + _4bz * Madg_Fix_q2) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * Madg_Fix_q1) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
      recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
      s0 *= recipNorm;
      s1 *= recipNorm;
      s2 *= recipNorm;
      s3 *= recipNorm;

      // Apply feedback step
      qDot1 -= Madg_Fix_beta * s0;
      qDot2 -= Madg_Fix_beta * s1;
      qDot3 -= Madg_Fix_beta * s2;
      qDot4 -= Madg_Fix_beta * s3;
   }

   // Integrate rate of change of quaternion to yield quaternion
   Madg_Fix_q0 += qDot1 * (1.0f / sampleFreq);
   Madg_Fix_q1 += qDot2 * (1.0f / sampleFreq);
   Madg_Fix_q2 += qDot3 * (1.0f / sampleFreq);
   Madg_Fix_q3 += qDot4 * (1.0f / sampleFreq);

   // Normalise quaternion
   recipNorm = invSqrt(Madg_Fix_q0 * Madg_Fix_q0 + Madg_Fix_q1 * Madg_Fix_q1 + Madg_Fix_q2 * Madg_Fix_q2 + Madg_Fix_q3 * Madg_Fix_q3);
   Madg_Fix_q0 *= recipNorm;
   Madg_Fix_q1 *= recipNorm;
   Madg_Fix_q2 *= recipNorm;
   Madg_Fix_q3 *= recipNorm;
}