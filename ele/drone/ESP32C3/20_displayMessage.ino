void displayMessage(){
  
//  Serial.print(" gyroP:");
//  displayValue(gyroP);
//  Serial.print(" pid_P2:");
//  displayValue(pid_P2);  
//  Serial.print(" gyroR:");
//  displayValue(gyroR);
//  Serial.print(" pid_R2:");
//  displayValue(pid_R2); 
  
//  Serial.print(" gyroY:");
//  displayValue(gyroY);
  
  
//  Serial.print("\t\t degP1: ");
//  Serial.print(degP);
//  Serial.print("\t\tpid_P2: ");
//  Serial.print(pid_P2);
//  Serial.print(" degR1: ");
//  Serial.print(degR);
//  Serial.print("\t\tpid_R2: ");
//  Serial.print(pid_R2);
  
  Serial.print(" degY1: ");
  Serial.print(degY);
//  Serial.print(" yawOn: ");
//  Serial.print(yawOn); 
  Serial.print(" refDegY: ");
  Serial.print(refDegY);
  Serial.print(" errDegY: ");
  Serial.print(errDegY);
//  Serial.print("\t\tpid_Y1: ");
//  Serial.print(pid_Y1);
  Serial.print("\t\tpid_Y2: ");
  Serial.print(pid_Y2);
//  Serial.print(" wifi_rudder: ");
//  Serial.print(wifi_rudder);
 
//  Serial.print(" voltage1: ");
//  displayValue(voltage1);
//  Serial.print(" voltage2: ");
//  displayValue(voltage2);
//  Serial.print(" voltage3: ");
//  displayValue(voltage3);
//  Serial.print(" voltage4: ");
//  displayValue(voltage4);
  
  /*
  Serial.print(" ActiveSense: ");
  Serial.print(wifi_Active_Sense);
  */
//  Serial.print("\t Throttle: ");
//  Serial.print(wifi_throttle);
  /*
  Serial.print(" Rudder: ");
  Serial.print(wifi_rudder);
  Serial.print(" Pitch: ");
  Serial.print( wifi_pitch);
  Serial.print(" Roll: ");
  Serial.print(wifi_roll);
  Serial.print(" SW-L: ");
  Serial.print(wifi_buttonL);
  Serial.print(" SW-R: ");
  Serial.print(wifi_buttonR);
  Serial.print(" Dip1: ");
  Serial.print(wifi_dip1);
  Serial.print(" Dip2: ");
  Serial.print(wifi_dip2);
  */
  /*
  Serial.print(" VR00: ");
  Serial.print(wifi_vr00);
  Serial.print(" VR01: ");
  Serial.print(wifi_vr01);
  Serial.print(" VR02: ");
  Serial.print(wifi_vr02);
  Serial.print(" VR03: ");
  Serial.print(wifi_vr03);
  Serial.print(" VR10: ");
  Serial.print(wifi_vr10);
  Serial.print(" VR11: ");
  Serial.print(wifi_vr11);
  Serial.print(" VR12: ");
  Serial.print(wifi_vr12);
  Serial.print(" VR13: ");
  Serial.print(wifi_vr13);
  */
//  Serial.print(" Y: ");
//  Serial.print(gyroAverage[1]);
//  Serial.print(" Z: ");
//  Serial.print(gyroAverage[2]); 
  /*
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  */
      

    /* Display calibration status for each sensor. */
/*
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.print(" CALIBRATION: Sys=");
  Serial.print(system, DEC);
  Serial.print(" Gyro=");
  Serial.print(gyro, DEC);
  Serial.print(" Accel=");
  Serial.print(accel, DEC);
  Serial.print(" Mag=");
  Serial.print(mag, DEC);
  */
  Serial.print("\n");

}

void displayValue(double inputData){
  if(abs(inputData) < 100){Serial.print(" ");}
  if(abs(inputData) < 10){Serial.print(" ");}
  if(inputData >= 0){Serial.print("+");}
  Serial.print(inputData);
}