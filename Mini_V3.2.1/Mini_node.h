#include "WString.h"
/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    Mini_node.h
 * @brief   Integrates updated OPC-R2 firmware written as OPC-R2.h & .cpp
 *
 * @author 	Percy Smith
 * @date 	  May 7, 2025
 * @log     UPDATE PODID in Mini_V3.2.1.ino - USE CTRL + F AND REPLACE ALL (ONLY 3 CHAR!!a)
******************************************************************************/
#ifndef _MINI_NODE_H
#define _MINI_NODE_H

// const String MiniID = "M03"; //U GET THREE CHARACTERS THAT IS IT !!!!!

#define SDCARD_LOG_ENABLED    1
#define SERIAL_LOG_ENABLED    1
#define RTC_ENABLED           1
#define OPC_ENABLED           1

// OPC chip select
#define OPC_CSPIN             9
// Digital pins from Adafruit shield
#define SD_CSPIN              10
#define EXT_GREEN_LED         5
#define EXT_RED_LED           6
#define RED_LED               4
#define GREEN_LED             3

// #define STATUS_RUNNING        12
// #define STATUS_ERROR          11
// #define STATUS_HALTED         13

#endif // _MINI_NODE_H
