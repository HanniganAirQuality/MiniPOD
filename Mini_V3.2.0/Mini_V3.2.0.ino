/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    Mini_V3.2.0.ino
 * @version Percy's 3.2.0
 * @brief   Integrates updated OPC-R2 firmware written as OPC-R2.h & .cpp
 *
 * @author 	Percy Smith
 * @date 	  June 7, 2025
 * @log     Do not change POD ID here, do that in Mini_node.h
 *          File Naming is now based on date to prevent huge files
 *          Adjust clock using the PCF8523_ClockAdjust.ino file first (then upload this)
******************************************************************************/

/*************  Included Libraries  *************/
#include "Mini_node.h"
#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>

/*************  Global Declarations  *************/
#if RTC_ENABLED
  #include <RTClib.h>
  RTC_PCF8523 rtc;
  DateTime rtc_date_time;
#endif

#if SDCARD_LOG_ENABLED
  SdFat sd;
  SdFile file;
  
  char fileName[] = "MMM_YYYY_MM_DD.CSV";
  int Y,M,D;
#endif

#if OPC_ENABLED
  #include "OPC-R2.h"
  OPC opc;
  histogramData opcData;
#endif

void setup()
{
  #if SERIAL_LOG_ENABLED
    Serial.begin(9600);
    Serial.println("Initialized Serial Monitor");
  #endif

  SPI.begin();

  #if OPC_ENABLED
    opc.begin();  // initializes CSpin as output & turns on the opc fan and laser
    if(!opc.begin())
    {
      #if SERIAL_LOG_ENABLED
        Serial.println("Error: Failed to initialize OPC!");
      #endif
    }
    opc.on();
    digitalWrite(RED_LED, HIGH);
    Serial.println("OPC on!");
    digitalWrite(OPC_CSPIN, HIGH); // just in case this closes SPI OPC line
  #endif

  // i think this is for our i2c with the RTC but who knows lol
  Wire.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(EXT_GREEN_LED, OUTPUT);
  pinMode(EXT_RED_LED, OUTPUT);

  // this is unnecessary in this firmware, but we're just checking that it's init at least
  #if RTC_ENABLED 
    if (!rtc.begin())
    {
      #if SERIAL_LOG_ENABLED
        Serial.println("Error: Failed to initialize RTC module");
      #endif
    }
    else
    {
      rtc_date_time = rtc.now();
    }
  #endif //RTC_ENABLED
  #if SDCARD_LOG_ENABLED
    pinMode(SD_CSPIN, OUTPUT);
    digitalWrite(SD_CSPIN, LOW);
    sd.begin(SD_CSPIN);
    while (!sd.begin(SD_CSPIN)) 
    {
      #if SERIAL_LOG_ENABLED
        Serial.println("SD Initialization Failed");
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(EXT_GREEN_LED, LOW);
        digitalWrite(EXT_RED_LED, HIGH);
        digitalWrite(RED_LED, LOW);
      #endif
      sd.begin(SD_CSPIN);
    }
    DateTime now = rtc.now();     //pulls setup() time so we have one file name per run in a day
    Y = now.year();
    M = now.month();
    D = now.day();
  sprintf(fileName, "%s_%04u_%02u_%02u.CSV", MiniID, Y, M, D);    //char array for fileName
  delay(100);   
  file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
  file.close();                                       //close file, we opened so loop() is faster 
  digitalWrite(SD_CSPIN, HIGH);    //release chip select on SD - allow other comm with SPI
  digitalWrite(GREEN_LED, LOW);     //turn off green LED (file is closed)
  digitalWrite(EXT_GREEN_LED, LOW);
  #endif

  delay(1000);
}

void loop()
{
  #if OPC_ENABLED
    digitalWrite(OPC_CSPIN, LOW);
    if(!opc.begin())  
    {
      digitalWrite(RED_LED, LOW);
      Serial.println("Cannot find OPC");
      opc.begin();
    }
    else
    {
      opcData = opc.histogramFormatted();
      // Serial.println("Polled for histogram");
      digitalWrite(RED_LED, HIGH);
    }
    digitalWrite(OPC_CSPIN, HIGH);
  #endif  //OPC_ENABLED

  #if SERIAL_LOG_ENABLED
    if(!Serial) {  //check if Serial is available... if not,
      Serial.end();      // close serial port
      delay(100);        //wait 100 millis
      Serial.begin(9600); // reenable serial again
    }    else    {
      #if RTC_ENABLED
        rtc_date_time = rtc.now();
        Serial.print(rtc_date_time.timestamp());
        Serial.print(",");
      #endif 
      #if OPC_ENABLED  // this is all the serial printing for testing
        for(int i = 0; i < 16; i++){
          Serial.print("Bin " + String(i) + ": " + String(opcData.bin[i]) + ",");
          delay(10);
        }
        Serial.print("\n  ");
        for(int j = 0; j < 4; j++){
          Serial.print("MToF " + String(j*2+1) + ": " + String(opcData.MToF[j]) + ",");
          delay(10);
        }
        delay(10);
        Serial.print("\n  ");
        Serial.print("Byte T: " + String(opcData.rawComms[41]) + String(opcData.rawComms[40]) + ",");
        Serial.print("Temperature: " + String(opcData.T_C) + "(" + String(opcData.signal_temp) + "),");
        delay(10);
        Serial.print("Byte RH: " + String(opcData.rawComms[43]) + String(opcData.rawComms[42]) + ",");
        Serial.print("Percent Relative Humidity: " + String(opcData.RH) + "(" + String(opcData.signal_relhum) + "),");
        delay(10);
        Serial.print("\n  ");
        Serial.print("Sample Flow Rate: " + String(opcData.sampleflowrate) + ",");
        delay(10);
        Serial.print("Sample Period: " + String(opcData.samplingperiod) + ",");
        delay(10);
        Serial.print("\n  ");
        Serial.print("Reject count Glitch: " + String(opcData.reject[0]) + ",");
        delay(10);
        Serial.print("Reject count Long: " + String(opcData.reject[1]) + ",");
        delay(10);
        Serial.print("\n  ");
        Serial.print("PM_A (PM1.0): " +  String(opcData.PM_ENV[0]) + ",");
        delay(10);
        Serial.print("PM_B (PM2.5): " + String(opcData.PM_ENV[1]) + ",");
        delay(10);
        Serial.print("PM_C (PM10.0): " + String(opcData.PM_ENV[2]) + ",");
        delay(10);
        Serial.print("\n  ");
        Serial.print("Checksum: " + String(opcData.checksum) + ",");
        Serial.println("Verify Checksum: " + String(opcData.verifycheck) + ",");
      #endif
    }
    Serial.print("\n");
  #endif  //SERIAL_LOG_ENABLED

  #if OPC_ENABLED
    digitalWrite(OPC_CSPIN, LOW);
    if(!opc.begin())  {
      digitalWrite(RED_LED, LOW);
      Serial.println("Cannot find OPC");
      opc.begin();
    }
    else
    {
      opcData = opc.histogramFormatted();
      // Serial.println("Polled for histogram");
      digitalWrite(RED_LED, HIGH);
    }
    digitalWrite(OPC_CSPIN, HIGH);
    delay(100);
  #endif  //OPC_ENABLED

  #if SDCARD_LOG_ENABLED    
    digitalWrite(SD_CSPIN, LOW);
    delay(100);
    while(!sd.begin(SD_CSPIN))
    {
      // this error changes the LED on the outside of the POD to red when the SD card fails to be found
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(EXT_GREEN_LED, LOW);
      digitalWrite(EXT_RED_LED, HIGH);
      #if SERIAL_LOG_ENABLED
        Serial.println("Issues opening SD in loop");
      #endif
      sd.begin(SD_CSPIN);
    }
    
    if (sd.begin(SD_CSPIN))
    {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(EXT_GREEN_LED, HIGH);
      digitalWrite(EXT_RED_LED, LOW);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
      #if RTC_ENABLED
        rtc_date_time = rtc.now();
        file.print("\r\n");
        file.print(rtc_date_time.timestamp());
        file.print(",");
      #else
        file.print(",");
      #endif

      #if OPC_ENABLED
      // Bins 0 - 15 
        for(int i = 0; i < 16; i++){
          file.print(String(opcData.bin[i]) + ",");
          delay(10);
        }
      // PM in ug/m^3 
        file.print(String(opcData.PM_ENV[0]) + ",");
        delay(10);
        file.print(String(opcData.PM_ENV[1]) + ",");
        delay(10);
        file.print(String(opcData.PM_ENV[2]) + ",");
        delay(10);
      // sampling information 
        file.print(String(opcData.sampleflowrate) + ",");
        delay(10);
        file.print(String(opcData.samplingperiod) + ",");
        delay(10);
      // T & RH as C & %
        file.print(String(opcData.T_C) + ",");
        delay(10);
        file.print(String(opcData.RH) + ",");
        delay(10);
      // T & RH signal only 
        file.print(String(opcData.signal_temp) + ",");
        delay(10);
        file.print(String(opcData.signal_relhum) + ",");
        delay(10);
      // MToF in 1/3 microseconds 
        for(int j = 0; j < 4; j++){
          file.print(String(opcData.MToF[j]) + ",");
          delay(10);
        }
      // glitch & checksum values
        file.print(String(opcData.reject[0]) + ",");
        delay(10);
        file.print(String(opcData.reject[1]) + ",");
        delay(10);
        file.print(String(opcData.checksum) + ",");
        delay(10);
        file.print(String(opcData.verifycheck));
        delay(10);
      #else //OPC_ENABLED
        file.print(",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
        delay(10);
      #endif //OPC_ENABLED
      file.sync();
      file.close();
      delay(1000);
      digitalWrite(GREEN_LED, LOW);
    } //if sd begins successfully
    digitalWrite(SD_CSPIN, HIGH);
  #endif //SDCARD_LOG_ENABLED
}
