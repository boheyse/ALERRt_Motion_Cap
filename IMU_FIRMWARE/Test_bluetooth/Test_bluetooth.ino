#include <SD.h>
#define SD_CHIP_SELECT_PIN 38
#define logFileName "head.txt"
#define SD_MAX_FILE_SIZE 10000000

void setup()
{
  pinMode(13, OUTPUT);
  SerialUSB.begin(115200); // Initialize Serial Monitor USB
  Serial1.begin(115200); // Initialize hardware serial port, pins 0/1

  //while (!SerialUSB) ; // Wait for Serial monitor to open

  // Send a welcome message to the serial monitor:
  SerialUSB.println("Sending Inital message");
  if(initSD())
  {
    SerialUSB.println("SD card is initialized");
  }
}

String toSend = "";
String start = "1";
String toStop = "";
String Stop = "2";
bool runn = true;
bool tsfr = false;

void loop()
{ 
  while(Serial1.available())
  {
    toSend = "";
    toStop = "";
    toSend = (char)Serial1.read();
    SerialUSB.println(toSend);
    int count = 0;
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    if(toSend == start)
    {
      SerialUSB.println("SD card is recording");
      while(runn)
      {
        sdLogString("write ");
            digitalWrite(13, HIGH);
            delay(2000);
            digitalWrite(13, LOW);
        //Serial1.write("write");
        toStop = (char)Serial1.read();
   
        if(toStop == Stop)
        {
          SerialUSB.println(toStop);
          runn = false;
          tsfr = true;
          break;
        }
//        String toSend = "";
//        String cease = "2";
//        toSend += (char)Serial1.read();
//        if(toSend == cease)
//        {
//          Serial1.println("SD card was stopped");
//          File dataFile = SD.open("GUN4.txt");
//          if (dataFile) 
//          {
//            while (dataFile.available()) { Serial1.write(dataFile.read()); }
//            Serial1.println("Finished Writing");
//            dataFile.close();
          }   
          Serial1.println("Stop Received...\n");
        }

        if(tsfr){
          Serial1.println("Trying to Open SD Card...");
          File dataFile = SD.open("gun4.txt");
          Serial1.println("SD card was Opened");
          if (dataFile) 
          {
            while (dataFile.available()) { 
              Serial1.write(dataFile.read()); 
              delay(1); 
              }
            Serial1.println("\nFinished Writing");
            dataFile.close();
          }
        }

        
        runn = true;
        tsfr = false;
      }
}


 
  
    
//  String toSend = "";
//  toSend += (char)Serial1.read();
//
//  if(Serial1.available())
//  {
//    SerialUSB.println(toSend); 
//    delay(100);
//    Serial1.println("Thanks");
//  }
//
//  if (SerialUSB.available()) // If data is sent to the monitor
//  {
//    String toSend = ""; // Create a new string
//    while (SerialUSB.available()) // While data is available
//    {
//      // Read from SerialUSB and add to the string:
//      toSend += (char)SerialUSB.read();
//    }
//    // Print a message stating what we're sending:
//    SerialUSB.println("Sending " + toSend + " to Serial1");
//
//    // Send the assembled string out over the hardware
//    // Serial1 port (TX pin 1).
//    Serial1.print(toSend);
//    delay(500);
//  }

  


//  if (Serial1.available()) // If data is sent from device
//  {
//    //Serial1.println("Message Received ");
//    String toSend = ""; // Create a new string
//    String start = "1";
//    while (Serial1.available()) // While data is available
//    {
//      // Read from hardware port and add to the string:
//      toSend += (char)Serial1.read();
//      if(start == toSend)
//      {
//        SerialUSB.println("I got the start signal");
//      }
//    }
//    // Print a message stating what we've received:
//    SerialUSB.println(toSend);
//  }
//}

bool initSD(void)
{
  // SD.begin should return true if a valid SD card is present
  if ( !SD.begin(SD_CHIP_SELECT_PIN) )
  {
    return false;
  }

  return true;
}

bool sdLogString(String toLog)
{
  // Open the current file name:
  File logFile = SD.open(logFileName, FILE_WRITE);
  
  // If the file will get too big with this new string, create
  // a new one, and open it.
//  if (logFile.size() > (SD_MAX_FILE_SIZE - toLog.length()))
//  {
//    logFileName = nextLogFile();
//    logFile = SD.open(logFileName, FILE_WRITE);
//  }

  // If the log file opened properly, add the string to it.
  if (logFile)
  {
    logFile.print(toLog);
    logFile.close();

    return true; // Return success
  }

  return false; // Return fail
}
