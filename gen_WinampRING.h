#ifndef gen_WinampRING_h 
#define gen_WinampRING_h

//functions
int init(void);
void config(void);
void quit(void);
BOOL CALLBACK ConfigProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void checkMonitorPlay();
void doAction(char *callername, char *callernum);
void openMixer();
void closeMixer();
void changeWinVol(int newVol);
void showErr();
void openComPort();
void closeComPort();
void openSipPort();
void closeSipPort();
int getWinVol();
unsigned __stdcall monitorPort();
unsigned __stdcall monitorPlay();
unsigned __stdcall monitorSipPort();
BOOL WriteComm(char* toWrite);
BOOL ReadComm(char *readStr);

//for WinSock
WSADATA wsaData;
SOCKET listenSocketUDP = INVALID_SOCKET;
SOCKET clientSocket = INVALID_SOCKET;

// Settings and their defaults
int volume=25;				//
int comPort=1;				//
BOOL listenOnModem=TRUE;    //If FALSE, then we're listening on SIP
char sipPort[6]="5060";		//The port to listen on for SIP
char *lastSipCallId=NULL;	//The Call-ID of the last SIP INVITE
BOOL percent=FALSE;			//
BOOL popup=FALSE;			//Properties you see in the config window
BOOL chWinVol=FALSE;		//
BOOL enabled=TRUE;			//
BOOL mixOpen=FALSE;			//
BOOL stop=FALSE;			//for monitor play thread
BOOL cancelExit=FALSE;		//to cancel exit of config when an error occurs
BOOL unresolvedError=FALSE;	//if error has not been resolved
BOOL badPort=FALSE;			//if current port number is bad
BOOL callerId=FALSE;		//enable caller ID or not
BOOL popupOpen=FALSE;		//TRUE when a popup box is open
char callIdInit[50]="AT#CID=1\r"; //code to enable caller ID on the modem
char initStr[50]="ATZ\r";		//Initialization string
long action=IPC_SETVOLUME;
time_t oldTime;				//last time doAction was called
char lastErr[100];			//last error that occurred
char *readme;				//path and filename of readme
unsigned int threadID;		//ID for monitorPort thread
unsigned int threadID1;		//ID for monitorPlay thread


HANDLE hComm=INVALID_HANDLE_VALUE;		//handle of COM port
HANDLE hPortMonThread=INVALID_HANDLE_VALUE;	//handle of monitorPort thread
HANDLE hEndPortMonThread=INVALID_HANDLE_VALUE;	//event to kill above thread
HANDLE hThread1=INVALID_HANDLE_VALUE;	//handle of monitorPlay thread
HANDLE hEndPlayMonThread=INVALID_HANDLE_VALUE;	//event to kill above thread
HANDLE hFakeDoAction=INVALID_HANDLE_VALUE;	//event to trigger a fake ring -- for debugging

OVERLAPPED overlapped;


HMIXER hMixer=0;			//structures needed for accessing mixer
MIXERLINE mxl={0};
MIXERLINECONTROLS mxlc={0};
MIXERCONTROL volCon={0};
MIXERCONTROL muteCon={0};
MIXERCONTROLDETAILS mxcd;
MIXERCONTROLDETAILS_UNSIGNED mxcdu;

//WNDPROC lpWndProcOld;

typedef struct {
  int version;
  char *description;
  int (*init)();
  void (*config)();
  void (*quit)();
  HWND hwndParent;
  HINSTANCE hDllInstance;
} winampGeneralPurposePlugin;

#define GPPHDR_VER 0x10

extern winampGeneralPurposePlugin *gen_plugins[256];
typedef winampGeneralPurposePlugin * (*winampGeneralPurposePluginGetter)();

#endif
