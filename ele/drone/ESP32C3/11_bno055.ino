void quaternion(){
  imu::Quaternion quat = bno.getQuat();
  double w_data = quat.w();
  double x_data = quat.x();
  double y_data = quat.y();
  double z_data = quat.z();
  //----------------------------------------------------------------------pitch---------
  double ysqr = y_data * y_data;
  double t0 = 2.0 * (w_data * x_data + y_data * z_data);
  double t1 = 1.0 - 2.0 * (x_data * x_data + ysqr);
  degP = atan2(t0, t1) * 57.2957795131 - 3;
  //----------------------------------------------------------------------roll----------
  double t2 = 2.0 * (w_data * y_data - z_data * x_data);
  t2 = t2 > 1.0 ? 1.0 : t2;
  t2 = t2 < -1.0 ? -1.0 : t2;
  degR = asin(t2) * 57.2957795131 -1.1;
  //----------------------------------------------------------------------yaw  ---------
  double t3 = 2.0 * (w_data * z_data + x_data * y_data);
  double t4 = 1.0 - 2.0 * (ysqr + z_data * z_data);
  degY = atan2(t3, t4) * 57.2957795131;
}

void gyro(){
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  gyroTotal[0] = gyroTotal[0] - gyroXYZ[0][gyroIndex];  // 古いデータの合計から最も古いデータを減算
  gyroTotal[1] = gyroTotal[1] - gyroXYZ[1][gyroIndex];  // 古いデータの合計から最も古いデータを減算
  gyroTotal[2] = gyroTotal[2] - gyroXYZ[2][gyroIndex];  // 古いデータの合計から最も古いデータを減算
  gyroXYZ[0][gyroIndex] = gyro.x();  // 新しいデータをバッファに追加
  gyroXYZ[1][gyroIndex] = gyro.y();  // 新しいデータをバッファに追加  
  gyroXYZ[2][gyroIndex] = gyro.z();  // 新しいデータをバッファに追加
  gyroTotal[0] = gyroTotal[0] + gyroXYZ[0][gyroIndex];  // 新しいデータを合計に加算
  gyroTotal[1] = gyroTotal[1] + gyroXYZ[1][gyroIndex];  // 新しいデータを合計に加算
  gyroTotal[2] = gyroTotal[2] + gyroXYZ[2][gyroIndex];  // 新しいデータを合計に加算
  if (gyroIndex >= (gyroNumReadings - 1)){
    gyroIndex = 0;
  } else {
    gyroIndex += 1;
  }
  gyroP = gyroTotal[0] / gyroNumReadings;  // 平均値を計算
  gyroR = gyroTotal[1] / gyroNumReadings;  // 平均値を計算
  gyroY = gyroTotal[2] / gyroNumReadings;  // 平均値を計算
}