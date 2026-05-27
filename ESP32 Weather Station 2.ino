/*****************************************
Weather-Star 12.0
Custom weather station control software.

Ralph Paonessa
August 10, 2022
Rev. May 5, 2026
******************************************/

// ========  ESP32 Libraries  ================  

#include <TimeLib.h>
#include "WiFiTools.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"	// for delay in loop???
#include <ESPAsyncWebServer.h>

// ========  Custom Libraries  ================  

#include "App_settings.h"
using namespace App_Settings;
#include "PinAssignments.h"
#include "Utilities.h"
using namespace Utilities;
#include "FileOperations.h"
using namespace FileOperations;
#include "GPSModule.h"
#include "SDCard.h"
//#include "dataPoint.h"
#include "SensorData.h"
#include "WindSpeed2.h"
#include "WindDirection.h"

/*****************      OPTION FLAGS      ******************/

const bool isRECOVER_FILESYS_DATA = false;			// Recover data from file system.

/*****************      DEBUGGING FLAGS      ******************/

const bool _isDEBUG_BypassGPS = true;				// Bypass gps syncing.
const bool _isDEBUG_BypassWifi = false;				// Bypass WiFi connect.	 XXX FAILS!!!
const bool _isDEBUG_BypassSDCard = false;			// Bypass SD card.
const bool _isDEBUG_ListLittleFS = true;			// List contents of LittleFS.
const bool _isDEBUG_BypassWebServer = false;		// Bypass Web Server.
const bool _isDEBUG_run_test_in_setup = true;		// Run only test code inserted in Setup.
const bool _isDEBUG_run_test_in_loop = false;		// Run test code inserted in Loop.
const bool _isDEBUG_addDummyDataLists = false;		// Add dummy data.
const bool _isDEBUG_simulateSensorReadings = false;	// Add dummy sensor reading values.
const bool _isDEBUG_simulateWindReadings = false;	// Add dummy wind reading values.
const bool _isDEBUG_AddDelayInLoop = false;			// Add delay in loop.
const int _LOOP_DELAY_DEBUG_ms = 100;				// Debug delay in loop, msec.


// ==========   SD card module   ==================== //
SDCard sd;		// SDCard instance that exposes SD card routines. 

//#if defined(VM_DEBUG)
#include "Testing.h"			// DEBUG AND TESTING
#include "SensorSimulate.h"
#include <WiFiMulti.h>
#include <esp32-hal-gpio.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal-timer.h>
#include <esp32-hal.h>
#include <HardwareSerial.h>
#include <WString.h>
#include <WiFi.h>
#include <WiFiType.h>
#include <esp_attr.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <cstddef>
//Testing test;					// class for test routings
//#endif

// ==========   CREATE SENSOR OBJECTS   ==================== //

SensorData d_TempF;					// Temp
SensorData d_Pres_mb(false);		// P					(Data not in filesys)
SensorData d_Pres_seaLvl_mb;		// P adjust to sea lvl
SensorData d_TempC_for_RH(false);	// Temp on P sensor		(Data not in filesys)
SensorData d_RH;					// Rel humidity
SensorData d_UVA(false);			// UVA					(Data not in filesys)
SensorData d_UVB(false);			// UVB					(Data not in filesys)
SensorData d_UVIndex;				// UV Index
SensorData d_Insol(true, true);		// Insolation			(Data not in filesys, no minima)
SensorData d_IRSky_C;				// IR sky temp
SensorData d_fanRPM(false);			// Fan RPM

/*
SensorData instances to average readings.
Wind speed handled by WindSpeed.
Wind direction handled by WindDirection.
*/

// WindSpeed instance for wind.
WindSpeed windSpeed(DAVIS_SPEED_CAL_FACTOR);
SensorData windGust;
WindDirection windDir(VANE_OFFSET);	// WindDirection instance for wind.

/// <summary>
/// Adds labels and units to SensorData instances.
/// </summary>
void sensors_AddLabels()
{
	windSpeed.addLabels("Wind Speed", "wind", "mph");
	windDir.addLabels("Wind direction", "windDir", "", "&deg;");
	windGust.addLabels("Wind Gust", "gust", "mph");
	d_TempF.addLabels("Temperature", "temp", "F", "&deg;F");
	d_Pres_mb.addLabels("Pressure (abs)", "presAbs", "mb");
	d_Pres_seaLvl_mb.addLabels("Pressure (SL)", "presSeaLvl", "mb");
	d_TempC_for_RH.addLabels("Temp for RH", "tForRH", "C", "&degC;");
	d_RH.addLabels("Rel. Humidity", "RH", "%", "&percnt;");
	d_IRSky_C.addLabels("Sky Temperature", "skyTemp", "C", "&degC;");
	d_UVA.addLabels("UV A Radiation", "uvA", "");
	d_UVB.addLabels("UV B Radiation", "uvB", "");
	d_UVIndex.addLabels("UV Index", "uvIndex", "");
	d_Insol.addLabels("Insolation", "sun", "%", "&percnt;");
	d_fanRPM.addLabels("Aspirator Fan speedInstant", "fanSpeed", "rpm");
}

/// <summary>
/// Creates data files for selected SensorData instances 
/// that save chart data on the file system.
/// </summary>
void sensors_createFiles()
{
	windSpeed.createFiles();
	windDir.createFiles();
	windGust.createFiles();
	d_TempF.createFiles();
	//d_Pres_mb.createFiles();
	d_Pres_seaLvl_mb.createFiles();
	//d_TempC_for_RH.createFiles(); 
	d_RH.createFiles();
	d_IRSky_C.createFiles();
	//d_UVA.createFiles();         
	//d_UVB.createFiles();         
	d_UVIndex.createFiles();
	d_Insol.createFiles();
	//d_fanRPM.createFiles();      
}


//#if defined(VM_DEBUG)
SensorSimulate dummy_Temp_F;			// Temperature readings.
SensorSimulate dummy_Pres_mb;			// Pressure readings.
SensorSimulate dummy_Pres_seaLvl_mb;	// Pressure readings.
SensorSimulate dummy_Temp_for_RH_C;		// Sensor temperature for pressure readings.
SensorSimulate dummy_RH;				// Rel. humidity readings.
SensorSimulate dummy_UVA;				// UVA readings.
SensorSimulate dummy_UVB;				// UVB readings.
SensorSimulate dummy_UVIndex;			// UV Index readings.
SensorSimulate dummy_Insol;				// Insolaton readings.
SensorSimulate dummy_IRSky_C;			// IR sky temperature readings.
SensorSimulate dummy_fanRPM;			// Fan RPM readings.

SensorSimulate dummy_anemCount;			// Anemometer rot count.
SensorSimulate dummy_windDir;			// Anemometer wind direction.
//#endif

// %%%%%%%%%%   STATUS FLAGS FOR DEVICES   %%%%%%%%%%%%%%%%
bool _isGood_Temp = false;
bool _isGood_PRH = false;
bool _isGood_UV = false;
bool _isGood_IR = false;
bool _isGood_WindDir = false;
bool _isGood_WindSpeed = false;
//bool _isGood_GPS = false;
bool _isGood_PMS = false;
//bool _isGood_SDCard = false;
bool _isGood_Solar = false;
bool _isGood_LittleFS = false;
bool _isGood_fan = false;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

unsigned long _time_start_current_loop;			//monitor loop timing
int _index = 0;	// Determines which sensor to read.
long int _time_start_first_loop;

/*
	WARNING: long int is 64-bit, which uses TWO 32-bit values.
	That would require portENTER_CRITICAL() when you READ the
	variable, not simply when you change it!
*/
volatile int _countInterrupts_base = 0;		// Number of accumulated timer interrupts for sensor reads.
volatile int _countInterrupts_10_min = 0;	// Number of accumulated timer interrupts for sensor for 10-min averages.
//volatile int _countInterrupts_60_min = 0;	// Number of accumulated timer interrupts for sensor for 60-min averages.

//// ==========   SD card module   ==================== //
//SDCard sd;		// SDCard instance that exposes SD card routines. 
// Moved to GPSModule library!

bool _isGood_SDCard = false;
bool _isGood_GPS = false;

// ==========   Async Web Server   ================== //
AsyncWebServer server(80);	// Async web server instance on port 80.

// ==========   WiFi network   ================== //
WiFiTools wifi;

// ==========   u-blox NEO-6M GPS   ========================== //
// GPS module instance. 
GPSModule gps;

int _oldMonth = 0;		// Month of previous day.
int _oldDay = 0;		// Day of previous day.
int _oldYear = 0;		// Year of previous day.

// ==========   PWM Fan for Radiation Shield  ======================== //

volatile unsigned long _fanHalfRots = 0;// count fan half-rotation (2 counts/cycle)

// HARDWARE INTERRUPT for fan tachometer switch that signals half-rotation
portMUX_TYPE hardwareMux_fan = portMUX_INITIALIZER_UNLOCKED;

// Function called by hardware interrupt when fan 
// tachometer switch closes on every half-rotation.
void IRAM_ATTR ISR_onFanHalfRotation() {
	portENTER_CRITICAL_ISR(&hardwareMux_fan);
	_fanHalfRots++;
	portEXIT_CRITICAL_ISR(&hardwareMux_fan);
}

// ==========   Davis anemometer - wind speed  ==================== //
/*
 Wind speed is determined by counting anemometer
 rotations over a brief base measurement period.
 Each rotation triggers a hardware interrupt that
 increments _anem_Rotations by 1.
*/
volatile unsigned int _anem_Rotations = 0;		// Count of anemometer rotations
unsigned long _lastDebounceTime = 0;			// Last millis when output pin was toggled
const unsigned int DEBOUNCE_TIMEOUT = 15;		// Debounce timeout (millisec)

/*
	INTERRUPT DEFINITIONS
		- TIMER interrupt to trigger sensor readings (every 4 sec)
		- HARDWARE interrupt to signal anemometer and fan rots
*/
// TIMER INTERRUPT to count anemometer and fan rotations.
hw_timer_t* timer_base = NULL;
portMUX_TYPE timerMux_base = portMUX_INITIALIZER_UNLOCKED;

/// <summary>
/// Timer interrupt service routine increments counts 
/// _countInterrupts_base, _countInterrupts_10_min, and 
/// _countInterrupts_60_min. Should happen every 4 sec 
/// (Base Period).
/// </summary>
void IRAM_ATTR ISR_onTimer_count() {
	portENTER_CRITICAL_ISR(&timerMux_base);
	_countInterrupts_base++;
	_countInterrupts_10_min++;
	//_countInterrupts_60_min++;
	portEXIT_CRITICAL_ISR(&timerMux_base);
}

// HARDWARE INTERRUPT that signals one anemometer rotation.
portMUX_TYPE hardwareMux_anem = portMUX_INITIALIZER_UNLOCKED;

// Function called by hardware interrupt when anemometer sensor switch closes.
// It increments the rotation count by 1, after switch debouncing.
void IRAM_ATTR ISR_onRotation_anem() {
	// Ignore any additional activations within the debounce time.
	if ((millis() - _lastDebounceTime) > DEBOUNCE_TIMEOUT) {
		portENTER_CRITICAL_ISR(&hardwareMux_anem);
		_anem_Rotations++;
		portEXIT_CRITICAL_ISR(&hardwareMux_anem);
		_lastDebounceTime = millis();
	}
}
// ========   END Davis Anemometer 6410  =================  //

// ========   Setup LittleFS   =================  //

/// <summary>
/// Initializes and mounts LittleFS filesystem. 
/// Logs mount success or failure, records used/available space, 
/// sets the _isGood_LittleFS flag on success, and optionally 
/// lists the root directory when debugging is enabled.
/// </summary>
void littleFS_initialize() {
	if (!LittleFS.begin()) {
		String msg = "ERROR: LittleFS didn't mount.";
		sd.logStatus(msg, millis());
	}
	else {
		_isGood_LittleFS = true;
		String msg = "LittleFS mounted.";
		sd.logStatus(msg, millis());
	}
	// Log used and available space.
	sd.logLittleFsSpaceUsage();

	if (_isDEBUG_ListLittleFS) {
		dirList(LittleFS, "/", 5);
	}
}


/// <summary>
/// Read recent sensor readings from LittleFS and load them back into memory.
/// /// 
/// XXX ONLY CODED FOR d_TempF!!!
/// /// 
/// <remarks>Used to recover recent data lost from memory after reboot.
/// 
/// XXX ONLY CODED FOR d_TempF!!!
/// 
/// </remarks>
/// </summary>
void recoverData() {
	unsigned long lastTime = lastReadingTime_fromFile();
	Serial.printf("recover_data(): lastReadingTime_fromFile = %lu\n", lastTime);

	// 10-min lists
	Serial.println("recover_data(): 10-min lists");
	if (d_TempF.isDatafile()
		&& (now() - lastTime) > DATA_RECOVERY_10_MIN_AGE_LIMIT)
	{
		d_TempF.recover_data_10_min_from_file();
		sd.logStatus("Recovered 10-min data.", millis());
	}

	// 60-min lists
	Serial.println("recover_data(): 60-min lists");
	if (d_TempF.isDatafile()
		&& (now() - lastTime) > DATA_RECOVERY_60_MIN_AGE_LIMIT)
	{
		d_TempF.recover_data_60_min_from_file();
		sd.logStatus("Recovered 60-min data.", millis());
	}

	// day lists
	Serial.println("recover_data(): day lists");
	if (d_TempF.isDatafile()
		&& (now() - lastTime) > DATA_RECOVERY_DAY_AGE_LIMIT)
	{
		d_TempF.recover_data_dayMaxMin_from_file();
		sd.logStatus("Recovered dayMaxMin data.", millis());
	}
}


/****************************************************************************/
/******************************      SETUP      *****************************/
/****************************************************************************/
void setup() {

	Serial.begin(115200);

	// Print status message now because SD is not yet online.
	String msg = "\n\n\n" + LINE_SEPARATOR_MAJOR + "\n";
	msg += String(millis() / 1000.);
	msg += "s ENTERING SETUP \nSD card not yet online.\n";
	msg += LINE_SEPARATOR_MAJOR + "\n\n";
	Serial.print(msg);

	//  SETUP: ==========  CREATE SD CARD   ========== //
	Serial.println("SETUP: ==========  CREATE SD CARD   ==========");
	// (Do this first - need SD card for logging.)
	_isGood_SDCard = sd.create(SPI_CS_PIN, _isDEBUG_BypassSDCard);
	// Begin status log entries to SD card.
	sd.logStatus();	// Empty line
	sd.logStatus(LINE_SEPARATOR_MAJOR);
	sd.logStatus("SETUP continues after SD card initialization.", gps.dateTime());
	sd.fileCreateOrExists(LOGFILE_PATH_DATA);
	sd.fileCreateOrExists(LOGFILE_PATH_STATUS);

	// Log the settings to the status file.
	sd.logDebugStatus(
		_isDEBUG_BypassGPS,
		_isDEBUG_BypassWifi,
		_isDEBUG_BypassSDCard,
		_isDEBUG_ListLittleFS,
		_isDEBUG_BypassWebServer,
		_isDEBUG_run_test_in_setup,
		_isDEBUG_run_test_in_loop,
		_isDEBUG_addDummyDataLists,
		_isDEBUG_simulateSensorReadings,
		_isDEBUG_simulateWindReadings,
		_isDEBUG_AddDelayInLoop,
		_LOOP_DELAY_DEBUG_ms);
	sd.logApp_Settings();

	//  SETUP: ==========  CREATE WIFI NETWORK   ========== //	
	Serial.println("SETUP: ==========  CREATE WIFI NETWORK   ==========");
	wifi.Initialize(sd);

	//XXX Should _isDEBUG_BypassWifi be a parameter??
	//XXX Or should we just skip wifiSetupAndConnect() if it's true?!
	wifi.wifiSetupAndConnect(gps.dateTime(), _isDEBUG_BypassWifi);

	//  ==========  CREATE ASYNC WEB SERVER   ========== //	
	Serial.println("SETUP: ==========  CREATE ASYNC WEB SERVER   ==========");
	serverRouteHandler();	// Define routes for server requests.
	sd.logStatus("Async web server routes defined.", millis());
	server.begin();			// Start async web server.
	sd.logStatus("Async web server beginning.", millis());

	// ==========   CREATE GPS AND SYNC TO GET TIME   ========== //
	Serial.println("SETUP: ==========  CREATE GPS AND SYNC TO GET TIME   ==========");
	// XXX  Need code to alter power of GPS!!!  XXX

	/* 	Format for setting s serial port:
		SerialObject.begin(baud-rate, protocol, RX pin, TX pin);
		(Wrong baud rate gives serial garbage.)
	*/
	// Connect to GPS

#define SERIAL_CONFIGURATION SERIAL_8N1	// data, parity, and stop bits
	//const int GPS_BAUD_RATE = 9600;     // Beitian = 9600; Brian's = 38400; NEO-6M 9600
	gps.begin(GPS_BAUD_RATE, SERIAL_CONFIGURATION, RX2_PIN, TX2_PIN);
	sd.logStatus("Connecting to GPS.", millis());
	// Get time and location from GPS.
	// This code is BLOCKING until gps syncs.
	bool isGpsSuccess = false;
	if (!gps.isSynced())
	{
		isGpsSuccess = gps.syncToGPS(sd, _isDEBUG_BypassGPS);
		_isGood_GPS = true;
	}
	if (!isGpsSuccess)
	{
		String msg = "ERROR: GPS did not sync after ";
		msg += String(GPS_CYCLES_COUNT_MAX) + " cycles.";
		sd.logStatus(msg, millis());
	}


	// ==========  SETUP LittleFS  ========== //
	Serial.println("SETUP: ==========  SETUP LittleFS   ==========");
	littleFS_initialize();


	//#if defined(VM_DEBUG)
	////////  TESTING   ////////
	Serial.println("SETUP: ==========  TESTING   ==========");
	if (_isDEBUG_addDummyDataLists) {
		Serial.println("SETUP: ==========  ADD DUMMY DATA   ==========");
		addDummyData();
		saveLastReadTime_toFile(now());
		Serial.println();
		Serial.println("XXX  saveLastReadTime_toFile(now())  XXX");
		Serial.println();
	}

	if (_isDEBUG_run_test_in_setup) {
		bool isDataInFilesys = true;
		bool isReportDailyMaxOnly = false;
		SensorData d(isDataInFilesys, isReportDailyMaxOnly);
		d.addLabels("Test sensor", "testPrefix", "units");
		d.createFiles(false, 2);

		unsigned long int t = 0;
		float val = 50;
		float increment = 5;
		for (size_t i = 0; i < 10; i++)
		{
			t += SECONDS_PER_DAY;	// Increment time 1 day.
			val += increment;
			d.addReading(dataPoint(t, val));
			
			Serial.printf("(%li, %f) countReadings() = %i\n",t, val, d.countReadings());

			delay(1000);

		}
				
		Serial.printf("avg_now() = %f\n", d.avg_now());
		Serial.printf("max_10_min() = %f\n", d.max_10_min().value);
		Serial.printf("min_10_min() = %f\n", d.min_10_min().value);

		//// File contents.
		//Testing::.testCodeForSetup_printFileContents(false, "/Sensor data/RH_10_min.txt");
		//Testing::.testCodeForSetup_printFileContents(false, "/Sensor data/wind_10_min.txt");
		//Testing::.testCodeForSetup_printFileContents(false, "/Sensor data/windDir_10_min.txt");
		//Testing::.testCodeForSetup_printFileContents(false, "/Sensor data/d_Temp_F_10_min.txt");
		//Testing::.testCodeForSetup_printFileContents(true, "/Sensor data/skyTemp_10_min.txt");

		/*Testing::testCodeForSetup_convert_DelimString_to_ListOfDataPoints(false);
		Testing::testCodeForSetup_list_dataPoints_fromString(false);
		Testing::testCodeForSetup_getStringList_from_String(true);*/

		Testing::infiniteLoop();
	}
	//#endif


	// ==========  CREATE SENSORS  ========== //

	Serial.println("SETUP: ==========  CREATE SENSORS   ==========");
	sensors_AddLabels();	// Add labels and units to the SensorData instances.
	sensors_begin();
	sensors_createFiles();

	// ==========  RECOVER DATA   ==========
	if (isRECOVER_FILESYS_DATA) {
		Serial.println("SETUP: ==========  RECOVER DATA FROM FILE SYSTEM   ==========");
		// Retrieve recent saved data from LittleFS. (In case of inadvertent reboot.)
		recoverData();
	}
	else {
		Serial.println("SETUP: BYPASS RECOVER DATA FROM FILE SYSTEM");
	}

	// Date info to determine when new day begins.
	_oldDay = day();
	_oldMonth = month();
	_oldYear = year();


	sd.logData(columnNames());	// Write column names to data log.
	sd.logStatus_indent("DATA COLUMNS:\t" + columnNames());	// Write column names to status log.

	/// ==========  CONFIGURE FAN PWM  ========== //
	const int FAN_PWM_FREQUENCY = 25000;	// 25kHz
	const int FAN_PWM_CHANNEL = 0;
	const int FAN_PWM_RESOLUTION = 8;

	// VERSION FOR ESP32 2.X ...
	ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	// Attach the channel to the pin for PWM output.
	ledcAttachPin(FAN_PWM_PIN, FAN_PWM_CHANNEL);
	// Set fan speed using PWM.
	ledcWrite(FAN_PWM_CHANNEL, FAN_DUTY_PERCENT / 100. * 256);
	/*
	VERSION FOR ESP32 3.X ...
	ledcAttach(FAN_PWM_PIN, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	ledcWrite(FAN_PWM_PIN, FAN_DUTY_PERCENT / 100. * 256);
	*/

	// Create interrupts after everything else has completed.

	// ==========  CREATE ANEMOMETER HARDWARE INTERRUPT  ========== //
	Serial.println("SETUP: ==========  CREATE ANEMOMETER HARDWARE INTERRUPT   ==========");
	// Anemometer hardware interrupt for detecting rotation.
	pinMode(WIND_SPEED_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN),
		ISR_onRotation_anem,
		FALLING);
	// Fan hardware interrupt to detect half-rotation.
	pinMode(FAN_SPEED_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(FAN_SPEED_PIN), ISR_onFanHalfRotation, FALLING);

	// ==========  CREATE TIMER INTERRUPT  ========== //
	Serial.println("SETUP: ==========  CREATE TIMER INTERRUPT   ==========");
	/*
	 Timer interrupt fires every BASE_PERIOD_SEC (4 sec) to
	 trigger counts of anemometer and fan rotations.
	 This is ALSO when we record all the other sensor readings.
	 ONLY WIND SPEED NEEDS TO BE RECORDED THIS FREQUENTLY.
	 [NOTE: Consider spacing out the other sensor readings.]
	*/
	// Compatible with ESP32 2.X:
	timer_base = timerBegin(0, 80, true);
	timerAttachInterrupt(timer_base, &ISR_onTimer_count, true);
	int duration_count = BASE_PERIOD_SEC * MICROSEC_PER_SECOND;	// Timer duration (microsec).
	timerAlarmWrite(timer_base, duration_count, true);		// Trigger every BASE_PERIOD_SEC.
	timerAlarmEnable(timer_base);
	/*
	VERSION FOR ESP32 3.X ...
	int duration_count = BASE_PERIOD_SEC * MICROSEC_PER_SECOND;	// Timer duration (microsec).
	timer_base = timerBegin(duration_count);
	timerAttachInterrupt(timer_base, &ISR_onTimer_count);
	timerAlarm(timer_base, duration_count, true, 0);		// Trigger every BASE_PERIOD_SEC.
	*/

	_time_start_first_loop = millis();

	Serial.println("SETUP: ==========  COMPLETED CREATE TIMER INTERRUPT   ==========");

	msg = "CURRENT LOCAL TIME is " + gps.dateTime();
	(IS_DAYLIGHT_TIME) ? msg = " Daylight time." : msg = " Standard time.";
	sd.logStatus(msg);
	sd.logStatus("SETUP END " + gps.dateTime(), millis());
}
/****************************************************************************/
/************************        END SETUP       ****************************/
/****************************************************************************/



/****************************************************************************/
/***************************       LOOP      ********************************/
/****************************************************************************/
void loop() {
	_time_start_current_loop = millis();	// To monitor loop execution time.

	//// Check for timer interrupts that were 
	//// not handled during code delays.
	//catchUnhandledBaseTimerInterrupts();

	/************************************************
		Read sensors and process data at intervals
		determined from timer interrupt.
	*************************************************/

	/*
	 After every 4 sec Base Period, get anemometer counts as well as
	 all sensor values. Use values to accumulate 10-min averages.
	 (This is ESSENTIAL for anemometer wind speed, which counts
	 rotations for every Base Period. It is CONVENIENT for timing
	 reading of other sensors.

	 At 10-min intervals, save 10-min averages and start
	   accumulating next 10-min avg.

	   At 60-min intervals, save 60-min averages and
	   start accumulating next 60-min avg.
	*/

	// ====================================================
	//		BASE_PERIOD_SEC (4 sec)
	// ====================================================	



	//XXX  if (_countInterrupts_base == 1) {    XXX
	if (_countInterrupts_base > 0) {
		// Read sensors and process data.
		readWind();
		readFan();
		// Read data for other sensors one at a time.
		readSensors(_index);
		_index++;
		if (_index > COUNT_SENSORS_TO_READ - 1) {
			_index = 0;		// Restart cycle.
		};
		portENTER_CRITICAL_ISR(&timerMux_base);
		_countInterrupts_base--;	// Base timer interrupt handled.
		portEXIT_CRITICAL_ISR(&timerMux_base);
	}

	//// ====================================================
	////		10-MIN INTERVAL
	//// ====================================================

	if (_countInterrupts_10_min >= BASE_PERIODS_IN_10_MIN) {
		// Get 10-min avgs.
		processReadings_10_min();
		sd.logData(sensorsDataString_10_min());	// Save readings to SD card.
		sd.logStatus("Logged 10-min avgs.", gps.dateTime());
		// Check for unhandled interrupts.
		if (_countInterrupts_10_min > BASE_PERIODS_IN_10_MIN)	// XXX  WRONG!!!!!!
		{
			String msg = "WARNING: 10-min interrupt count exceeded threshold by ";
			msg += String(_countInterrupts_10_min - BASE_PERIODS_IN_10_MIN);
			msg += " indicating unhandled 10-min timer interrupt.";
			sd.logStatus(msg, gps.dateTime());
		}
		portENTER_CRITICAL_ISR(&timerMux_base);
		_countInterrupts_10_min = 0;	// Interrupt handled.
		portEXIT_CRITICAL_ISR(&timerMux_base);
	}

	//// ====================================================
	////		60-MIN INTERVAL
	//// ====================================================   

	//if (_countInterrupts_60_min >= BASE_PERIODS_IN_60_MIN) {
	//	processReadings_60_min();
	//	sd.logData(sensorsDataString_10_min());	// Save readings to SD card.
	//	sd.logStatus("Logged 60-min avgs.", gps.dateTime());
	//	// Check for unhandled.
	//	if (_countInterrupts_60_min > BASE_PERIODS_IN_60_MIN) {
	//		String msg = "WARNING: 10-min interrupt count exceeded threshold by ";
	//		msg += String(_countInterrupts_60_min - BASE_PERIODS_IN_60_MIN);
	//		msg += " counts, indicating unhandled 10-min timer interrupt.";
	//		sd.logStatus(msg, gps.dateTime());
	//	}
	//	portENTER_CRITICAL_ISR(&timerMux_base);
	//	_countInterrupts_60_min--;	// Interrupt handled.
	//	portEXIT_CRITICAL_ISR(&timerMux_base);
	//}

	//// ====================================================
	////  CHECK FOR NEW DAY
	//if (day() > _oldDay || month() > _oldMonth || year() > _oldYear) {
	//	// NEW DAY. 
	//	// Save minima and maxima for previous day.
	//	processReadings_day();
	//	_oldDay = day();
	//	_oldMonth = month();
	//	_oldYear = year();
	//	sd.logStatus("New day rollover.", gps.dateTime());
	//}

	///// ==========  TEST FOR LOST WIFI CONNECTION  ========== //
	///*
	//If WiFi is lost, we're screwed because the time
	//to reconnect may throw of the sensor read timings.
	//Just bite the bullet and take the time to reconnect,
	//then recover.

	//If WiFi was lost, the time to reconnect will cause
	//the timer interrupt counts to increment beyond where
	//they should have been handled.
	//*/
	//if (!_isDEBUG_BypassWifi) {
	//	if (WiFi.status() != WL_CONNECTED) {
	//		wifi.checkWifiConnection(gps.dateTime());

	//		// XXX Moved here from WiFiTools::checkWifiConnection()
	//		resetInterruptCounts();
	//		String msg = "Read cycle skipped after WiFi was lost.";
	//		sd.logStatus(msg, gps.dateTime());
	//	}
	//}

	//#if defined(VM_DEBUG)
		// Add delay for DEBUG.
	if (_isDEBUG_AddDelayInLoop) {
		vTaskDelay(_LOOP_DELAY_DEBUG_ms / portTICK_PERIOD_MS);
	}
	//#endif

		// Watch for excessive processing time in loop.
	if (millis() - _time_start_current_loop > LOOP_TIME_WARN_THRESHOLD_MS) {
		String msg = "Loop " + String(millis() - _time_start_current_loop) + "ms";
		sd.logStatus(msg, gps.dateTime());
		//Serial.printf("long loop %lims\n", millis() - _time_start_current_loop);
	}
}
/******************************        END LOOP        **********************************/
/****************************************************************************************/
/****************************************************************************************/
