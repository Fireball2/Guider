//#define DEBUG
/**
October 9, 2006.
Trimmed to remove unnecessary functions. Leaving only the token
reader and the message parser (and appropriate variables and 
include headers.
**/


/* gps_func.c Copyright 2005, WinSystems Inc. All Rights reserved */
/***************************************************************
*
*   Name    : gps_func.c
*
*   Project : PCM-GPS
*
*   Purpose : Sample NMEA 0183 Decoding Routines
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


#include <QThread>
#include <QMutex>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "gps_func.h"


#include <mcheck.h>
#define MEMCHECK

#define BUFFER_SIZE 32768

/* GPS Global variables */

/* These are derived from the ZDA message */


float gps_utc_time;
int   gps_day;
int   gps_month;
int   gps_year;

/* These are added by the GGA  message */

float gps_latitude;
char  gps_lat_reference;
float gps_longitude;
char  gps_long_reference;
int   gps_quality;
int   gps_satellite_count;
float gps_hdop;
float gps_altitude;
char  gps_altitude_unit;
float gps_separation;
char  gps_separation_unit;
float gps_differential_age;
int   gps_differential_station_id;

/* These are added by the GLL message */

char gps_status;
char gps_mode_indicator;

/* These are added by the GSA message */

char gps_op_mode;
char gps_fix_mode;
int  gps_satellites_in_use[12];
float gps_pdop;
float gps_vdop;

/* These are added by the GSV message */

int gps_gsv_message_count;
int gps_gsv_message_number;
int gps_total_sats_in_view;

int gps_prn_number[3][4];
int gps_elevation[3][4];
int gps_azimuth[3][4];
int gps_snr[3][4];

/* These are added bu the RMC message */

char gps_rmc_status;
float gps_sog;
float gps_track;
long gps_rmc_date;
float gps_mag_variation;
char gps_variation_direction;
char gps_sys_mode_indicator;

/* These are added by the VTG message */

float gps_track_magnetic;
float gps_sog_kilometers;


/* Local function proto-types */

int parse_nmea(char *string);
char *my_token(char *source,char token);


char *field[255];


/* This function accepts a string believed to contain standard NMEA 0183 sentence
   data and parses those fields and loads the appriate global variables with the results.
*/

int parse_nmea(char *string)
{
#ifdef MEMCHECK
mtrace();
#endif
int field_count;
int x,y;

    field_count = 0;

#ifdef DEBUG
    printf("Parsing NMEA string : <%s>\n",string);
#endif

    /* NMEA 0183 fields are delimited by commas. The my token function returns
    pointers to the fields.
    */

    /* Get the first field pointer */

    field[0] = my_token(string,',');
#ifdef DEBUG
    if(field[0])
        printf("Token = <%s>\n",field[0]);
#endif

    field_count++;
    while(1)
    {
        /* Contiue retrieving fields until there are no more (NULL) */

        field[field_count] = my_token(NULL,',');
        if(field[field_count] == NULL)
            break;
#ifdef DEBUG
        printf("Token = <%s>\n",field[field_count]);
#endif

        field_count++;
    }

#ifdef DEBUG
    printf("%d fields parsed\n",field_count);
#endif

    /* If we got at least ONE field */

    if(field_count)
    {
        /* Check the first field for the valid NMEA 0183 headers */

        if(strcmp(field[0],"$GPGGA") == 0)
        {
            /* Retrive the values from the remaining fields */
	  

	  //gps_utc_time = atof(field[1]);
	  gps_latitude = atof(field[2]);
	  gps_lat_reference = *(field[3]);
	  gps_longitude = atof(field[4]);
	  gps_long_reference = *(field[5]);
	  gps_quality = atoi(field[6]);
	  gps_satellite_count = atoi(field[7]);
	  gps_hdop = atof(field[8]);
	  gps_altitude = atof(field[9]);
	  gps_altitude_unit = *(field[10]);
	  gps_separation = atof(field[11]);
	  gps_separation_unit = *(field[12]);
	  gps_differential_age = atof(field[13]);
	  gps_differential_station_id = atoi(field[14]);

#ifdef DEBUG
            printf("NMEA string GPGGA recognized\n");
            printf("Time = %9.2f\n",gps_utc_time);
            printf("Position : %8.3f %c %8.3f %c\n",gps_latitude,gps_lat_reference,
		   gps_longitude,gps_long_reference);
	    
            printf("GPS quality = %d, Satelite count = %d, HDOP = %4.2f\n",gps_quality,
		   gps_satellite_count, gps_hdop);
	    
            printf("GPS altitude = %9.2f %c, Geoidal Separation = %9.2f %c\n",gps_altitude,
		   gps_altitude_unit, gps_separation, gps_separation_unit);
	    
            printf("GPS differential update age = %9.2f.Station ID = %d\n",gps_differential_age,
		   gps_differential_station_id);
	    
#endif
	    
        }
	
        if(strcmp(field[0],"$GPGLL") == 0)
        {
            /* Retrieve the values from the remaining fields */


	  gps_latitude = atof(field[1]);
	  gps_lat_reference = *(field[2]);
	  gps_longitude = atof(field[3]);
	  gps_long_reference = *(field[4]);
	  //	  gps_utc_time = atof(field[5]);
	  gps_status = *(field[6]);
	  gps_mode_indicator = *(field[7]);

#ifdef DEBUG
            printf("NMEA string GPGLL recognized\n");
            printf("Position : %8.3f %c %8.3f %c\n",gps_latitude,gps_lat_reference,
                gps_longitude,gps_long_reference);
            printf("Time = %9.2f\n",gps_utc_time);

            printf("GPS status = %c, GPS mode indicator = %c\n",gps_status,gps_mode_indicator);
#endif

        }



        if(strcmp(field[0],"$GPGSA") == 0)
        {
            /* Retrieve the values from the remaining fields */

	  gps_op_mode = *(field[1]);
	  gps_fix_mode = *(field[2]);
	  gps_pdop = atof(field[15]);
	  gps_hdop = atof(field[16]);
	  gps_vdop = atof(field[17]);

#ifdef DEBUG
            printf("NMEA string GPGSA recognized\n");

            printf("Operation mode = %c, Fix mode = %c\n",gps_op_mode,gps_fix_mode);

            printf("Satelites in use : ");
#endif
            for(x=0; x<12; x++)
            {
                gps_satellites_in_use[x] = atoi(field[x+3]);
#ifdef DEBUG
                if(gps_satellites_in_use[x])
                    printf("%d   ",gps_satellites_in_use[x]);
#endif
            }
#ifdef DEBUG
            printf("\n");

            printf("GPS precision %5.2f PDOP, %5.2f HDOP, %5.2f VDOP\n",
                gps_pdop,gps_hdop,gps_vdop);
#endif
        }

        if(strcmp(field[0],"$GPGSV") == 0)
        {

            /* Retrieve the data from the remaining fields */

            gps_gsv_message_count = atoi(field[1]);
            gps_gsv_message_number = atoi(field[2]);

            gps_total_sats_in_view = atoi(field[3]);

#ifdef DEBUG
            printf("NMEA string GPGSV recognized\n");
            printf("Total satelites in view = %d\n",gps_total_sats_in_view);
#endif        
            if((gps_gsv_message_number > 0) && (gps_gsv_message_number < 4))
            {
                y = gps_gsv_message_number - 1;
                for(x=0; x< 4; x++)
                {
                    gps_prn_number[y][x] = atoi(field[(x*4)+4]);
                    gps_elevation[y][x] = atoi(field[(x*4)+5]);
                    gps_azimuth[y][x] = atoi(field[(x*4)+6]);
                    gps_snr[y][x] = atoi(field[(x*4)+7]);
#ifdef DEBUG
                    printf("Satelite %d - Elev = %d Azim = %d  SNR = %d\n",
                        gps_prn_number[y][x],gps_elevation[y][x],gps_azimuth[y][x],
                        gps_snr[y][x]);
#endif

                }
            }

        }

        if(strcmp(field[0],"$GPRMC") == 0)
        {

            /* Retrieve the data from the remaining fields */

	  //	  gps_utc_time = atof(field[1]);
	  gps_rmc_status = *(field[2]);
	  gps_latitude = atof(field[3]);
	  gps_lat_reference = *(field[4]);
	  gps_longitude = atof(field[5]);
	  gps_long_reference = *(field[6]);
	  gps_sog = atof(field[7]);
	  gps_track = atof(field[8]);
	  gps_rmc_date = atol(field[9]);
	  gps_mag_variation = atof(field[10]);
	  gps_variation_direction = *(field[11]);
	  gps_sys_mode_indicator = *(field[12]);

	  
#ifdef DEBUG
	  printf("NMEA string GPRMC recognized\n");
	  
	  printf("GPS UTC Time = %9.2f. RMC Status = %c\n",gps_utc_time,gps_rmc_status);
	  
	  
	  printf("Position : %7.2f Deg %c  %7.2f Deg %c\n",gps_latitude,gps_lat_reference,
		 gps_longitude,gps_long_reference);

	  
	  printf("GPS Track %7.2f degrees at %7.2f Knot groundspeed\n",gps_track,gps_sog);
	  
	  
	  printf("GPS Date : %8ld Mag Variation %6.2f Deg %c GPS Mode %c\n",
		 gps_rmc_date,gps_mag_variation,gps_variation_direction,gps_mode_indicator);
#endif
        }
	
        if(strcmp(field[0],"$GPVTG") == 0)
	  {
            /* Retrieve the data from the remaining fields */

            gps_track = atof(field[1]);
            gps_track_magnetic = atof(field[3]);
            gps_sog = atof(field[5]);
            gps_sog_kilometers = atof(field[7]);
            gps_sys_mode_indicator = *(field[9]);
	    
#ifdef DEBUG
            printf("NMEA string GPVTG recognized\n");
	    
	    
            printf("GPS Track : %7.2f True %7.2f Magnetic. Speed :  %9.2f Knots %9.2f Kilometer/Hour\n",
		   gps_track,gps_track_magnetic,gps_sog,gps_sog_kilometers);
#endif
	  }
	
        if(strcmp(field[0],"$GPZDA") == 0)
	  {
	    
            /* Retrieve the data from the remaining fields */

	    gps_utc_time = atof(field[1]); //this one
            gps_day = atoi(field[2]);
            gps_month = atoi(field[3]);
            gps_year = atoi(field[4]);

#ifdef DEBUG
            printf("NMEA string GPZDA recognized\n");
            printf("%9.2f %2d/%2d/%4d\n",gps_utc_time,gps_month,gps_day,gps_year);
#endif
        }

    }
    return field_count;
}


/* These variables and the function my_token are used to retrieve the comma
   delimited field pointers from the input string. Repeated calls to
   my_token return the next field until there are no more (NULL).
*/

static char stat_string[1024];
char *current = NULL;

char *my_token(char *source,char token)
{

#ifdef MEMCHECK
mtrace();
#endif
char *start;

    /* The source string is real only for the first call. Subsequent calls
    are made with the source string pointer as NULL
    */

    if(source != NULL)
    {
        /* If the string is empty return NULL */
        if(strlen(source) == 0)
            return NULL;
        strcpy(stat_string,source);
        /* Current is our 'current' position within the string */
        current = stat_string;
    }

    start = current;

    while(1)
    {
        /* If we're at the end of the string, return NULL */

        if((*current == '\0') && (current == start))
            return NULL;

        /* If we're at the ned now, but weren't when we started, we need
           to return the pointer for the last field before the end of string
        */

        if(*current == '\0')
            return start;

        /* If we've located our specified toke (comma) in the string
            load it's location in the copy with an end of string marker
            so that it can be handled correctly by the calling program.
        */

        if(*current == token)
        {
            *current = '\0';
            current++;
            return start;
        }
        else
        {
            current++;
        }
    }
}


