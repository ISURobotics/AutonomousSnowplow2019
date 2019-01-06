#pragma once

#include "macro_defs.h"
#include <iostream>
#include <vector>
#include <tuple>
#include <atomic>

using namespace std;

/*---------------------------------------------------
Various project config options - change as necessary.
Certain macro definitions as well as other options 
used here can be found in macro_defs.cpp
---------------------------------------------------*/
#define LIDAR_IN_USE       LMS_511
#define DECAWAVE_COM_PORT  ( "\\\\.\\COM7" )
#define IMU_COM_PORT       ( "\\\\.\\COM4" )
#define MOTOR_COM_PORT     ( "\\\\.\\COM6" )
#define TEMP_COM_PORT      ( "\\\\.\\COM9" )
//to change fields input either SINGLE_I or DOUBLE_I
#define FIELD              DOUBLE_I
#define MAP_RESOLUTION_M   ( 0.10 )
#define MAP_OBJ_THRESH     ( 100 )
#define NAV_POINT_METHOD   LIST
#define NAV_POINT_THRESH_M ( 0.25 )
#define ORI_THRESH_D       ( 5.0 )
#define SPEED_SCALAR       ( 1.00 )

/*---------------------------------------
lidar macros
---------------------------------------*/
#if ( LIDAR_IN_USE == TIM_551 )
	#define LIDAR_IP_ADDR     ( "169.254.185.233" )
	#define LIDAR_PORT        ( 2111 )
	#define LIDAR_START_ANGLE ( -45.0 )
	#define LIDAR_ANG_RES     ( 1.0 )
	#define LIDAR_DATA_POINTS ( 271 )
	#define LIDAR_MSG_VLD     ( "RSSI1" )

#elif ( LIDAR_IN_USE == LMS_511 )
	/*---------------------------------------
	These macros needs to be updated
	with the actual lms 511 info
	---------------------------------------*/
	#define LIDAR_IP_ADDR     ( "169.254.144.5" )
	#define LIDAR_PORT        ( 2112 )
	#define LIDAR_START_ANGLE ( -5.0 )
	#define LIDAR_ANG_RES     ( 0.5 )
	#define LIDAR_DATA_POINTS ( 381 )
	#define LIDAR_MSG_VLD     ( "0" )

#endif

/*---------------------------------------
Field macros - set custom width and
length under field == custom
---------------------------------------*/
#if( FIELD == SINGLE_I )
#define FIELD_WIDTH_M    ( 4.0 )
#define FIELD_LENGTH_M   ( 15.0 )
#elif( FIELD == DOUBLE_I )
#define FIELD_WIDTH_M    ( 7.0 )
#define FIELD_LENGTH_M   ( 15.0 )
#elif( FIELD == CUSTOM )
#define FIELD_WIDTH_M    ( 4.0 )
#define FIELD_LENGTH_M   ( 10.0 )
#endif

/*---------------------------------------
Set these to 1 to compile the timing code
and see output in the console
---------------------------------------*/
#define LIDAR_TIMING ( 0 )
#define MAIN_TIMING  ( 0 )

/*---------------------------------------
Project specific types
---------------------------------------*/
typedef std::vector<tuple<double, double>> lidar_data_packet;

typedef std::vector<std::vector<int>> lidar_hit_map;

typedef uint8_t drive_operation;
enum {
	STRAIGHT = (char)0x0,
	RIGHT    = (char)0x1,
	LEFT     = (char)0x2,
	STOP     = (char)0x3
};

typedef struct {
	drive_operation drive_op;
	unsigned char intensity;
	bool changed;
	unsigned char stop_char;
} drive_data_pkt;

typedef struct {
	double x;
	double y;
	//may add ref for linked list of points
} snowplow_nav_point;