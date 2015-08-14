		 WinampRING v1.5
	     Created and written by Gabriel Luci.

Special thanks to Dan Homorcean for debugging help.

WHAT'S NEW?
v1.5
New: SIP support

v1.1
New: Added caller ID support.
New: Modem initialization code can now be changed.
New: When OK or Apply is clicked, you are now informed that the port is being tested for the next couple seconds.
Fixed: If action was pause or stop, and message box was enabled, the message box would disappear before it could be read.
Fixed: If action was pause or stop, and you decided to not answer the phone and continue playback, WinampRING would react to the next ring again.
Fixed: Updated code for Visual Studio 2005, changing old, insecure functions to new, secure functions.

DOCUMENTATION

This plugin will turn down the volume, or pause or stop Winamp when the phone rings. It makes use of a standard Hayes compatible modem.

The modem port is only opened when Winamp is playing. If you use your modem for other purposes, such as the internet, simply pause or stop Winamp before connecting, then hit play again. WinampRING will ignore any errors caused by other programs having control of the modem.

OPTIONS DIALOG
Controls how WinampRING responds when it detects the phone ringing.

ERRORS
Errors are only reported when the options dialog is opened. This is so you don't have errors popping up when you just want to listen to music. If WinampRING fails to respond to the phone ringing, open the options dialog to check for errors. In the case of multiple errors, only the last is reported. If an error goes unresolved, WinampRING will disable itself.

OPTIONS LISTING

LOWER VOLUME
Select this to lower the volume when the phone rings.

PAUSE WINAMP
Pauses Winamp's playback when the phone rings.

STOP WINAMP
Stops Winamp's playback when the phone rings.

VOLUME OPTIONS
-Volume to change to
This sets the volume level you want after the phone rings. Typing in the text box will also change the selected volume level.
Note: WinampRING will only turn down the volume. If the volume to change to is higher than the current volume when the phone rings, WinampRING will not change the volume.

-Percent of current volume
If this option is selected, the volume will change to a percentage of what the current volume is. For example, say the volume to change to is set at 50% and when the phone rings, the current volume is set at 80%. The resulting volume level will be 40%.

-Change Windows volume
Select this option to change Windows' main volume instead of Winamp's volume. This is only of use if Winamp is playing through Windows' primary sound card.

MODEM COM PORT
This is the COM port your modem is on. It is required for usability.

INIT CODE
The code necessary to initialize the modem. For most modems, the default should be sufficient. If an error is produced saying that WinampRING cannot initialize the modem, you will have to find the proper code for your modem from your modem's manual or the internet and enter it in this field.

SIP PORT
The UDP port to listen on for a SIP INVITE. To be able to use this, you must configure your VoIP provider to send a call to a SIP URI, which will be your IP address. This port will also need to be allowed by your router. Currently, it only listens on IPv4.

POPUP MESSAGE ON RING
This will cause WinampRING to display a popup message when the phone rings simply stating "The phone is ringing!" with the time and date that it occurred. Caller ID information will be displayed as well, if the feature is enabled.

POPUP OPTIONS
-Enable caller ID
If your modem supports caller ID and you have a caller ID service from your phone provider, you can enable this feature to see the caller's name and number in the popup message. When enabled, WinampRING will not react immediately to a ring, but will wait up to 10 seconds until caller ID information is available, which usually happens between the first and second rings.

-Code to enable caller ID
Most modems have caller ID support disabled by default. So it needs to be enabled by sending the modem a code. The default code should work for most modems supporting caller ID. If it does not work for you, WinampRING will produce an error saying that it cannot enable caller ID. In this case, you will have to find the proper code for your modem from your modem's manual or the internet and enter it in this field. Some common codes are:
  AT#CID=1
  AT%CCID=1
  AT+VCID=1
  AT#CC1
  AT*ID1

ENABLED
Selecting or deselecting this option will enable or disable WinampRING.