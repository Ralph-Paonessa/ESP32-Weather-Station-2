// WiFiTools.h

#ifndef _WIFITOOLS_h
#define _WIFITOOLS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SDCard.h"
#include <WString.h>

/// <summary>
/// Exposes methods to handle WiFi operations.
/// </summary>
class WiFiTools {

private:
	SDCard _sd;

public:

	/// <summary>
	/// Specifying an SD card for logging.
	/// </summary>
	/// <param name="sd">An SDCard entity.</param>
	void Initialize(SDCard& sd);


	// <summary>
	/// Connect to strongest WiFi access point. 
	/// Will not return until connection succeeds 
	/// or timeout is reached.
	/// </summary>
	/// <param name="timeout_sec">
	/// Maximum number of seconds to try until failure.</param>
	/// <returns>True if connection is successful.</returns>
	bool wifiConnect(unsigned int timeout_sec);

	/// <summary>
	/// Check WiFi connection and reconnect if lost.
	/// </summary>
	/// <returns>True if WiFi is connected.</returns>
	bool checkWifiConnection(String dateTime);

	// <summary>
	/// Returns string describing WiFi status.
	/// </summary>
	/// <returns>String describing WiFi status</returns>
	String wifiConnect_Status();

	/// <summary>
	/// Adds credentials for WiFi access points.
	/// </summary>
	void wifiAddAccessPoints();

	// <summary>
	/// Returns string listing connected WiFi SSID, RSSI, and IP adresss.
	/// </summary>
	/// <returns>String describing WiFi connection.</returns>
	String wifi_ConnectionInfo();

	// <summary>
	/// Lists WiFi networks that are in range. 
	/// </summary>
	/// <returns>Number of networks found.</returns>
	int wifiListNetworks();

	/// <summary>
	/// Connects to the WiFi network.
	/// </summary>
	void wifiSetupAndConnect(String dateTime, bool isDEBUG_BypassWiFi);
};

#endif
