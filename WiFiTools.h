// WiFiTools.h

#ifndef _WIFITOOLS_h
#define _WIFITOOLS_h

#include <Arduino.h>

#include "SDCard.h"
#include <WiFiScan.h>

/// <summary>
/// Enumerates possible WiFi connection states.
/// </summary>
enum class WifiState
{
	/// <summary>
	/// Wifi is trying to connect.
	/// </summary>
	CONNECTING,
	/// <summary>
	/// Wifi successfully connected.
	/// </summary>
	CONNECTED,
	/// <summary>
	/// Wifi has lost connection.
	/// </summary>
	DISCONNECTED,
	/// <summary>
	/// Wifi is in waiting period before retrying connection.
	/// </summary>
	WAITING_TO_RETRY
};


/// <summary>
/// Exposes methods to handle WiFi operations.
/// </summary>
class WiFiTools {

private:
	//SDCard _sd;

	WifiState _wifiState = WifiState::DISCONNECTED;

public:

	/// <summary>
	/// Begin WiFiTools instance.
	/// </summary>
	void Begin();

	/// <summary>
	/// Handles events triggered by Wifi status changes.
	/// </summary>
	/// <param name="event"></param>
	/// <remarks>Events are enumerated in WiFiGeneric.h.</remarks>
	void WiFiEvent(WiFiEvent_t event);

	/// <summary>
	/// Connects in background to strongest WiFi access point.
	/// </summary>
	void connectWiFi();


	/*/// <summary>
	/// Connect to strongest WiFi access point.
	/// Will not return until connection succeeds
	/// or timeout is reached.
	/// </summary>
	/// <param name="timeout_sec">
	/// Maximum number of seconds to try until failure.</param>
	/// <returns>True if connection is successful.</returns>
	bool connectWiFi(unsigned int timeout_sec);*/

	/*/// <summary>
	/// Check WiFi connection and reconnect if lost.
	/// </summary>
	/// <returns>True if WiFi is connected.</returns>
	bool checkWifiConnection_OLD(String dateTime_Str);*/

	/// <summary>
	/// Returns state of WiFi connnection.
	/// </summary>
	/// <returns>State of WiFi connnection.</returns>
	/// <remarks>
	/// State is set by WiFiTools routines:
	///		- Begin (DISCONNECTED);
	///		- Connection atempt via WiFiConnect (CONNECTING);
	///		- WiFi event handler WiFiTools::WiFiEvent (CONNECTED, DISCONNECTED);
	/// </remarks>
	WifiState getWifiState() const;

	// <summary>
	/// Returns string describing WiFi status.
	/// </summary>
	/// <returns>String describing WiFi status</returns>
	String wifiConnect_Status();

	/// <summary>
	/// Adds credentials for WiFi access points.
	/// </summary>
	void wifiAddAccessPoints();

	/// <summary>
	/// Returns string listing connected WiFi SSID, RSSI, and IP adresss.
	/// </summary>
	/// <returns>String with connected WiFi SSID, RSSI, and IP adresss.</returns>
	String connectionInfo();

	/// <summary>
	/// Returns info on WiFi networks in range. 
	/// </summary>
	/// <returns>Info on WiFi networks in range.</returns>
	String networks_found_info();

	///// <summary>
	///// Connects to the WiFi network.
	///// </summary>
	//void wifiSetupAndConnect_OLD(String dateTime_Str, bool isDEBUG_BypassWiFi);
};

#endif
