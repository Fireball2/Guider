timespec tim;
tim.tv_sec = 0;
tim.tv_nsec = 10*1000000; //10ms

unsigned int loopcounter = 0;
GuiderMode modeID;

// three booleans to figure out if particular delay has been reached.
bool shortdelay = false;
bool mediumdelay = false; 
bool longdelay = false;
bool quartersec = false;
bool halfsec = false;
bool sec = false;
bool fivesec = false;

#define SHORTDELAY 1 // 20ms
#define MEDIUMDELAY 2 //50ms
#define LONGDELAY 3 //100ms;
#define QUARTERSEC 12 //250ms;
#define HALFSEC 25 //500ms
#define SEC 50 //1000ms
#define FIVESEC 255 //5000ms

modeID = camera.getModeID();
