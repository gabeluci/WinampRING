# WinampRING

A [Winamp](http://winamp.com) plugin that will turn down the volume, pause or stop Winamp when the phone rings. To detect a ring, it can use any modem on a COM port, or, for those with a configurable VoIP service, it can listen on the network for a SIP INVITE.

When using a modem, the COM port is only opened when Winamp is playing. If you use your modem for other purposes, such as the faxes, simply pause or stop Winamp before sending, then hit play again. WinampRING will ignore any errors caused by other programs having control of the modem.


## Download
The latest release is version 1.5, which added SIP support. Prior versions were hosted on the Winamp site before it was sold to Radionomy. Once they bring the site back to life, I'll probably host it there again, if they allow it.

[Download installer](https://github.com/gabeluci/WinampRING/releases/download/1.5/WinampRING.exe)

Once installed, open Winamp and go into the Preferences. Under 'Plugins', click on 'General Purpose'. Click on 'WinampRING v1.5', then 'Configure selected plug-in'.


## Description of Options
![Options Dialog](/WinampRING.jpg)


#### Action to take
Use the radio button to select which action to take when a ring is detected.

**Lower Volume**: Lower the volume to the specified level.

**Pause Winamp**: Pause playback (equivalent to pressing the pause button).

**Stop Winamp**: Stop playback (equivalent to pressing the stop button).


#### Volume Options
These options are used when the _Lower Volume_ option is selected.

**Volume to change to**: This sets the volume level you want after the phone rings. Typing in the text box will also change the selected volume level. WinampRING will only turn down the volume. If the volume to change to is higher than the current volume when the phone rings, WinampRING will not change the volume.

**Percent of current volume**: If this option is selected, the volume will change to a percentage of what the current volume is. For example, say the volume to change to is set at 50% and when the phone rings, the current volume is set at 80%. The resulting volume level will be 40%.

**Change Windows volume**: Select this option to change Windows' main volume instead of Winamp's volume. This is only of use if Winamp is playing through Windows' primary sound card.


#### Popup message on ring
This will cause WinampRING to display a popup message when the phone rings simply stating "The phone is ringing!" with the time and date that it occurred. Caller ID information will be displayed as well, if the feature is enabled.

**Enable caller ID**: Only relevant when using a modem. If your modem supports caller ID and you have a caller ID service from your phone provider, you can enable this feature to see the caller's name and number in the popup message. When enabled, WinampRING will not react immediately to a ring, but will wait up to 10 seconds until caller ID information is available, which usually happens between the first and second rings.

**Code to enable caller ID**: Most modems have caller ID support disabled by default. So it needs to be enabled by sending the modem a code. The default code should work for most modems supporting caller ID. If it does not work for you, WinampRING will produce an error saying that it cannot enable caller ID. In this case, you will have to find the proper code for your modem from your modem's manual or the internet and enter it in this field. Some common codes are:
* AT#CID=1
* AT%CCID=1
* AT+VCID=1
* AT#CC1
* AT*ID1


#### Ring Detection
Determines how a ring is detected.

**Modem**: Listen for a "RING" from a modem.

**Modem COM port**: This is the COM port your modem is on. It is required for usability.

**Init code**: The code necessary to initialize the modem. For most modems, the default should be sufficient. If an error is produced saying that WinampRING cannot initialize the modem, you will have to find the proper code for your modem from your modem's manual or the internet and enter it in this field.

**SIP**: Listens on the specified UDP port for a SIP INVITE. Note that this will not perform a SIP REGISTER with your VoIP provider. To be able to use this feature, you must configure your VoIP provider to send a call to a SIP URI, which will be your IP address (or a dynamic domain name pointing to your IP) combined with the SIP port you configure (e.g. "10.76.54.45:5060"). This port will also need to be allowed by your router. Currently, it only listens on IPv4. Admittedly, I did the abolute minimum to make this work. It will not reply to the SIP INVITE at all. That may not play nicely with some providers, but it seems to be fine for mine (Anveo).

**SIP port**: The UDP port to listen on. The default SIP port is 5060, but it's a good idea to use a non-standard port so you don't get unsolicited INVITEs (it happens).


#### Enabled
Selecting or deselecting this option will enable or disable WinampRING.


#### Errors
Errors are only reported when the options dialog is opened. This is so you don't have errors popping up when you just want to listen to music. If WinampRING fails to respond to the phone ringing, open the options dialog to check for errors. In the case of multiple errors, only the last is reported. If an error goes unresolved, WinampRING will disable itself.
