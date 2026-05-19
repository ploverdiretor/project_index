
 import processing.serial.*;
 
Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
String str = "Hello world!";

void setup() {
  background(0);
  size(640, 360);
  fill(255);
  textAlign(CENTER,CENTER);
  textSize(40);
  text("Hello world!",320,180);
  
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
}

void mousePressed(){
  myPort.write('\n');
  myPort.write(str);
  myPort.write('\n');
}

void serialEvent(Serial p){
  int x = p.read();  //xにシリアルの値を入れる
  print((char)x); //processingのシリアルモニタに数値を表示
}

void keyPressed(){
  myPort.write(key);
}
