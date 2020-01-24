/************************************************************
MPU9250_Basic
 Basic example sketch for MPU-9250 DMP Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library
This example sketch demonstrates how to initialize the 
MPU-9250, and stream its sensor outputs to a serial monitor.
Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0
Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
*************************************************************/
#include <SparkFunMPU9250-DMP.h>
#include <SD.h>

#define SerialPort SerialUSB
#define SD_CHIP_SELECT_PIN 38
#define LOG_FILE_INDEX_MAX 999 // Max number of files
#define LOG_FILE_PREFIX "chestS"  // Prefix name for log files
#define LOG_FILE_SUFFIX ".txt"  // Suffix name for log files
#define SAMPLE_RATE 20         // Rate to poll for data
#define COMPASS_SAMPLE_RATE 20
#define SD_LOG_WRITE_BUFFER_SIZE 2048 //let buffer fill to 1 kb of data
#define BatLevel A0 // Analog input pin to measure battery level

String logFileName = "";

MPU9250_DMP imu;

String toSend = "";
String start = "s";
String toStop = "";
String Stop = "x";
String DL = "";
String timestamp = "";
String timestamp_high = "";
String dataFileBuffer;
int timestamp_low = 0;
int ms_per_sample = 1000/SAMPLE_RATE;
int count = 0;
int check = 0;
int LipoV = 0;    // Battery Voltage variable
bool runn = true;
bool tsfr = false;
String beginTransfer = "A";


void setup() 
{
  SerialUSB.begin(38400); // Initialize Serial Monitor USB
  Serial1.begin(38400); // Initialize hardware serial port, pins 0/1
  pinMode(BatLevel,INPUT);   // Set the analog input pin to input mode
  //while (!SerialUSB) ;
  // Call imu.begin() to verify communication with and
  // initialize the MPU-9250 to it's default values.
  // Most functions return an error code - INV_SUCCESS (0)
  // indicates the IMU was present and successfully set up
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      SerialUSB.println("Unable to communicate with MPU-9250");
      SerialUSB.println("Check connections, and try again.");
      Serial1.println();
      delay(5000);
    }
  }

  // Use setSensors to turn on or off MPU-9250 sensors.
  // Any of the following defines can be combined:
  // INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_XYZ_COMPASS,
  // INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
  // Enable all sensors:
  imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

  // Use setGyroFSR() and setAccelFSR() to configure the
  // gyroscope and accelerometer full scale ranges.
  // Gyro options are +/- 250, 500, 1000, or 2000 dps
  imu.setGyroFSR(2000); // Set gyro to 2000 dps
  // Accel options are +/- 2, 4, 8, or 16 g
  imu.setAccelFSR(2); // Set accel to +/-2g
  // Note: the MPU-9250's magnetometer FSR is set at 
  // +/- 4912 uT (micro-tesla's)

  // setLPF() can be used to set the digital low-pass filter
  // of the accelerometer and gyroscope.
  // Can be any of the following: 188, 98, 42, 20, 10, 5
  // (values are in Hz).
  imu.setLPF(5); // Set LPF corner frequency to 5Hz

  // The sample rate of the accel/gyro can be set using
  // setSampleRate. Acceptable values range from 4Hz to 1kHz
  imu.setSampleRate(SAMPLE_RATE); // Set sample rate to 10Hz

  // Likewise, the compass (magnetometer) sample rate can be
  // set using the setCompassSampleRate() function.
  // This value can range between: 1-100Hz
  imu.setCompassSampleRate(COMPASS_SAMPLE_RATE); // Set mag rate to 10Hz
  SerialUSB.println("Initilaize");
  if(initSD())
  {
    SerialUSB.println(logFileName);
    SerialUSB.println("SD card is initialized");
  }
}


void loop() 
{
  while(Serial1.available())
  {
    char help = 'a';
    toSend = "";
    toStop = "";
    help = (char)Serial1.read();
    toSend = help;
    check = int(help);
    //SerialUSB.println(check);
    if(check >= 48 && check <= 57){
      timestamp += help;
      SerialUSB.println(timestamp);
    }
    else{
      SerialUSB.print("pass");
    }
    
    if(toSend == start)
    {
      timestamp_high = timestamp.substring(0,4);
      SerialUSB.println(timestamp_high);
      timestamp_low = timestamp.substring(4,13).toInt();
      SerialUSB.println(timestamp_low);
      
      //Serial1.println("Please place phone in pocket, recording will begin in 15 seconds...\n\n");
      logFileName = nextLogFile();
      if(count > 0){
        delay(1000);
      }
      sdLogString("HeadHead\n\n");
      if (timestamp.length() == 13){
        CheckBatLevel();
      }
      //Serial1.println("Recording has started");
      SerialUSB.println("SD card is recording");
      while(runn)
      {
        // dataReady() checks to see if new accel/gyro data
        // is available. It will return a boolean true or false
        // (New magnetometer data cannot be checked, as the library
        //  runs that sensor in single-conversion mode.)
        if ( imu.dataReady() )
        {
          // Call update() to update the imu objects sensor data.
          // You can specify which sensors to update by combining
          // UPDATE_ACCEL, UPDATE_GYRO, UPDATE_COMPASS, and/or
          // UPDATE_TEMPERATURE.
          // (The update function defaults to accel, gyro, compass,
          //  so you don't have to specify these values.)
          imu.update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
          
          getIMUData();
          toStop = (char)Serial1.read();
   
          if(toStop == Stop)
          {
            SerialUSB.println(toStop);
            runn = false;
            //tsfr = true;
            count ++;
            toSend = "A"; // uncomment to send data to app without chest going first
            break;
          }
          }
      }   
        //Serial1.println("Stop Received...\n");
        
        runn = true;
        tsfr = false;     
  } 
   // DL = (char)Serial1.read();
        

    if(toSend == beginTransfer){
      tsfr = true;
      delay(5000);
    }
    
    if(tsfr){
      Serial1.println("Transferring Data from: Head\n");
      File dataFile = SD.open(logFileName);
      //Serial1.println("SD card was Opened");
      if (dataFile) 
      {
        while (dataFile.available()) { 
          Serial1.write(dataFile.read()); 
          delay(1); 
          }
        Serial1.println("\nFinished Writing: Head\n");
        dataFile.close();
        Serial1.println("NextNext");
      }
      timestamp_high = "";
      timestamp_low = 0;
      timestamp = "";
      Serial1.flush();
      CheckBatLevel();
    }
    tsfr = false;
  }
}


String getIMUData(void)
{  
   String data = "";
// After calling update() the ax, ay, az, gx, gy, gz, mx,
  // my, mz, time, and/or temerature class variables are all
  // updated. Access them by placing the object. in front:

  // Use the calcAccel, calcGyro, and calcMag functions to
  // convert the raw sensor readings (signed 16-bit values)
  // to their respective units.
  float accelX = imu.calcAccel(imu.ax);
  float accelY = imu.calcAccel(imu.ay);
  float accelZ = imu.calcAccel(imu.az);
  float gyroX = imu.calcGyro(imu.gx);
  float gyroY = imu.calcGyro(imu.gy);
  float gyroZ = imu.calcGyro(imu.gz);
  float magX = imu.calcMag(imu.mx);
  float magY = imu.calcMag(imu.my);
  float magZ = imu.calcMag(imu.mz);
  timestamp_low = timestamp_low + ms_per_sample;
  SerialUSB.println(imu.time);
  SerialUSB.println(timestamp_low);
  
  data = timestamp_high + String(timestamp_low) + 
              ", " + String(gyroX) + ", " +
              String(gyroY) + ", " + String(gyroZ) + ", " + 
              String(accelX) + ", " + String(accelY) + ", " + 
              String(accelZ) + ", " + String(magX) + ", " +
              String(magY) + ", " + String(magZ) + "\n";
              
  if (data.length() + dataFileBuffer.length() >=
        SD_LOG_WRITE_BUFFER_SIZE)
    {
      sdLogString(dataFileBuffer); // Log SD buffer
      dataFileBuffer = ""; // Clear SD log buffer 
    }
    // Add new line to SD log buffer
    dataFileBuffer += data;
}


bool initSD(void)
{
  // SD.begin should return true if a valid SD card is present
  if ( !SD.begin(SD_CHIP_SELECT_PIN) )
  {
    SerialUSB.println("Returning false");
    return false;
  }

  return true;
}

bool sdLogString(String toLog)
{
  // Open the current file name:
  File logFile = SD.open(logFileName, FILE_WRITE);

  // If the log file opened properly, add the string to it.
  if (logFile)
  {
    SerialUSB.println("Writing to SD");
    logFile.print(toLog);
    SerialUSB.println("Trying to close");
    logFile.close();
    SerialUSB.println("Finished Writing to SD");
    return true; // Return success
  }
  SerialUSB.println("I'm returning false");
  return false; // Return fail
}

// Find the next available log file. Or return a null string
// if we've reached the maximum file limit.
String nextLogFile(void)
{
  String filename = "";
  int logIndex = 1;

  for (int i = 1; i < LOG_FILE_INDEX_MAX; i++)
  {
    // Construct a file with PREFIX[Index].SUFFIX
    filename = LOG_FILE_PREFIX;
    filename += String(logIndex);
    filename += LOG_FILE_SUFFIX;
    // If the file name doesn't exist, return it
    if (!SD.exists(filename))
    {
      return filename;
    }
    // Otherwise increment the index, and try again
    logIndex++;
  }

  return "";
}

// Battery Level Status function
void CheckBatLevel(void)
{
  LipoV = analogRead(BatLevel); // LipoV is determined by ADC value of the analog input voltage
  // 710 = 3.3 Volts, 905 = 4.2 Volts
  LipoV = map(LipoV,710,905,0,100);   // convert analog read values to percentage
  Serial1.print("BattH:" + String(LipoV));
}
