void loop(void){
  if (i2c_active ==1){

    quaternion(); //----------------------Outer Loop---------------
    double refDegP = wifi_pitch/10;
    double errDegP = refDegP - degP;
    pid_P1 = (pitch_Kp1 + wifi_vr01 / 10) * errDegP;
    if ((degP < -90) || (degP > 90)){
      m_alarm = 1;
      digitalWrite(10,HIGH);
    }
    double refDegR = wifi_roll/10;
    double errDegR = refDegR - degR;
    pid_R1 = (roll_Kp1 + wifi_vr11 / 10)  * errDegR;
    if ((degR < -90) || (degR > 90)){
      m_alarm = 1;
      digitalWrite(10,HIGH);
    }
    gyro(); //-----------------------------Inner Loop--------------
    double errDegP_s = pid_P1 - gyroP;
    double sumErrP = sumErrP + errDegP_s;
    if (sumErrP > pitch_i_Limit){sumErrP = pitch_i_Limit;}
    if (sumErrP < -pitch_i_Limit){sumErrP = -pitch_i_Limit;}
    double derivP = errDegP_s - preErrDegP;
    preErrDegP = errDegP_s;
    pid_P2 = (pitch_Kp2 +  (wifi_vr02 / 10)) * errDegP_s + (pitch_Ki2) * sumErrP + (pitch_Kd2 +  (wifi_vr03 / 10)) * derivP;
    if (pid_P2 > pid_P2_Limit){pid_P2 = pid_P2_Limit;}
    if (pid_P2 < -pid_P2_Limit){pid_P2 = -pid_P2_Limit;}

    double errDegR_s = pid_R1 - gyroR;
    double sumErrR = sumErrR + errDegR_s;
    if (sumErrR > roll_i_Limit){sumErrR = roll_i_Limit;}
    if (sumErrR < -roll_i_Limit){sumErrR = -roll_i_Limit;}
    double derivR = errDegR_s - preErrDegR;
    preErrDegR = errDegR_s;
    pid_R2 = (roll_Kp2 +  (wifi_vr12 / 10)) * errDegR_s + (roll_Ki2) * sumErrR + (roll_Kd2 +  (wifi_vr13 / 10)) * derivR;
    if (pid_R2 > pid_R2_Limit){pid_R2 = pid_R2_Limit;}
    if (pid_R2 < -pid_R2_Limit){pid_R2 = -pid_R2_Limit;}

    motor_drv();

    displayMessage();
    i2c_active = 0;
  }
//  delay(BNO055_SAMPLERATE_DELAY_MS);
}
