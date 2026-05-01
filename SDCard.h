// SDCard.h

#ifndef _SDCARD_h
#define _SDCARD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <FS.h>
#include <FSImpl.h>
#include <vfs_api.h>

#include <SD.h>
#include <sd_defines.h>
#include <sd_diskio.h>

#include <SPI.h>

#include "Utilities.h"

#include "App_Settings.h"
using namespace App_Settings;

#include "FileOperations.h"
using namespace FileOperations;

/// <summary>
/// Exposes methods to read/write files to an SD card.
/// </summary>
class SDCard {

private:
	int _SPI_CS_pin;	// SPI pin number for SD card.
	bool _isBypassSDCard = false;

public:
	/// <summary>
	/// Creates SD card instance. 
	/// </summary>
	SDCard();

	/// <summary>
	/// Creates SD card module.
	/// </summary>
	/// <param name="SPI_CS_pin">GPIO pin number.</param>
	/// <param name="isBypass">Set true to bypass SD card operations.</param>
	/// <returns>True if successful SD card initialization.</returns>
	bool create(int SPI_CS_pin, bool isBypass);

	/// <summary>
	/// Create the file if it does not exist.
	/// </summary>
	/// <param name="path">The file path including name.</param>
	/// <returns>True if file found or created.</returns>
	bool fileCreateOrExists(const String& path);

	/// <summary>
	/// Appends data to the SD card, and advances to new line.
	/// </summary>
	/// <param name="logEntry">String to write.</param>
	void logData(String logEntry);

	/// <summary>
	/// Writes blank line to status log (and serial monitor if VM_DEBUG).
	/// </summary>
	void logStatus();

	/// <summary>
	/// Writes line to status log (and serial monitor if VM_DEBUG).
	/// </summary>
	/// <param name="msg">Message to log.</param>
	void logStatus(const String& msg);

	/// <summary>
	/// Writes indented line to status log 
	/// (and serial monitor if VM_DEBUG).
	/// </summary>
	/// <param name="msg">Message to log.</param>
	void logStatus_indent(const String& msg);

	/// <summary>
	/// Writes line prefixed by date to status 
	/// log (and serial monitor if VM_DEBUG).
	/// </summary>
	/// <param name="msg">Message to log.</param>
	/// <param name="dateString">Date and time string to include.</param>
	void logStatus(const String& msg, const String& dateString);

	/// <summary>
	/// Writes line prefixed by seconds since start to
	/// status log (and serial monitor if VM_DEBUG).
	/// </summary>
	/// <param name="msg">Message to log.</param>
	/// <param name="millisec">Milliseconds since start).</param>
	void logStatus(const String& msg, unsigned long millisec);
		
	// <summary>
	/// Logs summary of devices status.
	/// </summary>
	void logDeviceStatus(
		String gpsDateTime,
		bool isDaylightTime,
		//bool wiFi_isConnected,
		int gpsTimeZoneOffset,
		bool isGood_SDCard,
		bool isGood_LittleFS,
		bool isGood_GPS,
		bool isGood_Temp,
		bool isGood_PRH,
		bool isGood_UV);

	// <summary>
	/// Logs state of debug options.
	/// </summary>
	void logDebugStatus(
		bool isDEBUG_BypassGPS,
		bool isDEBUG_BypassWifi,
		bool isDEBUG_BypassSDCard,
		bool isDEBUG_ListLittleFS,
		bool isDEBUG_BypassWebServer,
		bool isDEBUG_run_test_in_setup,
		bool isDEBUG_run_test_in_loop,
		bool isDEBUG_addDummyDataLists,
		bool isDEBUG_simulateSensorReadings,
		bool isDEBUG_simulateWindReadings,
		bool isDEBUG_AddDelayInLoop,
		bool LOOP_DELAY_DEBUG_ms);

	// <summary>
	/// Logs space used by LittleFS.
	/// </summary>
	void logLittleFsSpaceUsage();

	// <summary>
	/// Logs the application settings to the STATUS file.
	/// </summary>
	void logApp_Settings();

};

#endif
