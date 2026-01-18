void loop(void){
  double sumErrPi;
  double sumErrRi;
  double sumErrP;
  double sumErrR;
  double sumErrY;

  if (i2c_active ==1){

    quaternion(); //-----------------------------Outer Loop--------------------------------------------
    double refDegP = wifi_pitch/10.5;                               //  pitch ----------------
    double errDegP = refDegP - degP;        //P制御
    if(integralOn){
      sumErrPi = sumErrPi + errDegP;        //I制御
    }else{
      sumErrPi=0;
    }
    if (sumErrPi > pitch_i_Limit){sumErrPi = pitch_i_Limit;}
    if (sumErrPi < -pitch_i_Limit){sumErrPi = -pitch_i_Limit;}
//    pid_P1 = (pitch_Kp1 + 7.9 + wifi_vr01 / 10) * errDegP + (pitch_Ki1 + 0.2 + (wifi_vr02 / 10)) * sumErrPi;
    pid_P1 = (pitch_Kp1 + 7.9) * errDegP + (pitch_Ki1 + 0.2) * sumErrPi;
    if ((degP < -90) || (degP > 90)){
      m_alarm = 1;
      digitalWrite(10,HIGH);
    }
    double refDegR = wifi_roll/10.5;                                //  roll  ----------------
    double errDegR = refDegR - degR;
    if(integralOn){
    sumErrRi = sumErrRi + errDegR;
    }else{
      sumErrRi = 0;
    }
    if (sumErrRi > roll_i_Limit){sumErrRi = roll_i_Limit;}
    if (sumErrRi < -roll_i_Limit){sumErrRi = -roll_i_Limit;}
//    pid_R1 = (roll_Kp1 + 5 + wifi_vr11 / 10)  * errDegR + (roll_Ki1 + (wifi_vr12 / 10)) * sumErrRi;
    pid_R1 = (roll_Kp1 + 5)  * errDegR + (roll_Ki1) * sumErrRi;
    if ((degR < -90) || (degR > 90)){
      m_alarm = 1;
      digitalWrite(10,HIGH);
    }
    //if (yawOn == 0){                                                //  yaw   ----------------
    //  initYaw = degY;
    //}
    if((m_alarm==1)||(yawOn==0)){
      refDegY = degY;
    }else{
      refDegY -= (double)wifi_rudder/50;
    }
//    refDegY = degY - wifi_rudder/10;
    if((refDegY - degY)>180){
      errDegY = refDegY - degY -360;
    }else if((refDegY - degY)<(-180)){
      errDegY = refDegY - degY +360;
    }else{
      errDegY = refDegY - degY;
    }
    if((errDegY< -90)||(errDegY>90)){
      m_alarm = 1;
      digitalWrite(10,HIGH);
    }
    pid_Y1 = (yaw_Kp1 + 3+ (wifi_vr11 / 10))  * errDegY;

    gyro(); //----------------------------------Inner Loop--------------------------------------------
    double errDegP_s = pid_P1 - gyroP;                              //  pitch ----------------
    if(integralOn){
      sumErrP = sumErrP + errDegP_s;
    }else{
      sumErrP = 0;
    }
    if (sumErrP > pitch_i_Limit){sumErrP = pitch_i_Limit;}
    if (sumErrP < -pitch_i_Limit){sumErrP = -pitch_i_Limit;}
    double derivP = errDegP_s - preErrDegP;
    preErrDegP = errDegP_s;
//    pid_P2 = (pitch_Kp2 +0.1) * errDegP_s + (pitch_Ki2) * sumErrP + (pitch_Kd2 + 1.3 + (wifi_vr03 / 10)) * derivP;
    pid_P2 = (pitch_Kp2 +0.1) * errDegP_s + (pitch_Ki2) * sumErrP + (pitch_Kd2 + 1.3) * derivP;
    if (pid_P2 > pid_P2_Limit){pid_P2 = pid_P2_Limit;}
    if (pid_P2 < -pid_P2_Limit){pid_P2 = -pid_P2_Limit;}

    double errDegR_s = pid_R1 - gyroR;                              //  roll  ----------------
    if(integralOn){
      sumErrR = sumErrR + errDegR_s;
    }else{
      sumErrR = 0;
    }
    if (sumErrR > roll_i_Limit){sumErrR = roll_i_Limit;}
    if (sumErrR < -roll_i_Limit){sumErrR = -roll_i_Limit;}
    double derivR = errDegR_s - preErrDegR;
    preErrDegR = errDegR_s;
//    pid_R2 = (roll_Kp2 + 0.3) * errDegR_s + (roll_Ki2) * sumErrR + (roll_Kd2 + 1.0 + (wifi_vr13 / 10)) * derivR;
    pid_R2 = (roll_Kp2 + 0.3) * errDegR_s + (roll_Ki2) * sumErrR + (roll_Kd2 + 1.0) * derivR;
    if (pid_R2 > pid_R2_Limit){pid_R2 = pid_R2_Limit;}
    if (pid_R2 < -pid_R2_Limit){pid_R2 = -pid_R2_Limit;}

    double errDegY_s = pid_Y1 - gyroY;                              //  yaw   ----------------
    if(integralOn){
      sumErrY = sumErrY + errDegY_s;
    }else{
      sumErrY = 0;
    }
    if (sumErrY > yaw_i_Limit){sumErrY = yaw_i_Limit;}
    if (sumErrY < -yaw_i_Limit){sumErrY = -yaw_i_Limit;}
    double derivY = errDegY_s - preErrDegY;
    preErrDegY = errDegY_s;
    pid_Y2 = (yaw_Kp2+ 0+ (wifi_vr12 / 10) ) * errDegY_s + (yaw_Ki2) * sumErrY + (yaw_Kd2 + 0 + (wifi_vr13 / 10)) * derivY;
    if (pid_Y2 > pid_Y2_Limit){pid_Y2 = pid_Y2_Limit;}
    if (pid_Y2 < -pid_Y2_Limit){pid_Y2 = -pid_Y2_Limit;}

    motor_drv();

    displayMessage();
    i2c_active = 0;
  }
//  delay(BNO055_SAMPLERATE_DELAY_MS);
}
