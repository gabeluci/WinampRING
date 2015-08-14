#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <commctrl.h>
#include <process.h>
#include <time.h>
#include "resource.h"
#include "wa_calls.h"
#include "gen_WinampRING.h"

//this gets sent to Winamp
winampGeneralPurposePlugin plugin = {
  GPPHDR_VER,
  "WinampRING v1.5 [gen_WinampRING.dll]", // Plug-in description 
  init,
  config,
  quit,
};

//called when Winamp starts
int init() {
	HKEY reg;
	DWORD lenVal;

	if (RegOpenKeyEx(HKEY_CURRENT_USER,		//open HKEY_CURRENT_USER\Software\WinampRING
		"Software\\WinampRING", 0, KEY_READ,
		&reg) == ERROR_SUCCESS) {									//get all the settings from the registry
		RegQueryValueEx(reg, "Volume", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "Volume", 0, 0, (LPBYTE)&volume, &lenVal);
			if (volume > 100) volume = 100;
			if (volume < 0) volume = 0;
		}

		RegQueryValueEx(reg, "ListenOnModem", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "ListenOnModem", 0, 0, (LPBYTE)&listenOnModem, &lenVal);
		}

		RegQueryValueEx(reg, "COMPort", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "COMPort", 0, 0, (LPBYTE)&comPort, &lenVal);
		}

		RegQueryValueEx(reg, "Percent", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "Percent", 0, 0, (LPBYTE)&percent, &lenVal);
		}

		RegQueryValueEx(reg, "Popup", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "Popup", 0, 0, (LPBYTE)&popup, &lenVal);
		}

		RegQueryValueEx(reg, "ChWinVol", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "ChWinVol", 0, 0, (LPBYTE)&chWinVol, &lenVal);
		}

		RegQueryValueEx(reg, "Enabled", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "Enabled", 0, 0, (LPBYTE)&enabled, &lenVal);
		}

		RegQueryValueEx(reg, "Action", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "Action", 0, 0, (LPBYTE)&action, &lenVal);
		}

		RegQueryValueEx(reg, "CallerID", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "CallerID", 0, 0, (LPBYTE)&callerId, &lenVal);
		}

		RegQueryValueEx(reg, "CallerIDInit", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "CallerIDInit", 0, 0, (LPTSTR)callIdInit, &lenVal);
		}

		RegQueryValueEx(reg, "InitString", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "InitString", 0, 0, (LPTSTR)initStr, &lenVal);
		}

		RegQueryValueEx(reg, "SIPPort", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			RegQueryValueEx(reg, "SIPPort", 0, 0, (LPTSTR)sipPort, &lenVal);
		}

		RegQueryValueEx(reg, "InstallPath", 0, 0, NULL, &lenVal);
		if (lenVal > 0) {
			readme = malloc(sizeof(char)*(lenVal + 30));
			RegQueryValueEx(reg, "InstallPath", 0, 0, (LPTSTR)readme, &lenVal);
			strcat_s(readme, lenVal + 30, "\\Plugins\\Readme_WinampRING.txt");
		}

		switch (action) {
		case IPC_SETVOLUME:
		case WINAMP_BUTTON3:
		case WINAMP_BUTTON4:
			break;
		default:	//if action is invalid value, set to default
			action = IPC_SETVOLUME;
		}

		RegCloseKey(reg);
	} else {
		strcpy_s(lastErr, 100, "WinampRING could not access registry. Check access permissions.\nDefault settings loaded.");
	}

	time(&oldTime); //put a value in oldTime
	oldTime -= 20;  //and subtract 20 seconds so we can react to rings right away

	if (chWinVol && enabled)
		openMixer();	//open mixer if necessary
	checkMonitorPlay();	//start monitorPlay thread if necessary
	//lpWndProcOld = (WNDPROC)SetWindowLong(plugin.hwndParent,GWL_WNDPROC,(LONG)WndProc);

	return 0;
}

//called when Winamp opens config
void config() {		//opens config window
	DialogBox(plugin.hDllInstance, MAKEINTRESOURCE(IDD_WRDIALOG), plugin.hwndParent, ConfigProc);
}

//called when Winamp closes
void quit() {
	HKEY reg;
	DWORD r;
	if (RegCreateKeyEx(HKEY_CURRENT_USER,		//opens HKEY_CURRENT_USER\Software\WinampRING
		"Software\\WinampRING", 0,
		NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &reg, &r) == ERROR_SUCCESS) {											//write all the values back to the registry
		RegSetValueEx(reg, "Volume", 0, REG_DWORD, (LPBYTE)&volume, (DWORD) sizeof(volume));

		RegSetValueEx(reg, "ListenOnModem", 0, REG_DWORD, (LPBYTE)&listenOnModem, (DWORD) sizeof(listenOnModem));

		RegSetValueEx(reg, "COMPort", 0, REG_DWORD, (LPBYTE)&comPort, (DWORD) sizeof(comPort));

		RegSetValueEx(reg, "Percent", 0, REG_DWORD, (LPBYTE)&percent, (DWORD) sizeof(percent));

		RegSetValueEx(reg, "Popup", 0, REG_DWORD, (LPBYTE)&popup, (DWORD) sizeof(popup));

		RegSetValueEx(reg, "ChWinVol", 0, REG_DWORD, (LPBYTE)&chWinVol, (DWORD) sizeof(chWinVol));

		RegSetValueEx(reg, "Enabled", 0, REG_DWORD, (LPBYTE)&enabled, (DWORD) sizeof(enabled));

		RegSetValueEx(reg, "Action", 0, REG_DWORD, (LPBYTE)&action, (DWORD) sizeof(action));

		RegSetValueEx(reg, "CallerID", 0, REG_DWORD, (LPBYTE)&callerId, (DWORD) sizeof(callerId));

		RegSetValueEx(reg, "InitString", 0, REG_SZ, (LPBYTE)initStr, (DWORD)strlen(initStr) + 1);

		RegSetValueEx(reg, "CallerIDInit", 0, REG_SZ, (LPBYTE)callIdInit, (DWORD)strlen(callIdInit) + 1);

		RegSetValueEx(reg, "SIPPort", 0, REG_SZ, (LPBYTE)sipPort, (DWORD)sizeof(sipPort));

		RegCloseKey(reg);
	}
	free(readme);	//free space used for readme path
	if (mixOpen)	//close mixer if it was open
		closeMixer();
	//SetWindowLong(plugin.hwndParent, GWL_WNDPROC, (LONG)lpWndProcOld);
	enabled = FALSE;		//disable
	checkMonitorPlay();	//and stop monitorPlay thread
	closeComPort();		//close COM port if it's open
}

//shows the last error, if one exists
void showErr(HWND hwndPrnt) {
	unresolvedError = FALSE;
	if (strlen(lastErr) > 0) {
		MessageBox(hwndPrnt, lastErr, "WinampRING", MB_ICONEXCLAMATION | MB_OK);
		cancelExit = TRUE;
		unresolvedError = TRUE;	//raise flag if there is an error
		strcpy_s(lastErr, 100, "");		//clear the error
	}
}

//starts or stops a thread to monitor whether Winamp is playing or not
void checkMonitorPlay() {
	if (enabled && hThread1 == INVALID_HANDLE_VALUE) {
		stop = FALSE;		//start thread
		hThread1 = (HANDLE)_beginthreadex(NULL, 0, (void *)&monitorPlay, NULL, 0, &threadID1);
	} else if (!enabled && hThread1 != INVALID_HANDLE_VALUE) {
		stop = TRUE;		//flag the thread to stop
		WaitForSingleObject(hThread1, INFINITE);	//wait for it to stop
		CloseHandle(hThread1);		//tie loose ends
		hThread1 = INVALID_HANDLE_VALUE;
	}
}

//loop to monitor whether Winamp is playing or not
unsigned __stdcall monitorPlay() {
	while (!stop && !badPort) {
		if (SendMessage(plugin.hwndParent, WM_USER, 0, IPC_ISPLAYING) == 1) {
			if (listenOnModem) {
				openComPort();		//if playing, open port if it's not open
			} else {
				openSipPort();
			}
		} else {
			if (listenOnModem) {
				closeComPort();	//if paused or stopped, close port if it's open
			} else {
				closeSipPort();
			}
		}
		Sleep(500);			//check only every half second
	}

	if (listenOnModem) {
		closeComPort();	//if paused or stopped, close port if it's open
	} else {
		closeSipPort();
	}
	return 0;
}

void openSipPort() {
	if (listenSocketUDP == INVALID_SOCKET && enabled && !badPort) {

		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		struct addrinfo *result = NULL, *ptr = NULL, hints;

		if (iResult != 0) {
			strcpy_s(lastErr, 100, "WinampRING was unable to initialize the socket.");
			badPort = TRUE;
			return;
		}

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		iResult = getaddrinfo(NULL, sipPort, &hints, &result);
		if (iResult != 0) {
			sprintf_s(lastErr, 100, "WinampRING was unable to get address info for opening the socket, port (%s): %i", sipPort, WSAGetLastError());
			badPort = TRUE;
			WSACleanup();
			return;
		}

		listenSocketUDP = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listenSocketUDP == INVALID_SOCKET) {
			sprintf_s(lastErr, 100, "WinampRING was unable to open the socket: %i", WSAGetLastError());
			badPort = TRUE;
			freeaddrinfo(result);
			listenSocketUDP = INVALID_SOCKET;
			WSACleanup();
			return;
		}

		iResult = bind(listenSocketUDP, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			sprintf_s(lastErr, 100, "WinampRING was unable to bind the socket: %i", WSAGetLastError());
			badPort = TRUE;
			freeaddrinfo(result);
			closesocket(listenSocketUDP);
			listenSocketUDP = INVALID_SOCKET;
			WSACleanup();
			return;
		}

		freeaddrinfo(result);

		//if all successful, start thread to monitor the port
		hEndPortMonThread = CreateEvent(NULL, TRUE, FALSE, NULL);
		hFakeDoAction = CreateEvent(NULL, TRUE, FALSE, NULL);
		hPortMonThread = (HANDLE)_beginthreadex(NULL, 0, (void *)&monitorSipPort, NULL, 0, &threadID);
	}
}

void closeSipPort() {
	if (listenSocketUDP != INVALID_SOCKET) {
		if (hPortMonThread != INVALID_HANDLE_VALUE) {	//if the thread is going, kill it
			//while (popupOpen) { Sleep(100); } //if a popup is open, wait till it's gone
			//Flag thread to die
			SetEvent(hEndPortMonThread);
			//and wait for it to do so
			WaitForSingleObject(hPortMonThread, INFINITE);
			CloseHandle(hPortMonThread);
			hPortMonThread = INVALID_HANDLE_VALUE;

			CloseHandle(hEndPortMonThread);
			hEndPortMonThread = INVALID_HANDLE_VALUE;

			CloseHandle(hFakeDoAction);
			hFakeDoAction = INVALID_HANDLE_VALUE;
		}
		closesocket(listenSocketUDP);
		listenSocketUDP = INVALID_SOCKET;
		WSACleanup();
	}
}

unsigned __stdcall monitorSipPort() {
	HANDLE hArray[3];
	char RecvBuf[1024];
	int BufLen = 1024;
	int iResult = 0;
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	char *s;
	char *temp;
	char *callername = NULL;
	char *callernumber = NULL;
	char *thisCallId = NULL;

	hArray[0] = hEndPortMonThread;	//event to kill thread
	hArray[1] = CreateEvent(NULL, FALSE, FALSE, "UDPEvent");
	WSAEventSelect(listenSocketUDP, hArray[1], FD_READ);
	hArray[2] = hFakeDoAction;	//event to trigger fake ring

	while (TRUE) {
		//sit and wait for ring or signal to die
		switch (WaitForMultipleObjects(3, hArray, FALSE, INFINITE)) {
		case WAIT_OBJECT_0:
			ResetEvent(hArray[0]);
			return FALSE;
			break;
		case WAIT_OBJECT_0 + 1:
			iResult = recvfrom(listenSocketUDP, RecvBuf, BufLen, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
			if (strstr(RecvBuf, "INVITE") == RecvBuf) { //"INVITE" appears first
				s = strstr(RecvBuf, "Call-ID:") + 9;
				temp = strstr(s, "\r");
				thisCallId = malloc(temp - s + 1);
				memcpy(thisCallId, s, temp - s);
				thisCallId[temp - s] = '\0';

				if (lastSipCallId != NULL && strcmp(thisCallId, lastSipCallId) == 0) {
					free(thisCallId); //same call as last time, so ignore
				} else {
					if (lastSipCallId != NULL) {
						free(lastSipCallId);
					}
					lastSipCallId = thisCallId;

					if (callername != NULL) {
						free(callername);
						callername = NULL;
					}
					if (callernumber != NULL) {
						free(callernumber);
						callernumber = NULL;
					}

					s = strstr(RecvBuf, "From:");
					if (s != NULL) {
						s += 6; //advance past "From: "
						if (*s == '"') s++;
						if (s == strstr(s, "sip:")) {
							//no display name
							s += 4;
							temp = strstr(s, "@");
							callernumber = malloc(temp - s + 1);
							memcpy(callernumber, s, temp - s);
							callernumber[temp - s] = '\0';
						} else {
							temp = strstr(s, "<");
							if (temp != NULL) {
								temp--;
								while (*(temp - 1) == ' ' || *(temp - 1) == '\"') {
									temp--;
								}
								callername = malloc(temp - s + 1);
								memcpy(callername, s, temp - s);
								callername[temp - s] = '\0';

								s = strstr(s, "sip:");
								if (s != NULL) {
									s += 4;
									temp = strstr(s, "@");
									callernumber = malloc(temp - s + 1);
									memcpy(callernumber, s, temp - s);
									callernumber[temp - s] = '\0';
								}
							}
						}
					}

					if (callername == NULL) callername = "Unknown name";
					if (callernumber == NULL) callernumber = "Unknown number";

					doAction(callername, callernumber);
				}
			}
			break;
		case WAIT_OBJECT_0 + 2:
			doAction("Fake ring", "905-123-4567");
			ResetEvent(hArray[2]);
			break;
		case WAIT_FAILED:
			MessageBox(0, "Wait error.\n\nPlease report this as it should never happen:\ngabe@luci.ca", "WinampRING", 0);
			return FALSE;
			break;
		default:;
		}
	}
	return TRUE;
}

//handles all messages to the config window (mostly from mouse clicks)
BOOL CALLBACK ConfigProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char s[50] = "";
	BOOL r = FALSE;
	int vol = 0;
	switch (uMsg) {
	case WM_INITDIALOG:
	{ //Center the dialog
		RECT rc, rcDlg, rcOwner;
		// Get the owner window and dialog box rectangles.

		GetWindowRect(GetDesktopWindow(), &rcOwner);
		GetWindowRect(hwndDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that
		// right and bottom values represent the width and
		// height, and then offset the owner again to discard
		// space taken up by the dialog box.
		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining
		// space and the owner's original position.
		SetWindowPos(hwndDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);
	}

	//update all the controls to the current settings
	CheckDlgButton(hwndDlg, IDC_VOLUME, action == IPC_SETVOLUME);
	CheckDlgButton(hwndDlg, IDC_PAUSE, action == WINAMP_BUTTON3);
	CheckDlgButton(hwndDlg, IDC_STOP, action == WINAMP_BUTTON4);
	CheckDlgButton(hwndDlg, IDC_CHKPERCENT, percent);
	CheckDlgButton(hwndDlg, IDC_CHKWINVOL, chWinVol);
	CheckDlgButton(hwndDlg, IDC_CHKPOPUP, popup);
	CheckDlgButton(hwndDlg, IDC_CHKENABLE, enabled);
	CheckDlgButton(hwndDlg, IDC_CHKCALLERID, callerId);
	CheckDlgButton(hwndDlg, IDC_MODEM, listenOnModem);
	CheckDlgButton(hwndDlg, IDC_SIP, !listenOnModem);

	strncpy_s(s, 50, initStr, strlen(initStr) - 1);
	SetDlgItemText(hwndDlg, IDC_EDITINIT, s);

	strncpy_s(s, 50, callIdInit, strlen(callIdInit) - 1);
	SetDlgItemText(hwndDlg, IDC_EDITCALLERID, s);

	if (volume == 0 && chWinVol) {	//0 vol for windows vol is mute
		SetDlgItemText(hwndDlg, IDC_EDITVOL, "mute");
	} else {
		sprintf_s(s, 50, "%i%%", volume);
		SetDlgItemText(hwndDlg, IDC_EDITVOL, s);
	}
	sprintf_s(s, 50, "%i", comPort);
	SetDlgItemText(hwndDlg, IDC_EDITPORT, s);

	strncpy_s(s, 50, sipPort, strlen(sipPort));
	SetDlgItemText(hwndDlg, IDC_SIPPORT, s);

	SendDlgItemMessage(hwndDlg, IDC_SVOL, TBM_SETPOS, 1, volume);

	EnableWindow(GetDlgItem(hwndDlg, IDC_APPLY), FALSE);
	SetDlgItemText(hwndDlg, IDCANCEL, "&Close");

	r = enabled;	//update enabled/disabled depending on options
	EnableWindow(GetDlgItem(hwndDlg, IDC_GRENABLE), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_VOLUME), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_PAUSE), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_STOP), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHKPOPUP), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_MODEM), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_SIP), r);

	//enable/disable volume options
	r = enabled && action == IPC_SETVOLUME;
	EnableWindow(GetDlgItem(hwndDlg, IDC_GRVOL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_SVOL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_EDITVOL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHKPERCENT), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHKWINVOL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_VOLLABEL), r);

	//enable/disable popup options
	r = enabled && popup && listenOnModem;
	EnableWindow(GetDlgItem(hwndDlg, IDC_GRPOPUP), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHKCALLERID), r);

	r = enabled && popup && callerId && listenOnModem;
	EnableWindow(GetDlgItem(hwndDlg, IDC_CALLIDLABEL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_EDITCALLERID), r);

	//enable/disable modem options
	r = enabled && listenOnModem;
	EnableWindow(GetDlgItem(hwndDlg, IDC_EDITPORT), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_COMLABEL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_EDITINIT), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_INITLABEL), r);

	//enable/disable sip options
	r = enabled && !listenOnModem;
	EnableWindow(GetDlgItem(hwndDlg, IDC_SIPPORTLABEL), r);
	EnableWindow(GetDlgItem(hwndDlg, IDC_SIPPORT), r);

	showErr(hwndDlg);	//show last error
	cancelExit = FALSE;
	break;
	case WM_HSCROLL:	//update edit box with new value of scroll bar
		vol = SendDlgItemMessage(hwndDlg, IDC_SVOL, TBM_GETPOS, 0, 0);
		if (vol == 0 && IsDlgButtonChecked(hwndDlg, IDC_CHKWINVOL)) {
			SetDlgItemText(hwndDlg, IDC_EDITVOL, "mute");
		} else {
			char str[5];
			sprintf_s(str, 5, "%i%%", vol);
			SetDlgItemText(hwndDlg, IDC_EDITVOL, str);
		}
		//flag changed state
		EnableWindow(GetDlgItem(hwndDlg, IDC_APPLY), TRUE);
		SetDlgItemText(hwndDlg, IDCANCEL, "&Cancel");
		break;
	case WM_COMMAND:
		//if anything changes, flag changed state
		if ((HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == BN_CLICKED) && LOWORD(wParam) != IDC_ABOUT && LOWORD(wParam) != IDOK) {
			EnableWindow(GetDlgItem(hwndDlg, IDC_APPLY), TRUE);
			SetDlgItemText(hwndDlg, IDCANCEL, "&Cancel");
		}
		switch (LOWORD(wParam)) {
		case IDC_DOACTION:
			if (hFakeDoAction != INVALID_HANDLE_VALUE) {
				SetEvent(hFakeDoAction);
			}
			break;
		case IDOK:
		case IDC_APPLY:	//if OK or Apply clicked, update all values
			enabled = FALSE;	//shut everything down so that it can be tested fresh
			checkMonitorPlay();

			GetDlgItemText(hwndDlg, IDC_EDITVOL, s, 4);
			sscanf_s(s, "%i", &volume);
			GetDlgItemText(hwndDlg, IDC_EDITPORT, s, 2);
			sscanf_s(s, "%i", &comPort);

			GetDlgItemText(hwndDlg, IDC_SIPPORT, s, 6);
			sprintf_s(sipPort, 6, "%s", s);

			GetDlgItemText(hwndDlg, IDC_EDITINIT, s, 50);
			sprintf_s(initStr, 50, "%s\r", s);

			GetDlgItemText(hwndDlg, IDC_EDITCALLERID, s, 50);
			sprintf_s(callIdInit, 50, "%s\r", s);

			if (IsDlgButtonChecked(hwndDlg, IDC_VOLUME))
				action = IPC_SETVOLUME;
			if (IsDlgButtonChecked(hwndDlg, IDC_PAUSE))
				action = WINAMP_BUTTON3;
			if (IsDlgButtonChecked(hwndDlg, IDC_STOP))
				action = WINAMP_BUTTON4;
			percent = IsDlgButtonChecked(hwndDlg, IDC_CHKPERCENT);
			chWinVol = IsDlgButtonChecked(hwndDlg, IDC_CHKWINVOL);
			popup = IsDlgButtonChecked(hwndDlg, IDC_CHKPOPUP);
			enabled = IsDlgButtonChecked(hwndDlg, IDC_CHKENABLE);
			callerId = IsDlgButtonChecked(hwndDlg, IDC_CHKCALLERID);
			listenOnModem = IsDlgButtonChecked(hwndDlg, IDC_MODEM);
			if (chWinVol && enabled) {
				openMixer();
			} else if (mixOpen) {
				closeMixer();
			}
			badPort = FALSE;
			if (enabled) {
				SetWindowText(hwndDlg, "WinampRING Options - Testing port...");
				if (listenOnModem) {
					openComPort();			//open port to test it
				} else {
					openSipPort();
				}
				SetWindowText(hwndDlg, "WinampRING Options");
				cancelExit = FALSE;
				showErr(hwndDlg);	//show errors if any
			}
			//flag unchanged state
			EnableWindow(GetDlgItem(hwndDlg, IDC_APPLY), FALSE);
			SetDlgItemText(hwndDlg, IDCANCEL, "&Close");
			checkMonitorPlay();		//start/stop monitorPlay as appropriate
			if (LOWORD(wParam) == IDC_APPLY) {
				cancelExit = FALSE;
				break;	//if Apply was clicked, we want to stop here
			}
		case IDCANCEL:
			if (cancelExit) {		//if error occured, keep window open
				cancelExit = FALSE;
			} else {				//otherwise close it
				if (unresolvedError)
					enabled = FALSE;	//disable if error went unresolved
				EndDialog(hwndDlg, 0);
			}
			break;
		case IDC_EDITVOL:	//edit box changed
			switch (HIWORD(wParam)) {
			case EN_KILLFOCUS:
			{		//update scroll bar with new value of edit box when focus lost
				int v;

				GetDlgItemText(hwndDlg, IDC_EDITVOL, s, 4);
				sscanf_s(s, "%i", &v);
				if (v > 100) {
					v = 100;
				} else if (v < 0) {
					v = 0;
				}
				SendDlgItemMessage(hwndDlg, IDC_SVOL, TBM_SETPOS, 1, v);
				sprintf_s(s, 50, "%i%%", v);
				SetDlgItemText(hwndDlg, IDC_EDITVOL, s);
				break;
			}
			}
			break;
		case IDC_CHKWINVOL:	//if vol is 0, change to mute or 0% depending on new state
			vol = SendDlgItemMessage(hwndDlg, IDC_SVOL, TBM_GETPOS, 0, 0);
			if (vol == 0 && IsDlgButtonChecked(hwndDlg, IDC_CHKWINVOL)) {
				SetDlgItemText(hwndDlg, IDC_EDITVOL, "mute");
			} else {
				char str[4];
				sprintf_s(str, 4, "%i%%", vol);
				SetDlgItemText(hwndDlg, IDC_EDITVOL, str);
			}
			break;
		case IDC_CHKENABLE:	//update enabled states depending on check boxes
			r = IsDlgButtonChecked(hwndDlg, IDC_CHKENABLE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_GRENABLE), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_VOLUME), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PAUSE), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_STOP), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHKPOPUP), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_GRDETECT), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_MODEM), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SIP), r);
		case IDC_VOLUME:
		case IDC_PAUSE:
		case IDC_STOP:
			r = IsDlgButtonChecked(hwndDlg, IDC_CHKENABLE) &&
				IsDlgButtonChecked(hwndDlg, IDC_VOLUME);
			EnableWindow(GetDlgItem(hwndDlg, IDC_GRVOL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SVOL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_EDITVOL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHKPERCENT), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHKWINVOL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_VOLLABEL), r);
		case IDC_SIP:
		case IDC_MODEM:
			r = IsDlgButtonChecked(hwndDlg, IDC_CHKENABLE) &&
				IsDlgButtonChecked(hwndDlg, IDC_MODEM);
			EnableWindow(GetDlgItem(hwndDlg, IDC_EDITPORT), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_COMLABEL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_EDITINIT), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_INITLABEL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SIPPORTLABEL), !r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SIPPORT), !r);
		case IDC_CHKPOPUP:
			r = IsDlgButtonChecked(hwndDlg, IDC_CHKENABLE) &&
				IsDlgButtonChecked(hwndDlg, IDC_CHKPOPUP) &&
				IsDlgButtonChecked(hwndDlg, IDC_MODEM);
			EnableWindow(GetDlgItem(hwndDlg, IDC_GRPOPUP), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHKCALLERID), r);
		case IDC_CHKCALLERID:
			r = IsDlgButtonChecked(hwndDlg, IDC_CHKENABLE) &&
				IsDlgButtonChecked(hwndDlg, IDC_CHKPOPUP) &&
				IsDlgButtonChecked(hwndDlg, IDC_CHKCALLERID) &&
				IsDlgButtonChecked(hwndDlg, IDC_MODEM);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CALLIDLABEL), r);
			EnableWindow(GetDlgItem(hwndDlg, IDC_EDITCALLERID), r);
			break;
		case IDC_ABOUT:		//open about window
			DialogBox(plugin.hDllInstance, MAKEINTRESOURCE(IDD_ABOUT), hwndDlg, AboutProc);
			break;
		}
		break;
	}
	return FALSE;
}

//handles all messages to about window
BOOL CALLBACK AboutProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
	{
		HANDLE hReadme;	//open readme file
		hReadme = CreateFile((LPCSTR)readme, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
		if (hReadme == INVALID_HANDLE_VALUE) {	//handle error
			SetDlgItemText(hwndDlg, IDC_ABOUTEDIT, "Could not open Readme_WinampRING.txt. Please reinstall WinampRING.");
			break;
		}
		{	//put readme file into edit box
			DWORD dwFileSize = GetFileSize(hReadme, NULL);
			DWORD byread;
			char *buf;

			buf = malloc(dwFileSize + 1);
			buf[dwFileSize] = '\0'; //null termination
			if (buf == NULL) {
				SetDlgItemText(hwndDlg, IDC_ABOUTEDIT, "Insufficient memory to open Readme_WinampRING.txt");
				break;
			}
			ReadFile(hReadme, buf, dwFileSize, &byread, NULL);
			SetDlgItemText(hwndDlg, IDC_ABOUTEDIT, buf);
			free(buf);	//free memory used for file
		}
		CloseHandle(hReadme);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:		//close window
		case IDCANCEL:
			EndDialog(hwndDlg, 0);
		}
	}
	return FALSE;
}

//completes the desired action when the phone rings
void doAction(char *callername, char *callernum) {
	if (SendMessage(plugin.hwndParent, WM_USER, 0, IPC_ISPLAYING) != 1)
		return;		//if Winamp's not playing, we shouldn't be doing anything
	if (action == IPC_SETVOLUME) {//if we're supposed to change the volume, do so
		int vol;
		if (chWinVol) {		//for Windows volume
			if (percent) {
				changeWinVol(getWinVol()*volume / 100);
			} else {
				vol = volCon.Bounds.lMaximum*volume / 100;
				if (vol < getWinVol())
					changeWinVol(vol);
			}
		} else {			//for Winamp volume
			int old = SendMessage(plugin.hwndParent, WM_USER, -666, IPC_SETVOLUME);
			if (percent) {
				vol = old*volume / 100;
			} else {
				vol = 255 * volume / 100;
			}
			if (vol < old)
				SendMessage(plugin.hwndParent, WM_USER, vol, IPC_SETVOLUME);
		}
	} else {	//if pause or stop, send that message
		SendMessage(plugin.hwndParent, WM_COMMAND, action, 0);
	}
	if (popup) {		//open popup with current time if asked for
		struct tm newtime;
		char am_pm[] = "AM";
		time_t long_time;
		char msg[100];
		char asciitime[50];

		time(&long_time);                // Get time as long integer.
		localtime_s(&newtime, &long_time); // Convert to local time.

		if (newtime.tm_hour >= 12)        // Set up extension.
			strcpy_s(am_pm, 3, "PM");
		if (newtime.tm_hour > 12)        // Convert from 24-hour
			newtime.tm_hour -= 12;    //   to 12-hour clock.
		if (newtime.tm_hour == 0)        //Set hour to 12 if midnight.
			newtime.tm_hour = 12;

		//If caller ID is enabled, output the information.
		asctime_s(asciitime, 50, &newtime);
		if (callerId || !listenOnModem) {
			sprintf_s(msg, 100, "The phone is ringing!\n%.19s %s\n\nFrom:\n%s\n%s\n", asciitime, am_pm, callername, callernum);
		} else { //otherwise, don't
			sprintf_s(msg, 100, "The phone is ringing!\n%.19s %s\n", asciitime, am_pm);
		}

		//flag message box as open, so this thread isn't killed before it is closed
		//popupOpen=TRUE;
		MessageBox(0, msg, "WinampRING", MB_ICONINFORMATION | MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		//popupOpen=FALSE;
	}
}

//opens Windows' default mixer (for controlling Windows volume)
void openMixer() {
	if (!(enabled && chWinVol && action == IPC_SETVOLUME) || mixOpen)
		return;	//if we don't need to, don't do it
	//open default mixer
	if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR) {
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		//Get mixer info
		if (mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR) {
			strcpy_s(lastErr, 100, "Could not open default mixer.\nWinampRING will be unable to change Windows' volume.");
			return;
		}
		//set structures for volume control
		mxlc.cbStruct = sizeof(mxlc);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(volCon);
		mxlc.pamxctrl = &volCon;

		volCon.cbStruct = sizeof(volCon);
		//get volume info
		if (mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) {
			strcpy_s(lastErr, 100, "Could not open default mixer.\nWinampRING will be unable to change Windows' volume.");
			mixerClose((HMIXER)hMixer);
			return;
		}
		//get volume control
		if (mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_LINEID) != MMSYSERR_NOERROR) {
			strcpy_s(lastErr, 100, "Could not open default mixer.\nWinampRING will be unable to change Windows' volume.");
			mixerClose((HMIXER)hMixer);
			return;
		}
		//set structures for mute control
		mxlc.cbStruct = sizeof(mxlc);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(muteCon);
		mxlc.pamxctrl = &muteCon;

		muteCon.cbStruct = sizeof(muteCon);
		//get mute control
		if (mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) {
			strcpy_s(lastErr, 100, "Could not open default mixer.\nWinampRING will be unable to change Windows' volume.");
			mixerClose((HMIXER)hMixer);
			return;
		}

	} else {	//handle error
		strcpy_s(lastErr, 100, "Could not open default mixer.\nWinampRING will be unable to change Windows' volume.");
		return;
	}
	mixOpen = TRUE;	//show that mixer is open
}

//changes Windows' volume to the specified volume level
void changeWinVol(int newVol) {
	if (!mixOpen)	//if mixer's not open, we can't do anything
		return;
	//make sure new volume is within bounds
	if (newVol > volCon.Bounds.lMaximum) {
		newVol = volCon.Bounds.lMaximum;
	} else if (newVol < volCon.Bounds.lMinimum) {
		newVol = volCon.Bounds.lMinimum;
	}

	if (newVol == 0) {		//if vol=0, mute instead of changing volume
		mxcdu.dwValue = 1;	//set values in the structure

		mxcd.cbStruct = sizeof(mxcd);
		mxcd.hwndOwner = 0;
		mxcd.dwControlID = muteCon.dwControlID;
		mxcd.cbDetails = sizeof(mxcdu);
		mxcd.paDetails = &mxcdu;
		mxcd.cChannels = 1;
		//and mute
		if (mixerSetControlDetails((HMIXEROBJ)hMixer, (LPMIXERCONTROLDETAILS)&mxcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
			strcpy_s(lastErr, 100, "WinampRING was unable to mute Windows' volume using the default mixer.");
	} else {		//otherwise, change volume
		mxcdu.dwValue = newVol;	//set values in structure

		mxcd.cbStruct = sizeof(mxcd);
		mxcd.hwndOwner = 0;
		mxcd.dwControlID = volCon.dwControlID;
		mxcd.cbDetails = sizeof(mxcdu);
		mxcd.paDetails = &mxcdu;
		mxcd.cChannels = 1;
		//and change the volume
		if (mixerSetControlDetails((HMIXEROBJ)hMixer, (LPMIXERCONTROLDETAILS)&mxcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
			strcpy_s(lastErr, 100, "WinampRING was unable to change Windows' volume using the default mixer.");
	}
}

//returns Windows' current volume
int getWinVol() {
	if (!mixOpen)	//if mixer's not open, we can't do it
		return 0;

	mxcdu.dwValue = 0;	//set values in structure

	mxcd.cbStruct = sizeof(mxcd);
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = volCon.dwControlID;
	mxcd.cbDetails = sizeof(mxcdu);
	mxcd.paDetails = &mxcdu;
	mxcd.cChannels = 1;
	//and get the volume
	if (mixerGetControlDetails((HMIXEROBJ)hMixer, (LPMIXERCONTROLDETAILS)&mxcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
		strcpy_s(lastErr, 100, "WinampRING was unable to get Windows' current volume using the default mixer.\n\'Percent of current\' option will be useless while using \'Change Windows volume\' unless fixed.");
	return mxcdu.dwValue;	//return the volume
}

//closes the mixer if it's open
void closeMixer() {
	if (mixOpen) {
		mixerClose((HMIXER)hMixer);
		mixOpen = FALSE;	//set flag
	}
}

//open the COM port
void openComPort() {	//do only if enabled and port isn't open and port is not bad
	if (hComm == INVALID_HANDLE_VALUE && enabled && !badPort) {
		char port[6];
		char buf[50] = "";
		BOOL m_bPortReady;
		COMMTIMEOUTS m_CommTimeouts;

		memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		sprintf_s(port, 6, "COM%i", comPort);	//set COM port string e.g. "COM1"
		hComm = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (hComm == INVALID_HANDLE_VALUE) {	//if error
			if (GetLastError() == ERROR_SHARING_VIOLATION)
				return;		//if already opened by another process, ignore the error
			strcpy_s(lastErr, 100, "WinampRING was unable to open the specified COM port.\nPlease check your settings.");
			badPort = TRUE;	//otherwise, report and set badport flag
		}
		//read timeout values
		m_bPortReady = GetCommTimeouts(hComm, &m_CommTimeouts);

		m_CommTimeouts.ReadIntervalTimeout = 1000;
		m_CommTimeouts.ReadTotalTimeoutMultiplier = 0;
		m_CommTimeouts.ReadTotalTimeoutConstant = 3000;
		m_CommTimeouts.WriteTotalTimeoutConstant = 50;
		m_CommTimeouts.WriteTotalTimeoutMultiplier = 10;
		//and change them
		m_bPortReady = SetCommTimeouts(hComm, &m_CommTimeouts);

		//send initialization
		if (!WriteComm(initStr)) {
			return;
		}
		//wait for response
		if (!ReadComm(buf)) {
			return;
		}
		//test for OK
		if (!strstr(buf, "OK")) {
			strcpy_s(lastErr, 100, "WinampRING could not initialize your modem.\nPlease check your settings.\n");
			//strcpy_s(lastErr,buf);	//for debugging purposes
			badPort = TRUE;	//if didn't get it, report error and bad port
			closeComPort();	//and close port
			return;
		}

		if (popup && callerId) {
			//send code to enable caller ID
			if (!WriteComm(callIdInit)) {
				return;
			}
			//wait for OK back
			if (!ReadComm(buf)) {
				return;
			}
			//test for OK
			if (!strstr(buf, "OK")) {
				strcpy_s(lastErr, 100, "WinampRING could not enable caller ID.\nPlease check your settings.\n");
				//strcpy_s(lastErr,buf);	//for debugging purposes
				badPort = TRUE;	//if didn't get it, report error and bad port
				closeComPort();	//and close port
				return;
			}
		}

		//if all successful, start thread to monitor the port
		hEndPortMonThread = CreateEvent(NULL, TRUE, FALSE, NULL);
		hFakeDoAction = CreateEvent(NULL, TRUE, FALSE, NULL);
		hPortMonThread = (HANDLE)_beginthreadex(NULL, 0, (void *)&monitorPort, NULL, 0, &threadID);
	}
}

//closes the COM port if it's open
void closeComPort() {
	if (hComm != INVALID_HANDLE_VALUE) {
		if (hPortMonThread != INVALID_HANDLE_VALUE) {	//if the thread is going, kill it
			//while (popupOpen) { Sleep(100); } //if a popup is open, wait till it's gone
			//Flag thread to die
			SetEvent(hEndPortMonThread);
			//and wait for it to do so
			WaitForSingleObject(hPortMonThread, INFINITE);
			CloseHandle(hPortMonThread);
			hPortMonThread = INVALID_HANDLE_VALUE;

			CloseHandle(hEndPortMonThread);
			hEndPortMonThread = INVALID_HANDLE_VALUE;

			CloseHandle(hFakeDoAction);
			hFakeDoAction = INVALID_HANDLE_VALUE;
		}
		CloseHandle(hComm);			//close port
		hComm = INVALID_HANDLE_VALUE;
	}
}

//monitors the COM port for a ring
unsigned __stdcall monitorPort() {
	long event;
	time_t newTime;
	char buf[100] = "";
	BOOL canDoAction = FALSE;
	char callername[50] = "";
	char callernum[50] = "";
	char *clrnam;
	char *clrnum;
	HANDLE hArray[3];

	hArray[0] = hEndPortMonThread;	//event to kill thread
	hArray[1] = overlapped.hEvent;	//overlapped comm event
	hArray[2] = hFakeDoAction;	//event to trigger fake ring

	while (TRUE) {
		//if this thread is signalled to die, then die
		//if (WaitForSingleObject(hEndPortMonThread, 0) == WAIT_OBJECT_0) {
		//	return FALSE;
		//}

		//set mask to only detect rings
		if (!SetCommMask(hComm, EV_RING)) {
			strcpy_s(lastErr, 100, "An error occurred while communicating with the specified COM port.\nPlease check your settings.");
			return FALSE;
		}

		//tell the modem we're waiting for a ring
		WaitCommEvent(hComm, (LPDWORD)&event, (LPOVERLAPPED)&overlapped);
		//sit and wait for ring or signal to die
		switch (WaitForMultipleObjects(3, hArray, FALSE, INFINITE)) {
		case WAIT_OBJECT_0:
			//MessageBox(0,"Ending port monitoring thread.","WinampRING",0);
			ResetEvent(hArray[0]);
			return FALSE;
			break;
		case WAIT_OBJECT_0 + 1:
			if (event == EV_RING) {	//if ring detected
				int i;
				//check output
				if (!ReadComm(buf)) {
					return FALSE;
				}
				//MessageBox(0,buf,"",0);
				//verify RING
				if (strstr(buf, "RING")) {
					//char s[50];
					time(&newTime);	//get the new time

					//sprintf_s(s, "%d", newTime-oldTime);
					//MessageBox(0,s,"",0);

					if (newTime - oldTime >= 20) {
						canDoAction = TRUE;	//doAction only if it's been more than 20 seconds since last ring
					}
					time(&oldTime);	//save time as old time
					if (popup && callerId && canDoAction) {
						//if we don't have caller ID info yet, wait for it
						{
							time_t oldTime1;
							time_t newTime1;
							//MessageBox(0,buf,"",0);
							time(&oldTime1);
							time(&newTime1);
							//test for caller ID info
							while (!strstr(buf, "NMBR") && newTime1 - oldTime1 < 10) {
								//wait 10 seconds if not immediately available
								if (!ReadComm(buf)) {
									return FALSE;
								}
								time(&newTime1);
							}
						}
						if (strstr(buf, "NAME")) {
							//parse name
							clrnam = strstr(buf, "NAME") + 4;
							while (clrnam[0] == '=' || clrnam[0] == ' ' || clrnam[0] == ':') {
								clrnam++;
							}
							for (i = 0; TRUE; i++) {
								if (clrnam[i] == '\n' || clrnam[i] == '\r' || clrnam[i] == '\0' || (clrnam[i] == 'N' && clrnam[i + 1] == 'M' && clrnam[i + 2] == 'B' && clrnam[i + 3] == 'R'))
									break; //Break on new line, null char, or NMBR
								strncat_s(callername, 50, clrnam + i, 1);
							}
							if (strcmp(callername, "O") == 0) {
								strcpy_s(callername, 50, "Unavailable");
							} else if (strcmp(callername, "P") == 0) {
								strcpy_s(callername, 50, "Private name");
							}
							//MessageBox(0, callername, "WinampRING", 0);
						}

						if (strstr(buf, "NMBR") || strstr(buf, "NUMBER")) {
							//parse number
							if (strstr(buf, "NMBR")) {
								clrnum = strstr(buf, "NMBR") + 4;
							} else {
								clrnum = strstr(buf, "NUMBER") + 6;
							}
							while (clrnum[0] == '=' || clrnum[0] == ' ' || clrnum[0] == ':') {
								clrnum++;
							}
							for (i = 0; TRUE; i++) {
								if (clrnum[i] == '\n' || clrnum[i] == '\r' || clrnum[i] == '\0')
									break; //Break on new line or null char
								strncat_s(callernum, 50, clrnum + i, 1);
							}
							if (strcmp(callernum, "O") == 0 || strcmp(callernum, "-") == 0) {
								strcpy_s(callernum, 50, "Unknown number");
							} else if (strcmp(callernum, "P") == 0) {
								strcpy_s(callernum, 50, "Private number");
							}
							//MessageBox(0, callernum, "WinampRING", 0);
						} else {
							//if we didn't find any info, we want to say so
							strcpy_s(callername, 50, "Information unavailable");
						}
					}
				}

			}
			if (canDoAction) {
				canDoAction = FALSE;
				doAction(callername, callernum);
				strcpy_s(callername, 50, "");
				strcpy_s(callernum, 50, "");
			}

			//reinitialize modem to clear ring flag
			if (!WriteComm(initStr)) {
				strcpy_s(lastErr, 100, "An error occurred while writing to the specified COM port.\nPlease check your settings.");
				return FALSE;
			}
			//resend code to enable caller ID, if needed
			if (callerId && !WriteComm(callIdInit)) {
				return FALSE;
			}
			//check and discard output from modem
			if (!ReadComm(buf)) {
				return FALSE;
			}
			strcpy_s(buf, 100, "");

			PurgeComm(hComm, PURGE_RXABORT | PURGE_RXCLEAR);
			ResetEvent(hArray[1]);
			break;
		case WAIT_OBJECT_0 + 2:
			doAction("Fake ring", "905-123-4567");
			ResetEvent(hArray[2]);
			break;
		case WAIT_FAILED:
			MessageBox(0, "Wait error.\n\nPlease report this as it should never happen:\ngabe@luci.ca", "WinampRING", 0);
			return FALSE;
			break;
		default:;
		}
	}
	return FALSE;
}

BOOL WriteComm(char toWrite[100]) {
	BOOL retVal = FALSE;
	int byteWrit = 0;

	if (hComm == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	//MessageBox(NULL, toWrite,"WinampRING",0);
	retVal = WriteFile(hComm, toWrite, strlen(toWrite), (LPDWORD)&byteWrit, &overlapped);
	GetOverlappedResult(hComm, &overlapped, (LPDWORD)&byteWrit, TRUE);
	if (byteWrit > 0) {
		return TRUE;
	}
	strcpy_s(lastErr, 100, "An error occurred while writing to the specified COM port.\nPlease check your settings.");
	badPort = TRUE;	//if can't write, report error and bad port
	return FALSE;
}

BOOL ReadComm(char readStr[100]) {
	BOOL retVal = FALSE;
	int byteRead = 0;

	if (hComm == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	retVal = ReadFile(hComm, readStr, 99, (LPDWORD)&byteRead, &overlapped);
	GetOverlappedResult(hComm, &overlapped, (LPDWORD)&byteRead, TRUE);
	//MessageBox(NULL, readStr,"WinampRING",0);
	if (byteRead > 0) {
		readStr[byteRead + 1] = '\0'; //null terminate the string
		return TRUE;
	}
	strcpy_s(lastErr, 100, "An error occurred while reading the specified COM port.\nPlease check your settings.");
	badPort = TRUE;	//if can't read, report error and bad port
	return FALSE;
}

//for Winamp's use
__declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
	return &plugin;
}