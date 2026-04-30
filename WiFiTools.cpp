#include "WiFiTools.h"
#include <WiFiMulti.h>
#include <HardwareSerial.h>
// 
// 
// 

//WiFiTools::WiFiTools() {}

void WiFiTools::Initialize(SDCard& sd)
{
	_sd = sd;
}

WiFiMulti wifiMulti;	// WiFiMulti instance to connect to wifi.

/// <summary>
/// Connect to strongest WiFi access point. 
/// Will not return until connection succeeds 
/// or timeout is reached.
/// </summary>
/// <param name="timeout_sec">
/// Maximum number of seconds to try until failure.</param>
/// <returns>True if connection is successful.</returns>
bool WiFiTools::wifiConnect(unsigned int timeout_sec) {
	unsigned long timeStart = millis();
	// Try to connect for timeout_sec.
	while (
		wifiMulti.run(WIFI_CONNECT_TIMEOUT_SEC * 1000) != WL_CONNECTED
		&&
		millis() - timeStart < timeout_sec * 1000
		) {
		// Trying to connect ...
	}
	String msg = "Connecting to WiFi required ";
	msg += String((millis() - timeStart) / 1000.) + "s";
	_sd.logStatus(msg);
	return  WiFi.isConnected();
}

/// <summary>
/// Check WiFi connection and reconnect if lost.
/// </summary>
/// <returns>True if WiFi is connected.</returns>
bool WiFiTools::checkWifiConnection(String dateTime) {
	// XXX Note: Can also try WL_CONNECTION_LOST !!!
	// If WiFi is lost, reconnect.
	bool isConnected = true;
	if (WiFi.status() != WL_CONNECTED) {	// XXX THIS DID NOT WORK?!?!?!
		// Wifi not connected.
		unsigned long timeStart = millis();
		//bool isResetTimerCounts = true;	// Flag to reset rotation count
		// Attempt to reconnect to wifi.
		isConnected = false;
		if (wifiConnect(WIFI_CONNECT_TIMEOUT_LOST_SEC)) {
			// Success.
			isConnected = true;
			// Log connection success.
			String msg = "Wifi re-connect successful after ";
			msg += String((millis() - timeStart) / 1000., 3) + "s";
			_sd.logStatus(msg, dateTime);
			_sd.logStatus_indent(wifi_ConnectionInfo());
			// Print IP address to serial monitor.
			Serial.println("SERVER IP ADDRESS: " + WiFi.localIP().toString());
		}
		else {
			// Failure.
			isConnected = false;
			// Log connection failure.
			String msg = "Wifi connect FAILED after ";
			msg += String((millis() - timeStart) / 1000., 3) + "s";
			_sd.logStatus(msg, dateTime);
		}
		//// Reset timer interrupt counts because of delays
		//// that occurred while reconnecting wifi.
		//// XXX resetInterruptCounts() NOT IN THIS SCOPE!!!
		//if (isResetTimerCounts) {
		//	resetInterruptCounts();
		//	String msg = "Read cycle skipped after WiFi was lost.";
		//	_sd.logStatus(msg, dateTime);
		//	//isResetTimerCounts = false;	// Reset flag.
		//}
		//return isConnected;
	}
	return isConnected;
}

/// <summary>
/// Returns string describing WiFi status.
/// </summary>
/// <returns>String describing WiFi status</returns>
String WiFiTools::wifiConnect_Status() {
	/*
	Key to WiFi status enum:
		WiFi.status() values:
		WL_NO_SHIELD = 255,
		WL_IDLE_STATUS = 0,
		WL_NO_SSID_AVAIL = 1,
		WL_SCAN_COMPLETED = 2,
		WL_CONNECTED = 3,
		WL_CONNECT_FAILED = 4,
		WL_CONNECTION_LOST = 5,
		WL_DISCONNECTED = 6
	*/
	switch (WiFi.status())
	{
	case WL_NO_SSID_AVAIL:
		return "WiFi SSID not found";
	case WL_CONNECT_FAILED:
		return "WiFi connect failed";
	case WL_CONNECTION_LOST:
		return "WiFi Connection was lost";
	case WL_SCAN_COMPLETED:
		return "WiFi Scan is completed";
	case WL_DISCONNECTED:
		return "WiFi is disconnected";
	case WL_CONNECTED:
		return "WiFi is connected!";
	default:
		return "WiFi Status: " + WiFi.status();	// Integer value.
	}
}

/// <summary>
/// Adds credentials for WiFi access points.
/// </summary>
void WiFiTools::wifiAddAccessPoints() {
	wifiMulti.addAP("RP-GL-24Ghz", "dew3pays");			// GLI.net
	wifiMulti.addAP("RPPhoto", "M##$e4you*");			// RP office
	wifiMulti.addAP("RP-router", "M##$e4you*");			// home
	wifiMulti.addAP("RP-HooToo", "dew3pays");			// HooToo portable
	wifiMulti.addAP("Vaughn Wireless", "redracoon1");	// Vaughn office
}

/// <summary>
/// Returns string listing connected WiFi SSID, RSSI, and IP adresss.
/// </summary>
/// <returns></returns>
String WiFiTools::wifi_ConnectionInfo() {
	String s = "WiFi connection: SSID ";
	s += String(WiFi.SSID());
	s += ", RSSI " + String(WiFi.RSSI());
	s += ", IP address " + WiFi.localIP().toString();
	return s;
}

/// <summary>
/// Lists WiFi networks that are in range. 
/// </summary>
/// <returns>Number of networks found.</returns>
int WiFiTools::wifiListNetworks() {
	int n = WiFi.scanNetworks();	// returns number of networks
	String msg = "Network scan complete.";
	_sd.logStatus(msg, millis());
	if (n == 0) {
		_sd.logStatus("No networks found.");
	}
	else {
		msg = "Found " + String(n) + " networks:";
		_sd.logStatus(msg, millis());
		for (int i = 0; i < n; ++i) {
			// Print SSID and RSSI for each network found
			msg = String(WiFi.SSID(i));
			msg += "\t";
			msg += String(WiFi.RSSI(i));
			_sd.logStatus_indent(msg);
		}
	}
	return n;
}

/// <summary>
/// Connects to the WiFi network.
/// </summary>
void WiFiTools::wifiSetupAndConnect(String dateTime, bool isDEBUG_BypassWiFi) {
	if (!isDEBUG_BypassWiFi) {
		// Specify WiFi credentials for router(s).
		wifiAddAccessPoints();
		// Connect to wifi.
		_sd.logStatus("Connecting to Wifi.", dateTime);
#if defined(VM_DEBUG)
		wifiListNetworks();
#endif
		if (wifiConnect(WIFI_CONNECT_TIMEOUT_LOST_SEC)) {
			_sd.logStatus("Wifi connected.", dateTime);
			_sd.logStatus_indent(wifi_ConnectionInfo());
		}
		else {
			_sd.logStatus("Wifi Connection FAILED.", dateTime);
		}
	}
	else {
		// Bypassing wifi
		_sd.logStatus("BYPASS WIFI", dateTime);
	}
}
