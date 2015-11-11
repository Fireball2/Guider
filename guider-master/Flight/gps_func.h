#ifndef GPS_FUNC_H_
#define GPS_FUNC_H_
/**
Edited 10/9/06
to remove some of the unnecessary fluff
**/


/* gps_func.h Copyright 2005, WinSystems Inc. All Rights reserved */
/***************************************************************
*
*   Name    : gps_func.h
*
*   Project : PCM-GPS
*
*   Purpose : Sample NMEA Decoding Routines
*
*	Revision: 1.00
*
*   Date    : February 8, 2005
*
*	Author	: Steve Mottin
*
***************************************************************
*
*	Changes :
*
*   Revision        Date          Description
*   --------        ----------    ------------------------------
*   1.00            28/08/2005    Created
*
****************************************************************
*/



/* GPS Global variables */

/* These are from the ZDA message */

extern float gps_utc_time;
extern int   gps_day;
extern int   gps_month;
extern int   gps_year;

/* These are added by the GGA  message */

extern float gps_latitude;
extern char  gps_lat_reference;
extern float gps_longitude;
extern char  gps_long_reference;
extern int   gps_quality;
extern int   gps_satellite_count;
extern float gps_hdop;
extern float gps_altitude;
extern char  gps_altitude_unit;
extern float gps_separation;
extern char  gps_separation_unit;
extern float gps_differential_age;
extern int   gps_differential_station_id;

/* These are added by the GLL message */

extern char gps_status;
extern char gps_mode_indicator;

/* These are added by the GSA message */

extern char gps_op_mode;
extern char gps_fix_mode;
extern int  gps_satellites_in_use[12];
extern float gps_pdop;
extern float gps_vdop;

/* These are added by the GSV message */

extern int gps_gsv_message_count;
extern int gps_gsv_message_number;
extern int gps_total_sats_in_view;

extern int gps_prn_number[3][4];
extern int gps_elevation[3][4];
extern int gps_azimuth[3][4];
extern int gps_snr[3][4];

/* These are added by the RMC message */

extern char gps_rmc_status;
extern float gps_sog;
extern float gps_track;
extern long gps_rmc_date;
extern float gps_mag_variation;
extern char gps_variation_direction;
extern char gps_sys_mode_indicator;

/* These are added by the VTG message */

extern float gps_track_magnetic;
extern float gps_sog_kilometers;

int parse_nmea(char *string);
char *my_token(char *source,char token);
void dummyfunction();

#endif
