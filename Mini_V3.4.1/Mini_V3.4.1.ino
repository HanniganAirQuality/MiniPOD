/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    Mini_V3.4.1.ino
 * @version Percy's 3.4.1
 * @brief   Integrates updated OPC-R2 firmware written as OPC-R2.h & .cpp
 *
 * @author 	Percy Smith
 * @date 	  January 15, 2025
 * @log     Changes .print statements to use F() & decrease memory
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
int Y, M, D;
#endif
#if CROSSTALK_ON
#include <SoftwareSerial.h>
SoftwareSerial Serial1(7,2);
#endif

#if OPC_ENABLED
#include "OPC-R2.h"
OPC opc;
histogramData opcData;
bool updatedData = false;

    float PM1_0_ENV;
    float PM2_5_ENV;
    float PM10_0_ENV;
    char SCALED_PM1[10];
    char SCALED_PM25[10];
    char SCALED_PM10[10];
#endif

void setup() 
{
#if SERIAL_LOG_ENABLED
  Serial.begin(9600);
  Serial.println(F("Initialized Serial Monitor"));
#endif
#if CROSSTALK_ON
Serial1.begin(9600);
#endif

  SPI.begin();

#if OPC_ENABLED
  opc.begin();  // initializes CSpin as output & turns on the opc fan and laser
  if (!opc.begin()) {
#if SERIAL_LOG_ENABLED
    Serial.println(F("Error: Failed to initialize OPC!"));
#endif
  }
  opc.on();
  digitalWrite(RED_LED, HIGH);
  Serial.println(F("OPC on!"));
  digitalWrite(OPC_CSPIN, HIGH);  // just in case this closes SPI OPC line
#endif

  // i think this is for our i2c with the RTC but who knows lol
  Wire.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(EXT_GREEN_LED, OUTPUT);
  pinMode(EXT_RED_LED, OUTPUT);

// this is unnecessary in this firmware, but we're just checking that it's init at least
#if RTC_ENABLED
  if (!rtc.begin()) {
#if SERIAL_LOG_ENABLED
    Serial.println(F("Error: Failed to initialize RTC module"));
#endif
  } else {
    rtc_date_time = rtc.now();
  }
#endif  //RTC_ENABLED
#if SDCARD_LOG_ENABLED
  pinMode(SD_CSPIN, OUTPUT);
  digitalWrite(SD_CSPIN, LOW);
  sd.begin(SD_CSPIN);
  while (!sd.begin(SD_CSPIN)) {
#if SERIAL_LOG_ENABLED
    Serial.println(F("SD Initialization Failed"));
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(EXT_GREEN_LED, LOW);
    digitalWrite(EXT_RED_LED, HIGH);
    digitalWrite(RED_LED, LOW);
#endif
    sd.begin(SD_CSPIN);
  }
  DateTime now = rtc.now();  //pulls setup() time so we have one file name per run in a day
  Y = now.year();
  M = now.month();
  D = now.day();
  sprintf(fileName, "%s_%04u_%02u_%02u.CSV", "M11", Y, M, D);  //char array for fileName
  delay(100);
  file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create permissions
  file.close();                                       //close file, we opened so loop() is faster
  digitalWrite(SD_CSPIN, HIGH);                       //release chip select on SD - allow other comm with SPI
  digitalWrite(GREEN_LED, LOW);                       //turn off green LED (file is closed)
  digitalWrite(EXT_GREEN_LED, LOW);
#endif

  delay(1000);
}

void loop() {
  // CYCLE 1 BEGINS (Serial) **********************************************************
  #if OPC_ENABLED
    digitalWrite(OPC_CSPIN, LOW);
    if (!opc.begin()) {
      digitalWrite(RED_LED, LOW);
      Serial.println(F("No Find OPC"));
      opc.begin();
    } else {
      opcData = opc.histogramFormatted();
      // Serial.println("Polled for histogram");
      digitalWrite(RED_LED, HIGH);
    }
    digitalWrite(OPC_CSPIN, HIGH);
  #endif  //OPC_ENABLED

  #if SERIAL_LOG_ENABLED
    if (!Serial) {         //check if Serial is available... if not,
      Serial.end();        // close serial port
      delay(100);          //wait 100 millis
      Serial.begin(9600);  // reenable serial again
    } else {
  #if RTC_ENABLED
      rtc_date_time = rtc.now();
      Serial.print(rtc_date_time.timestamp());
      Serial.print(F(","));
  #endif
  #if OPC_ENABLED  // this is all the serial printing for testing
      for (int i = 0; i < 16; i++) {
        // Serial.print(F("Bin "));
        // Serial.print(i);
        // Serial.print(F(": "));
        Serial.print(opcData.bin[i]);
        Serial.print(F(","));
        delay(10);
      }
      // Serial.print(F("\n"));
      for (int j = 0; j < 4; j++) {
        // Serial.print(F("MToF "));
        // Serial.print(j*2+1);
        // Serial.print(F(": "));
        Serial.print(opcData.MToF[j]);
        Serial.print(F(","));
        delay(10);
      }
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("Byte T: "));
      // Serial.print(opcData.rawComms[41]);
      // Serial.print(opcData.rawComms[40]);
      // Serial.print(F(","));
      // Serial.print(F("T: "));
      Serial.print(opcData.T_C);
      Serial.print(F(","));
      // Serial.print(F("("));
      // Serial.print(opcData.signal_temp);
      // Serial.print(F("),"));
      delay(10);
      // Serial.print(F("Byte RH: "));
      // Serial.print(opcData.rawComms[43]);
      // Serial.print(opcData.rawComms[42]);
      // Serial.print(F(","));
      // Serial.print(F("Percent RH: "));
      // Serial.print(opcData.RH);
      // Serial.print(F("("));
      Serial.print(opcData.signal_relhum);
      Serial.print(F(","));
      // Serial.print(F("),"));
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("Sample Flow Rate: "));
      Serial.print(opcData.sampleflowrate);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("Sample Period: "));
      Serial.print(opcData.samplingperiod);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("Reject Glitch: "));
      Serial.print(opcData.reject[0]);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("Reject Long: "));
      Serial.print(opcData.reject[1]);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("PM_A (PM1.0): "));
      Serial.print(opcData.PM_ENV[0]);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("PM_B (PM2.5): "));
      Serial.print(opcData.PM_ENV[1]);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("PM_C (PM10.0): "));
      Serial.print(opcData.PM_ENV[2]);
      Serial.print(F(","));
      delay(10);
      // Serial.print(F("\n"));
      // Serial.print(F("CS: "));
      Serial.print(opcData.checksum);
      Serial.print(F(","));
      // Serial.print(F("VC: "));
      Serial.print(opcData.verifycheck);
      Serial.print(F(","));
  #endif
    }
    Serial.print(F("\n"));
  #endif  //SERIAL_LOG_ENABLED
    // updatedData = false;

    // CYCLE 2 BEGINS (SD) **********************************************************
  #if OPC_ENABLED
    digitalWrite(OPC_CSPIN, LOW);
    if (!opc.begin()) {
      digitalWrite(RED_LED, LOW);
      Serial.println(F("Cannot find OPC"));
      updatedData = false;
      opc.begin();
    } else {
      opcData = opc.histogramFormatted();
      // Serial.println("Polled for histogram");
      digitalWrite(RED_LED, HIGH);
      updatedData = true;
      
      PM1_0_ENV = opcData.PM_ENV[0];
      dtostrf(PM1_0_ENV, 3, 2, SCALED_PM1);
      PM2_5_ENV = opcData.PM_ENV[1];
      dtostrf(PM2_5_ENV, 3, 2, SCALED_PM25);
      PM10_0_ENV = opcData.PM_ENV[2];
      dtostrf(PM10_0_ENV, 3, 2, SCALED_PM10);
      // Serial.print(F("\n"));
      // Serial.print(PM1_0_ENV);
      // Serial.print(F(","));
      // Serial.print(PM2_5_ENV);
      // Serial.print(F(","));
      // Serial.print(PM10_0_ENV);
      // Serial.print(F(","));
    }
    digitalWrite(OPC_CSPIN, HIGH);
    delay(100);
  #endif  //OPC_ENABLED

  #if SDCARD_LOG_ENABLED
    digitalWrite(SD_CSPIN, LOW);
    delay(100);
    while (!sd.begin(SD_CSPIN)) {
      // this error changes the LED on the outside of the POD to red when the SD card fails to be found
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(EXT_GREEN_LED, LOW);
      digitalWrite(EXT_RED_LED, HIGH);
  #if SERIAL_LOG_ENABLED
      Serial.println(F("Issues opening SD in loop"));
  #endif
      sd.begin(SD_CSPIN);
    }

    if (sd.begin(SD_CSPIN)) {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(EXT_GREEN_LED, HIGH);
      digitalWrite(EXT_RED_LED, LOW);
  #if RTC_ENABLED
      DateTime now = rtc.now();  //pulls setup() time so we have one file name per run in a day
      Y = now.year();
      M = now.month();
      D = now.day();
      sprintf(fileName, "%s_%04u_%02u_%02u.CSV", "M11", Y, M, D);  //char array for fileName
  #endif
      delay(100);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
  #if RTC_ENABLED
      rtc_date_time = rtc.now();
      file.print(F("\r\n"));
      file.print(rtc_date_time.timestamp());
      file.print(F(","));
  #else
      file.print(F(","));
  #endif

  #if OPC_ENABLED
      // Bins 0 - 15
      for (int i = 0; i < 16; i++) {
        file.print(opcData.bin[i]);
        file.print(F(","));
        delay(10);
      }
      // PM in ug/m^3
      file.print(opcData.PM_ENV[0]);
      file.print(F(","));
      delay(10);
      file.print(opcData.PM_ENV[1]);
      file.print(F(","));
      delay(10);
      file.print(opcData.PM_ENV[2]);
      file.print(F(","));
      delay(10);
      // sampling information
      file.print(opcData.sampleflowrate);
      file.print(F(","));
      delay(10);
      file.print(opcData.samplingperiod);
      file.print(F(","));
      delay(10);
      // T & RH as C & %
      file.print(opcData.T_C);
      file.print(F(","));
      delay(10);
      file.print(opcData.RH);
      file.print(F(","));
      delay(10);
      // T & RH signal only
      file.print(opcData.signal_temp);
      file.print(F(","));
      delay(10);
      file.print(opcData.signal_relhum);
      file.print(F(","));
      delay(10);
      // MToF in 1/3 microseconds
      for (int j = 0; j < 4; j++) {
        file.print(opcData.MToF[j]);
        file.print(F(","));
        delay(10);
      }
      // glitch & checksum values
      file.print(opcData.reject[0]);
      file.print(F(","));
      delay(10);
      file.print(opcData.reject[1]);
      file.print(F(","));
      delay(10);
      file.print(opcData.checksum);
      file.print(F(","));
      delay(10);
      file.print(opcData.verifycheck);
      delay(10);
  #else   //OPC_ENABLED
      file.print(F(",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,"));
      delay(10);
  #endif  //OPC_ENABLED
      file.sync();
      file.close();
      delay(1000);
      digitalWrite(GREEN_LED, LOW);
    }  //if sd begins successfully
    digitalWrite(SD_CSPIN, HIGH);
  #endif  //SDCARD_LOG_ENABLED
    // CYCLE 3 BEGINS (SoftwareSerial) **********************************************************
  #if CROSSTALK_ON
    // bool hasReceivedRequest = false;
    if (updatedData)
    {
      if (Serial1.available()) {
        String megaRequestToUno = Serial1.readStringUntil('\n');
        delay(100);
        megaRequestToUno.trim();

        if (megaRequestToUno == "Mega requesting data from Uno") {
          #if DEBUG_CROSSTALK
            Serial.print(F("Mega Message:"));
            Serial.print(megaRequestToUno);
          #endif
          char unoResponse[128];
          snprintf(unoResponse, sizeof(unoResponse), "A%s,%s,%s\n",
                  SCALED_PM1, SCALED_PM25, SCALED_PM10);  // Remove JSON formatting & move to \n
          delay(100);
          Serial1.print(unoResponse);
          Serial1.flush();
          #if DEBUG_CROSSTALK
            Serial.print(F("\n"));
            Serial.print(unoResponse);
          #endif //DEBUG_CROSSTALK
        }
      }
      updatedData = false;
    } //if (updatedData)
  #endif  //CROSSTALK_ON
}
