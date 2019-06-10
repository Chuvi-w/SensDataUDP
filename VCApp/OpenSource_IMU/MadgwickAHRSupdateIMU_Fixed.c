void MadgwickAHRSupdateIMU(double gx, double gy, double gz, double ax, double ay, double az)
{
   double recipNorm;
   double s0, s1, s2, s3;
   double qDot1, qDot2, qDot3, qDot4;
   double _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

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

      // Auxiliary variables to avoid repeated arithmetic
      _2q0 = 2.0f * Madg_Fix_q0;
      _2q1 = 2.0f * Madg_Fix_q1;
      _2q2 = 2.0f * Madg_Fix_q2;
      _2q3 = 2.0f * Madg_Fix_q3;
      _4q0 = 4.0f * Madg_Fix_q0;
      _4q1 = 4.0f * Madg_Fix_q1;
      _4q2 = 4.0f * Madg_Fix_q2;
      _8q1 = 8.0f * Madg_Fix_q1;
      _8q2 = 8.0f * Madg_Fix_q2;
      q0q0 = Madg_Fix_q0 * Madg_Fix_q0;
      q1q1 = Madg_Fix_q1 * Madg_Fix_q1;
      q2q2 = Madg_Fix_q2 * Madg_Fix_q2;
      q3q3 = Madg_Fix_q3 * Madg_Fix_q3;

      // Gradient decent algorithm corrective step
      s0        = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
      s1        = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * Madg_Fix_q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
      s2        = 4.0f * q0q0 * Madg_Fix_q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
      s3        = 4.0f * q1q1 * Madg_Fix_q3 - _2q1 * ax + 4.0f * q2q2 * Madg_Fix_q3 - _2q2 * ay;
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