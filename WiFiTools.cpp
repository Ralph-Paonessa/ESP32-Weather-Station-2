#include "WiFiTools.h"
#include <WiFiMulti.h>
#include <HardwareSerial.h>
#include "SDCard.h"
#include <WiFiGeneric.h>


void WiFiTools::Begin() {
	_wifiState = WifiState::DISCONNECTED;	// initial state
}

WiFiMulti wifiMulti;	// WiFiMulti instance to connect to wifi.

/// <summary>
/// Handles events triggered by Wifi status changes.
/// </summary>
/// <param name="event"></param>
/// <remarks>Events are enumerated in WiFiGeneric.h.</remarks>
void WiFiTools::WiFiEvent(WiFiEvent_t event) {
	switch (event)
	{
	case ARDUINO_EVENT_WIFI_STA_GOT_IP:
		_wifiState = WifiState::CONNECTED;
		break;
	case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
		_wifiState = WifiState::DISCONNECTED;
		break;
	}
}

/// <summary>
/// Connect in background to strongest WiFi access point. Log connection attempt.
/// </summary>
/// <time>String with current time for log (h:mm:ss or millis()).</time>
void WiFiTools::connectWiFi() {
	// Identify that we are connecting.
	_wifiState = WifiState::CONNECTING;
	// Connect to strongest access point.
	wifiMulti.run();
}

///// <summary>
///// Connect to strongest WiFi access point. 
///// Will not return until connection succeeds 
///// or timeout is reached.
///// </summary>
///// <param name="timeout_sec">
///// Maximum number of seconds to try until failure.</param>
///// <returns>True if connection is successful.</returns>
//bool WiFiTools::connectWiFi(unsigned int timeout_sec) {
//	unsigned long timeStart = millis();
//	// Try to connect for timeout_sec.
//	while (
//		wifiMulti.run(WIFI_CONNECT_TIMEOUT_SEC * 1000) != WL_CONNECTED
//		&&
//		millis() - timeStart < timeout_sec * 1000
//		) {
//		// Trying to connect ...
//	}
//	String s = "Connecting to WiFi required ";
//	s += String((millis() - timeStart) / 1000.) + "s";
//	_sd.logStatus(s);
//	return  WiFi.isConnected();
//}

///// <summary>
///// Check WiFi connection and reconnect if lost.
///// </summary>
///// <returns>True if WiFi is connected.</returns>
//bool WiFiTools::checkWifiConnection_OLD(String dateTime_Str) {
//	// XXX Note: Can also try WL_CONNECTION_LOST !!!
//	// If WiFi is lost, reconnect.
//	bool isConnected = true;
//	if (WiFi.status() != WL_CONNECTED) {	// XXX THIS DID NOT WORK?!?!?!
//		// Wifi not connected.
//		unsigned long timeStart = millis();
//		//bool isResetTimerCounts = true;	// Flag to reset rotation count
//		// Attempt to reconnect to wifi.
//		isConnected = false;
//		if (connectWiFi(dateTime_Str)) {
//			// Success.
//			isConnected = true;
//			// Log connection success.
//			String s = "Wifi re-connect successful after ";
//			s += String((millis() - timeStart) / 1000., 3) + "s";
//			_sd.logStatus(s, dateTime_Str);
//			_sd.logStatus_indent(connectionInfo());
//			// Print IP address to serial monitor.
//			Serial.println("SERVER IP ADDRESS: " + WiFi.localIP().toString());
//		}
//		else {
//			// Failure.
//			isConnected = false;
//			// Log connection failure.
//			String s = "Wifi connect FAILED after ";
//			s += String((millis() - timeStart) / 1000., 3) + "s";
//			_sd.logStatus(s, dateTime_Str);
//		}
//	}
//	return isConnected;
//}

/// <summary>
/// Returns last state of WiFi connnection.
/// </summary>
/// <returns>State of WiFi connnection.</returns>
/// <remarks>
/// State is set by WiFiTools routines:
///		- Begin (DISCONNECTED);
///		- Connection atempt via WiFiConnect (CONNECTING);
///		- WiFi event handler WiFiTools::WiFiEvent (CONNECTED, DISCONNECTED);
/// </remarks>
WifiState WiFiTools::getWifiState() const {
	return _wifiState;
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
/// <returns>String with connected WiFi SSID, RSSI, and IP adresss.</returns>
String WiFiTools::connectionInfo() {
	String s = "WiFi connection: SSID ";
	s += String(WiFi.SSID());
	s += ", RSSI " + String(WiFi.RSSI());
	s += ", IP address " + WiFi.localIP().toString();
	return s;
}

/// <summary>
/// Returns s on WiFi networks in range. 
/// </summary>
/// <returns>Info on WiFi networks in range.</returns>
String WiFiTools::networks_found_info() {
	int n = WiFi.scanNetworks();	// num networks found
	String s = "Network scan complete.";
	s += " Found " + String(n) + " networks:";
	for (int i = 0; i < n; ++i) {
		// Print SSID and RSSI for each network found
		s += "\n\t";
		s += String(WiFi.SSID(i));
		s += "\t";
		s += String(WiFi.RSSI(i));
	}
	return s;
}

///// <summary>
///// Connects to the WiFi network.
///// </summary>
//void WiFiTools::wifiSetupAndConnect_OLD(String dateTime_Str, bool isDEBUG_BypassWiFi) {
//	if (!isDEBUG_BypassWiFi) {
//		// Specify WiFi credentials for router(s).
//		wifiAddAccessPoints();
//		// Connect to wifi.
//		_sd.logStatus("Connecting to Wifi.", dateTime_Str);
//#if defined(VM_DEBUG)
//		networks_found_info();
//#endif
//		if (connectWiFi(WIFI_CONNECT_TIMEOUT_LOST_SEC)) {
//			_sd.logStatus("Wifi connected.", dateTime_Str);
//			_sd.logStatus_indent(connectionInfo());
//		}
//		else {
//			_sd.logStatus("Wifi Connection FAILED.", dateTime_Str);
//		}
//	}
//	else {
//		// Bypassing wifi
//		_sd.logStatus("BYPASS WIFI", dateTime_Str);
//	}
//}
