#ifndef COMMAND_IDS_H
#define COMMAND_IDS_H


// This defines the various values for the commands so that they can be changed here
// instead of being hard coded into the source elswhere

// Ground <---> Guider Commands and messages


typedef enum {
  GG_OVERMAXCNT = 15,
  GG_OVERMAXTHR = 16,
  GG_TOGGLEALGORITHM = 17,
  GG_SPOTRMS = 18,
  GG_BOXSIZE = 19,
  
  GG_REBOOTGUIDER 	=	140,
  GG_RESTOREDISPLAY =              141,
  GG_SHUTDOWNGUIDER 	=	142,
  
  GG_GUIDERSTATUS 	=	150,
  GG_TEXTMESSAGE		=	151,
  
  GG_SWITCHINGGUIDER = 160,
  GG_SWITCHINGDTU = 161,
  GG_SWITCHTOLARGE = 162,
  GG_SWITCHTOSMALL = 163,
    GG_WINDOWTRACK = 164,
    GG_SMALLTIME = 165,
    GG_LARGETIME = 166,
    GG_RESETMODE = 167,
    GG_STARTSAVING = 168,
    GG_STOPSAVING = 169,
    GG_FEELLUCKY = 11,
    GG_DITHERPATTERN = 12,
    GG_SWITCHZOOMTYPE = 14,
    GG_INITMODETC 		=	200,
    GG_AUTOCOLLIMATIONTC =	201,
    GG_ALIGNMENTTC 		=	202,
    GG_SLEWINGTC		=	203,
    GG_ELCEOFFSETTC		=	204,
    GG_ALIGNTRACKTC		=	205,
    GG_AUTOCOLLIMATIONXYOFFSETS = 206,
    GG_AUTOCOLLIMATIONZOOMXY = 207,
  
    GG_SETROITC			=	210,
    GG_SETTRIGGERPERIODTC	=	211,
    GG_CAMNORMGAINTC	=	212,
    GG_CAMABSOLUTEOFFSETTC =	213,
    
    GG_TOGGLEACZOOM = 214,
    GG_SETDISPLAYCONTROLSTC =	215,
    // GG_FRAMEUPDATEPERIODTC =	216,
    // GG_DISPLAYFORMATTC 	=	217,
    // GG_IMAGESTACK		=	218,
  
    GG_NUDGETRACKING = 219,
  GG_OFFSETSANDGAINS    = 220,
    
    GG_FBGAIN = 221,
    GG_FBSATVAL = 222,
    GG_FBTHRESHOLD = 223,
    GG_FBDISTTOL = 224,
    GG_FBGRID = 225,
    GG_FBCENTBOX = 226,
    GG_FBAPBOX = 227,
    GG_FBREADOUTOFFSET = 228,
    GG_FBREADOUTNOISE = 229,
    
    GG_FOCUSTC	       =       	230,
    GG_HOMECAMERA		=	231,
    GG_TURNONLEDTC		=	232,
    GG_TURNOFFLEDTC		=	233,
    GG_TAGLOG = 234,
    GG_SETPLATESCALETC	=	235,
    GG_RESETPLATESCALETC =	236,
    GG_SETROTATIONTC	=	237,
    GG_DACOUTPUT = 238,
    
    
    GG_TRACKBLOB = 239,
    GG_INFOBLOB = 240,
    GG_NEXTBLOB = 241,
    GG_TRACKINGPOINT = 242,
    GG_MOVESTEPSIZE = 243,
    
    GG_LOCKEDFC = 245,
    GG_LOCKEDROI = 246,
    GG_GUIDINGFC = 247,
    GG_GUIDINGROI = 248,
    
    GG_TURNOFFMPF = 250,
    GG_TURNONMPF = 251,
    
    GG_ADJUSTOFFSET = 249,
    
    GG_RESETPROGRAM = 254,
    GG_RESTARTPROGRAM = 255,
    // Guider <--> MPF Commands and messages
    
  
    GM_GUIDERPOINTINGERRORS	= 1,
    GM_AUTOCOLLIMATIONREQ =	2,
    GM_SWITCHGUIDERREQ	=	3,
    GM_SWITCHDTUREQ		=	4,
    GM_SLEWINGREQ		=	6,
    GM_ELCEOFFSETREQ	=	7,
    GM_GUIDERSTATUSDATA	=	8,
    GM_CENTERROIREQ		=	9,
  GM_AUTOCOLLIMATIONTC=	100,
  GM_SLEWINGTC		=	101,
  GM_ALIGNMENTTC		=	102,
  GM_ELCEOFFSETTC		=	103,
  
  
  MG_DTUPOINTINGERROR	=	1,
  MG_DTUCOORDINATEDATA=	2,
  MG_ELCEPOSITIONDATA	=	3,
  MG_MPFSTATUSDATA	=	4
  
} commandID;

#endif
