void get_adValue2(int ad_Num){
  SPI.beginTransaction(settings);
    digitalWrite(SS2, LOW);
    switch(ad_Num){
      case 0:
        SPI.transfer(0b00000110); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b00000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 1:
        SPI.transfer(0b00000110); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b01000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 2:
        SPI.transfer(0b00000110); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b10000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 3:
        SPI.transfer(0b00000110); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b11000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 4:
        SPI.transfer(0b00000111); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b00000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 5:
        SPI.transfer(0b00000111); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b01000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 6:
        SPI.transfer(0b00000111); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b10000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      case 7:
        SPI.transfer(0b00000111); // Start bit 1 , CH0 singleEnd
        ad_HighByte = SPI.transfer(0b11000000); // CH0 singleEnd
        ad_LowByte = SPI.transfer(0x00); // dummy
        break;
      default:
        break;
    }
  digitalWrite(SS2, HIGH);
  SPI.endTransaction();
}