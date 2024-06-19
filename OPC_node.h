/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    OPC_node.h
 * @brief   Integrates updated OPC-R2 firmware written as OPC-R2.h & .cpp
 *
 * @author 	Percy Smith
 * @date 	  June 19, 2024
 * @log     Should use the LEDs, write all OPC histogram data, RTC Timestamp
******************************************************************************/
#ifndef _OPC_NODE_H
#define _OPC_NODE_H

#define SDCARD_LOG_ENABLED    1
#define SERIAL_LOG_ENABLED    1
#define LED_ENABLED           1 
#define RTC_ENABLED           1
#define OPC_ENABLED           1

// OPC chip select
#define OPC_CSPIN            9
// Digital pins from Adafruit shield
#define SD_CSPIN              10
#define EXT_GREEN_LED         5
#define EXT_RED_LED           6
#define RED_LED               4
#define GREEN_LED             3

// #define STATUS_RUNNING        12
// #define STATUS_ERROR          11
// #define STATUS_HALTED         13

#endif // _XPOD_NODE_H
