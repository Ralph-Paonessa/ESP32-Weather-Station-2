#include "SDCard.h"

/// <summary>
/// Creates SD card instance. 
/// </summary>
SDCard::SDCard() {}

/// <summary>
/// Creates SD card module.
/// </summary>
/// <param name="SPI_CS_pin">GPIO pin number.</param>
/// <param name="isBypass">
/// Set true to bypass SD card operations and add 
/// dummy data.</param>
/// <returns>True if successful SD card initialization.</returns>
bool SDCard::create(int SPI_CS_pin, bool isBypass) {

	_SPI_CS_pin = SPI_CS_pin;

	_isBypassSDCard = isBypass;
	if (isBypass) {
		// Bypass initializing SD card		
		Serial.print(millis() / 1000.);
		Serial.println("s [SDCard.create] BYPASS SD card.");
		return false;
	}
	// Create SD card
	Serial.print(millis() / 1000.);
	Serial.println("s [SDCard.create] Initializing SD card.");
	if (SD.begin(_SPI_CS_pin)) {
		// Success.
		logStatus(); logStatus();	// empty lines
		logStatus(LINE_SEPARATOR_LOG_BEGINS);
		logStatus("[SDCard.create] Logging has started.", millis());
		logStatus("[SDCard.create] MicroSD card mount successful.", millis());
		return true;
	}
	else {
		// Failure.
		logStatus("[SDCard.create] ERROR: MicroSD card mount failed.", millis());
		return false;
	}
}

/// <summary>
/// Appends a line of data to the DATA file.
/// </summary>
/// <param name="msg">String to write.</param>
void SDCard::logData(String msg) {
#if defined(VM_DEBUG)
	Serial.println(msg);     // Echo to serial monitor
#endif
	if (!_isBypassSDCard) {
		String status = msg + "\r\n";	// Append CR + LF.
		fileAppend(SD, LOGFILE_PATH_DATA.c_str(), status.c_str());
	}
}

/// <summary>
/// Writes indented line to status log 
/// (and serial monitor if VM_DEBUG).
/// </summary>
/// <param name="msg">Message to log.</param>
void SDCard::logStatus_indent(const String& msg) {
#if defined(VM_DEBUG)
	Serial.println("\t" + msg);     // Echo to serial monitor
#endif
	if (!_isBypassSDCard) {
		String status = "\t" + msg + "\r\n";	// Append CR + LF.
		fileAppend(SD, LOGFILE_PATH_STATUS.c_str(), status.c_str());
	}
}

/// <summary>
/// Writes blank line to status log (and serial monitor if VM_DEBUG).
/// </summary>
void SDCard::logStatus() {
#if defined(VM_DEBUG)
	Serial.println();	// Echo to serial monitor
#endif
	if (!_isBypassSDCard) {
		fileAppend(SD, LOGFILE_PATH_STATUS.c_str(), "\r\n");
	}
}

/// <summary>
/// Writes line to status log (and serial monitor if VM_DEBUG).
/// </summary>
/// <param name="msg">Message to log.</param>
void SDCard::logStatus(const String& msg) {
#if defined(VM_DEBUG)
	Serial.println(msg);	// Echo to serial monitor
#endif
	if (!_isBypassSDCard) {
		String status = msg + "\r\n";	// Append CR + LF.
		fileAppend(SD, LOGFILE_PATH_STATUS.c_str(), status.c_str());
	}
}

/// <summary>
/// Writes line prefixed by date to status 
/// log (and serial monitor if VM_DEBUG).
/// </summary>
/// <param name="msg">Message to log.</param>
/// <param name="dateString">Date and time string to include.</param>
void SDCard::logStatus(const String& msg, const String& dateString) {
	String status = dateString + " ";
	status += msg;
#if defined(VM_DEBUG)
	Serial.println(status);	// Echo to serial monitor
#endif
	if (!_isBypassSDCard) {
		status += "\r\n";	// Append CR + LF.
		fileAppend(SD, LOGFILE_PATH_STATUS.c_str(), status.c_str());
	}
}

/// <summary>
/// Writes message line, prefixed by seconds since start,
/// to status log (and serial monitor if VM_DEBUG).
/// </summary>
/// <param name="msg">Message to log.</param>
/// <param name="millisec">Milliseconds since start).</param>
void SDCard::logStatus(const String& msg, unsigned long millisec) {
	String status = String(millisec / 1000., 2) + "s ";
	status += msg;
#if defined(VM_DEBUG)
	Serial.println(status);	// Echo to serial monitor
#endif
	if (!_isBypassSDCard) {
		status += "\r\n";	// Append CR + LF.
		fileAppend(SD, LOGFILE_PATH_STATUS.c_str(), status.c_str());
	}
}



// Returns true on success
// or if the file already exists; otherwise
// returns false.


/// <summary>
/// Creates a file on the SD card if it doesn't
/// already exist. 
/// </summary>
/// <param name="path">File path.</param>
/// <returns>True is new file created, false if </returns>
bool SDCard::fileCreateOrExists(const String& path) {
	if (_isBypassSDCard) {
		return false;
	}
	// File does not exist, so create empty file.
	File file = SD.open(path, FILE_WRITE);
	if (!file) {
		String msg = "[SDCard.fileCreateOrExists] " + path + " file could not be created.";
		logStatus(msg, millis());
		return false;
	}
	else {
		String msg = "[SDCard.fileCreateOrExists] " + path + " file created.";
		logStatus(msg, millis());
		file.close();
		return true;
	}
}

/// <summary>
/// Logs summary of devices status.
/// </summary>
void SDCard::logDeviceStatus(
	String gpsDateTime,
	bool isDaylightTime,
	//bool wiFi_isConnected,
	int gpsTimeZoneOffset,
	bool isGood_SDCard,
	bool isGood_LittleFS,
	bool isGood_GPS,
	bool isGood_Temp,
	bool isGood_PRH,
	bool isGood_UV) {
	String msg = "DEVICE STATUS REPORT:";
	logStatus(msg, gpsDateTime);

	msg = "WiFi connected: XXX THIS IS NOT CODED YET!!";
	//msg += Utilities::bool_OK_Error(wiFi_isConnected);
	logStatus_indent(msg);

	msg = "SD card: ";
	msg += Utilities::bool_OK_Error(isGood_SDCard);
	logStatus_indent(msg);

	msg = "LittleFS flash file system: ";
	msg += Utilities::bool_OK_Error(isGood_LittleFS);
	logStatus_indent(msg);

	msg = "GPS module - ";
	msg += Utilities::bool_OK_Error(isGood_GPS);
	logStatus_indent(msg);

	msg = "Dallas temperature sensor: ";
	msg += Utilities::bool_OK_Error(isGood_Temp);
	logStatus_indent(msg);

	msg = "Radiation shield fan: ";
	msg += "???";
	//msg += bool_OK_Error(isGood_fan);
	logStatus_indent(msg);

	msg = "Pressure & RH sensor: ";
	msg += Utilities::bool_OK_Error(isGood_PRH);
	logStatus_indent(msg);

	msg = "Insolation sensor: ";
	msg += "???";
	logStatus_indent(msg);

	msg = "UV sensor: ";
	msg += Utilities::bool_OK_Error(isGood_UV);
	logStatus_indent(msg);

	msg = "Wind direction sensor: ";
	msg += "???";
	logStatus_indent(msg);

	msg = "Wind speed sensor: ";
	msg += "???";
	logStatus_indent(msg);

	msg = "Time zone offset from UTC: ";
	msg += gpsTimeZoneOffset;
	logStatus_indent(msg);

	msg = "Is Daylight Time: : ";
	msg += Utilities::bool_true_false(isDaylightTime);
	logStatus_indent(msg);
}

/// <summary>
/// Logs state of debug options.
/// </summary>
void SDCard::logDebugStatus(
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
	bool LOOP_DELAY_DEBUG_ms) {
#if defined(VM_DEBUG)
	logStatus("Compiled as DEBUG BUILD");
#else
	logStatus("Compiled as RELEASE BUILD");
	Serial.print("Compiled as RELEASE BUILD\n");
#endif
	logStatus(LINE_SEPARATOR);
	logStatus("DEBUG FLAGS SET:");
	if (isDEBUG_BypassGPS) {
		logStatus_indent("BYPASS GPS");
	}
	if (isDEBUG_BypassWifi) {
		logStatus_indent("BYPASS WIFI");
	}
	if (isDEBUG_ListLittleFS) {
		logStatus_indent("LIST LittleFS CONTENTS");
	}
	if (isDEBUG_BypassSDCard) {
		Serial.println("BYPASS SD CARD");	// Can't log to SD!
	}
	if (isDEBUG_BypassWebServer) {
		logStatus_indent("BYPASS WEB SERVER");
	}
	if (isDEBUG_run_test_in_setup) {
		logStatus_indent("RUN TEST CODE IN SETUP");
	}
	if (isDEBUG_run_test_in_loop) {
		logStatus_indent("RUN TEST CODE IN LOOP");
	}
	if (isDEBUG_addDummyDataLists) {
		logStatus_indent("ADD DUMMY DATA");
	}
	if (isDEBUG_simulateSensorReadings) {
		logStatus_indent("USE DUMMY DATA FOR SENSOR READINGS");
	}
	if (isDEBUG_simulateWindReadings) {
		logStatus_indent("USE DUMMY DATA FOR WIND READINGS");
	}
	if (isDEBUG_AddDelayInLoop) {
		String msg = String(LOOP_DELAY_DEBUG_ms);
		msg += " ms DELAY ADDED IN LOOP";
		logStatus_indent(msg);
	}
	logStatus_indent("End of debug flags");
	logStatus(LINE_SEPARATOR);
}

/// <summary>
/// Logs space used by LittleFS.
/// </summary>
void SDCard::logLittleFsSpaceUsage() {
	String msg = "LittleFS Total space: ";
	msg += String(LittleFS.totalBytes() / 1000.);
	msg += " KB";
	logStatus_indent(msg);
	msg = "LittleFS space used: ";
	msg += String(LittleFS.usedBytes() / 1000.);
	msg += " KB";
	logStatus_indent(msg);
	msg = "LittleFS space available: ";
	msg += String((LittleFS.totalBytes() - LittleFS.usedBytes()) / 1000.);
	msg += " KB";
	logStatus_indent(msg);
}

/// <summary>
/// Logs the application settings to the STATUS file.
/// </summary>
void SDCard::logApp_Settings() {
	String msg = "APP SETTINGS:";
	logStatus(msg);
	msg = "BASE_PERIOD_SEC: " + String(BASE_PERIOD_SEC);
	msg += " sec";
	logStatus_indent(msg);
	msg = "GPS_CYCLES_FOR_SYNC: " + String(GPS_CYCLES_FOR_SYNC);
	logStatus_indent(msg);
	msg = "GPS_CYCLES_COUNT_MAX: " + String(GPS_CYCLES_COUNT_MAX);
	logStatus_indent(msg);
	msg = "GPS_DELAY_BETWEEN_CYCLES: " + String(GPS_CYCLE_DELAY_SEC);
	logStatus_indent(msg);
	msg = "GPS_MAX_ALLOWED_HDOP: " + String(GPS_MAX_ALLOWED_HDOP);
	logStatus_indent(msg);
	msg = "UTC_OFFSET_HOURS: " + String(UTC_OFFSET_HOURS);
	logStatus_indent(msg);
	msg = "IS_DAYLIGHT_TIME: " + String(IS_DAYLIGHT_TIME);
	logStatus_indent(msg);
	msg = "WIFI_CONNECT_TIMEOUT_SEC: " + String(WIFI_CONNECT_TIMEOUT_SEC);
	logStatus_indent(msg);
	msg = "LOGFILE_PATH_DATA: " + String(LOGFILE_PATH_DATA);
	logStatus_indent(msg);
	msg = "LOGFILE_PATH_STATUS: " + String(LOGFILE_PATH_STATUS);
	logStatus_indent(msg);
	msg = "INSOL_REFERENCE_MAX: " + String(INSOL_REFERENCE_MAX);
	logStatus_indent(msg);
	msg = "DAVIS_SPEED_CAL_FACTOR: " + String(DAVIS_SPEED_CAL_FACTOR);
	logStatus_indent(msg);
	msg = "VANE_OFFSET: " + String(VANE_OFFSET);
	logStatus_indent(msg);
	msg = "WIND_DIRECTION_SPEED_THRESHOLD: " + String(WIND_DIRECTION_SPEED_THRESHOLD);
	logStatus_indent(msg);
	msg = "LOOP_TIME_WARNING_THRESHOLD_MS: " + String(LOOP_TIME_WARNING_THRESHOLD_MS);
	logStatus_indent(msg);
}
