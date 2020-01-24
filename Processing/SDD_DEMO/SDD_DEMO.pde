/******************************************************************************************
* Test Sketch for Razor AHRS v1.4.2
* 9 Degree of Measurement Attitude and Heading Reference System
* for Sparkfun "9DOF Razor IMU" and "9DOF Sensor Stick"
*
* Released under GNU GPL (General Public License) v3.0
* Copyright (C) 2013 Peter Bartz [http://ptrbrtz.net]
* Copyright (C) 2011-2012 Quality & Usability Lab, Deutsche Telekom Laboratories, TU Berlin
* Written by Peter Bartz (peter-bartz@gmx.de)
*
* Infos, updates, bug reports, contributions and feedback:
*     https://github.com/ptrbrtz/razor-9dof-ahrs
******************************************************************************************/

/*
  NOTE: There seems to be a bug with the serial library in Processing versions 1.5
  and 1.5.1: "WARNING: RXTX Version mismatch ...".
  Processing 2.0.x seems to work just fine. Later versions may too.
  Alternatively, the older version 1.2.1 also works and is still available on the web.
*/

import processing.opengl.*;
import processing.serial.*;

// IF THE SKETCH CRASHES OR HANGS ON STARTUP, MAKE SURE YOU ARE USING THE RIGHT SERIAL PORT:
// 1. Have a look at the Processing console output of this sketch.
// 2. Look for the serial port list and find the port you need (it's the same as in Arduino).
// 3. Set your port number here:
final static int SERIAL_PORT_NUM = 5;
// 4. Try again.


final static int SERIAL_PORT_BAUD_RATE = 57600;

int sampleCount = 1;

float yaw_head = 0.0f;
float pitch_head = 0.0f;
float roll_head = 0.0f;

float yaw_gun = 0.0f;
float pitch_gun = 0.0f;
float roll_gun = 0.0f;

float yaw_chest = 0.0f;
float pitch_chest = 0.0f;
float roll_chest = 0.0f;
    
float yawOffset = 0.0f;

String [] head; 
String [] chest;
String [] gun;

PFont font;
PShape gun_obj;
PShape chest_obj;
PShape head_obj;

PShape alien = createShape(GROUP);

// Add the two "child" shapes to the parent group
alien.addChild(head_obj);
alien.addChild(chest_obj);

// Draw the group
translate(width/2, height/2);
shape(alien);

Serial serial;

boolean synched = false;

// Skip incoming serial stream data until token is found
boolean readToken(Serial serial, String token) {
  // Wait until enough bytes are available
  if (serial.available() < token.length())
    return false;
  
  // Check if incoming bytes match token
  for (int i = 0; i < token.length(); i++) {
    if (serial.read() != token.charAt(i))
      return false;
  }
  
  return true;
}

// Global setup
void setup() {
  // Setup graphics
  size(1200, 700, P3D);
  gun_obj = loadShape("glock.obj");
  head_obj = loadShape("head.obj");
  chest_obj = loadShape("chest.obj");
  
  //gun_obj.vertex(0,-100);
  gun_obj.scale(45.0);
  
  head_obj.vertex(0,0);
  head_obj.scale(8.0);
 // head_obj.rotate(PI);
  
  chest_obj.vertex(0,0);
  chest_obj.scale(17.0);
  
  smooth();
  noStroke();
  frameRate(40);
  
  // Load font
  font = loadFont("Univers-66.vlw");
  textFont(font);
  
  head = loadStrings("/Users/Heyseb1/Documents/D2/processing_data/demo/Head.csv");
  chest = loadStrings("/Users/Heyseb1/Documents/D2/processing_data/demo/Chest.csv");
  gun = loadStrings("/Users/Heyseb1/Documents/D2/processing_data/demo/Gun.csv");
  // Setup serial port I/O
  println("AVAILABLE SERIAL PORTS:");
 // println(Serial.list());
  //String portName = Serial.list()[SERIAL_PORT_NUM];
  println();
  println("HAVE A LOOK AT THE LIST ABOVE AND SET THE RIGHT SERIAL PORT NUMBER IN THE CODE!");
  //println("  -> Using port " + SERIAL_PORT_NUM + ": " + portName);
  //serial = new Serial(this, portName, SERIAL_PORT_BAUD_RATE);
}

void setupRazor() {
  println("Trying to setup and synch Razor...");
  
  // On Mac OSX and Linux (Windows too?) the board will do a reset when we connect, which is really bad.
  // See "Automatic (Software) Reset" on http://www.arduino.cc/en/Main/ArduinoBoardProMini
  // So we have to wait until the bootloader is finished and the Razor firmware can receive commands.
  // To prevent this, disconnect/cut/unplug the DTR line going to the board. This also has the advantage,
  // that the angles you receive are stable right from the beginning. 
  delay(3000);  // 3 seconds should be enough
  
  println("Never gets passed here");
  // Set Razor output parameters
  //serial.write("#ob");  // Turn on binary output
  //serial.write("#o1");  // Turn on continuous streaming output
  //serial.write("#oe0"); // Disable error message output
  
  //// Synch with Razor
  //serial.clear();  // Clear input buffer up to here
  //serial.write("#s00");  // Request synch token
}

void getValueChest() {
  try{
    String line = chest[sampleCount];
    String[] pieces = split(line, ',');
    yaw_chest = float(pieces[1]) -255;
    roll_chest = float(pieces[2]);
    pitch_chest = float(pieces[3]) -88;
    
    }
    catch(ArrayIndexOutOfBoundsException e){
      sampleCount = 0;
    }
}

void getValueHead() {
  try{
    String line = head[sampleCount];
    String[] pieces = split(line, ',');
    yaw_head = float(pieces[1]) -110;
    roll_head = float(pieces[2]) -10;
    pitch_head = float(pieces[3]) + 75+20;

    }
    catch(ArrayIndexOutOfBoundsException e){
      sampleCount = 0;
    }
}

void getValueGun() {
  try{
    String line = gun[sampleCount];
    String[] pieces = split(line, ',');
    yaw_gun = float(pieces[1])-95;
    pitch_gun = float(pieces[2]);
    roll_gun = float(pieces[3]);
    sampleCount++; }
    catch(ArrayIndexOutOfBoundsException e){
      sampleCount = 0;
    }
}

void draw() {
   // Reset scene
  background(0);
  lights();

  // Sync with Razor 
  if (!synched) {
    textAlign(CENTER);
    fill(255);
    text("Loading your Recording...", width/2, height/2, -200);
    
    if (frameCount == 2)
      setupRazor();  // Set ouput params and request synch token
    else if (frameCount > 2)
      synched = true;
      //synched = readToken(serial, "#SYNCH00\r\n");  // Look for synch token
    return;
  }
  
  getValueGun();
  // Draw gun
  pushMatrix();
  translate(width-100, height -40, -600);
  rotateY(radians(yaw_gun));
  rotateX(radians(roll_gun));
  rotateZ(radians(pitch_gun));
  shape(gun_obj);
  popMatrix();
  
  getValueHead();
  // Draw head
  pushMatrix();
  translate(width/2.0, -1, -500);
  rotateY(radians(yaw_head));
  rotateX(radians(pitch_head));
  rotateZ(radians(roll_head));
  shape(head_obj); 
  popMatrix();
  
  getValueChest();
    // Draw chest
  pushMatrix();
  translate((width/2.0) - 25, height-125, -500);
  rotateY(radians(yaw_chest));
  rotateX(-radians(pitch_chest) );
  rotateZ(radians(roll_chest));

  shape(chest_obj);
  popMatrix();
  
  textFont(font, 20);
  fill(255);
  textAlign(LEFT);

  // Output info text
  //text("Point FTDI connector towards screen and press 'a' to align", 10, 25);

//  // Output angles
//  pushMatrix();
//  translate(10, height - 10);
//  textAlign(LEFT);
//  text("Yaw: " + ((int) yaw_chest), 0, 0);
//  text("Pitch: " + ((int) pitch_chest), 150, 0);
//  text("Roll: " + ((int) roll_chest), 300, 0);
//  popMatrix();
}

void keyPressed() {
  switch (key) {
    case '0':  // Turn Razor's continuous output stream off
      serial.write("#o0");
      break;
    case '1':  // Turn Razor's continuous output stream on
      serial.write("#o1");
      break;
    case 'f':  // Request one single yaw/pitch/roll frame from Razor (use when continuous streaming is off)
      serial.write("#f");
      break;
    case 'a':  // Align screen with Razor
      //yawOffset = yaw;
  }
}
