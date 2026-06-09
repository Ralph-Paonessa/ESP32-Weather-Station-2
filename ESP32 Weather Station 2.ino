/*****************************************
Weather-Star 12.0
Custom weather station control software.

Ralph Paonessa
August 10, 2022
Rev. May 29, 2026
******************************************/

// ========  ESP32 Libraries  ================  

#include <TimeLib.h>
#include "WiFiTools.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// ========  Custom Libraries  ================  

#include "App_settings.h"
#include "DebugFlags.h"
using namespace App_Settings;
#include "PinAssignments.h"
#include "Utilities.h"
using namespace Utilities;
#include "FileOperations.h"
using namespace FileOps;
#include "GPSModule.h"
#include "SDCard.h"
//#include "DataPoint.h"
#include "SensorData.h"
#include "WindSpeed2.h"
#include "WindDirection.h"

// ==========   SD card module   ==================== //
SDCard sd;		// SDCard instance for SD card routines. 

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
#include <SD.h>
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
SensorData windGust;
WindSpeed windSpeed(DAVIS_SPEED_CAL_FACTOR);	// Wind speed
WindDirection windDir(VANE_OFFSET);	// Wind direction

/// <summary>
/// Adds labels and units to SensorData instances.
/// </summary>
void sensors_AddLabels() {
	windSpeed.addLabels("Wind Speed", "wind", "mph");
	windDir.addLabels("Wind direction", "windDir", "", "&deg;");
	windGust.addLabels("Wind Gust", "gust", "mph");
	d_TempF.addLabels("Temperature", "temp", "F", "&deg;F");
	d_Pres_mb.addLabels("Pressure (station)", "presSta", "mb");
	d_Pres_seaLvl_mb.addLabels("Pressure (MSLP)", "presSeaLvl", "mb");
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
/// Creates filesys data files for selected SensorData instances. 
/// </summary>
void sensors_createFiles() {
	windSpeed.createSensorDataFiles();
	windDir.createSensorDataFiles();
	windGust.createSensorDataFiles();
	d_TempF.createSensorDataFiles();
	//d_Pres_mb.createSensorDataFiles();
	d_Pres_seaLvl_mb.createSensorDataFiles();
	//d_TempC_for_RH.createSensorDataFiles(); 
	d_RH.createSensorDataFiles();
	d_IRSky_C.createSensorDataFiles();
	//d_UVA.createSensorDataFiles();         
	//d_UVB.createSensorDataFiles();         
	d_UVIndex.createSensorDataFiles();
	d_Insol.createSensorDataFiles();
	//d_fanRPM.createSensorDataFiles();  

	// File to hold most recent sensor read time.
	FileStatus status = FileOps::fileCreateOrExists(LittleFS, SENSOR_LAST_SAVE_TIME_FILEPATH_FS);
	sd.logStatus("Status of " + SENSOR_LAST_SAVE_TIME_FILEPATH_FS + " = " + fileStatus_toString(status), millis());
}

#if defined(VM_DEBUG)
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
#endif

// %%%%%%%%%%   STATUS FLAGS FOR DEVICES   %%%%%%%%%%%%%%%%
bool isGood_Temp = false;
bool isGood_PRH = false;
bool isGood_UV = false;
bool isGood_IR = false;
bool isGood_WindDir = false;
bool isGood_WindSpeed = false;
//bool isGood_GPS = false;
bool isGood_PMS = false;
//bool isGood_SDCard = false;
bool isGood_Solar = false;
bool isGood_LittleFS = false;
bool isGood_fan = false;
bool isGood_SDCard = false;
bool isGood_GPS = false;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

unsigned long time_start_current_loop;		//monitor loop timing
long int time_start_first_loop;
int sensor_idx = 0;		// Determines which sensor to read.
int sensor_idx_10 = 0;	// Determines which sensor to process at 10 min.
int sensor_idx_60 = 0;	// Determines which sensor to process at 60 min.
int sensor_idx_day = 0;	// Determines which sensor to process at day rollover.

unsigned long last_debounce_time_anem = 0;	// Last millis when anemometer output pin was toggled

// INITIALIZE ANEMOMETER INTERRUPTS

volatile uint32_t _anemTimer_ticks = 0;// Count unhandled anemometer timer interrupts
volatile uint32_t _anemCount = 0;				// Count of anemometer rotations (volatile)

uint32_t anemRots = 0;							// Count of anemometer rotations

// Timer to trigger anemometer rotations count.
hw_timer_t* timerAnem = NULL;

// FreeRTOS spinlock (port multiplexer) to protect 
// shared data from concurrent access.
portMUX_TYPE muxAnem = portMUX_INITIALIZER_UNLOCKED;

/// <summary>
/// ISR handler for anemometer rotation pin. Counts 
/// anemometer rotations.
/// </summary>
/// <remarks>ARDUINO_ISR_ATTR uses fast RAM.</remarks>
void ARDUINO_ISR_ATTR onAnemRotate() {
	// Ignore any additional activations within the debounce time.
	if ((millis() - last_debounce_time_anem) > DAVIS_ANEM_DEBOUNCE_TIMEOUT_MS) {
		portENTER_CRITICAL_ISR(&muxAnem);
		_anemCount++;
		portEXIT_CRITICAL_ISR(&muxAnem);
		last_debounce_time_anem = millis();
	}
}

/// <summary>
/// ISR handler for anemometer timer. Holds rotation count 
/// every ANEM_READ_PERIOD_SEC for wind speed calculation,
/// and flags that it's time to calculate instantaneous wind
/// speed.
/// </summary>
/// <remarks>ARDUINO_ISR_ATTR uses fast RAM.</remarks>
void ARDUINO_ISR_ATTR onAnemTimer() {
	portENTER_CRITICAL_ISR(&muxAnem);
	anemRots = _anemCount;	// hold rotation count.
	_anemCount = 0;			// reset count for next timer cycle.
	_anemTimer_ticks++;		// flag to calculate wind speed.
	portEXIT_CRITICAL_ISR(&muxAnem);
}

// FAN (for radiation shield)
unsigned long millis_fan_read = 0;	// keep track of fan read time
volatile int fanHalfRots = 0;		// count fan half-rotations_v (1 count/half-rotation)

// ==========   Async Web Server   ================== //
AsyncWebServer server(80);	// Async web server instance on port 80.

// ==========   WiFi network   ================== //
WiFiTools wifi;

// ==========   u-blox NEO-6M GPS   ========================== //
// GPS module instance. 
GPSModule gps;

int oldMonth = 0;	// Month of previous day.
int oldDay = 0;		// Day of previous day.
int oldYear = 0;	// Year of previous day.

// ========   Setup LittleFS   =================  //

/// <summary>
/// Initializes and mounts LittleFS filesystem. 
/// Logs mount success or failure, records used/available space, 
/// sets the isGood_LittleFS flag on success, and optionally 
/// lists the root directory when debugging is enabled.
/// </summary>
void littleFS_initialize() {
	if (!LittleFS.begin()) {
		String msg = "ERROR: LittleFS didn't mount.";
		sd.logStatus(msg, millis());
	}
	else {
		isGood_LittleFS = true;
		String msg = "LittleFS mounted.";
		sd.logStatus(msg, millis());
	}
	// Log used and available space.
	sd.logLittleFsSpaceUsage();

	if (isDEBUG_ListLittleFS) {
		dirList(LittleFS, "/", 5);
	}
}

/// <summary>
/// Reads recent sensor readings from LittleFS and loads them back into memory.
/// </summary>
/// <param name="sensData">SensorData instance.</param>
/// <param name="lastSaveTime">Most recent data save time.</param>
/// <param name="time">Time recovery is performed.</param>
void recoverData(SensorData& sensData, time_t lastSaveTime, time_t time) {
	String tStr = GPSModule::dateTime_Str(time);
	String msg;
	if (!sensData.isDatafile()) {
		msg = "Data file is not configured for " + sensData.label();
		sd.logStatus(msg, tStr);
		return;
	}

	// 10-min lists
	Serial.println("recoverData(): 10-min lists " + sensData.label());
	if ((now() - lastSaveTime) <= DATA_RECOVER_10_MIN_AGE_LIMIT_SEC) {
		sensData.recover_data_10_min_from_file();
		sd.logStatus("Recovered 10-min data.", millis());
		sd.logStatus(sensData.dataPoints_10_min_as_String().c_str(), tStr);
	}
	else {
		msg = "No 10-min data found within configured age limit of ";
		msg += String(DATA_RECOVER_10_MIN_AGE_LIMIT_SEC / 60.) + "min";
		sd.logStatus(msg, tStr);
	}

	// 60-min lists
	Serial.println("recoverData(): 60-min lists " + sensData.label());
	if ((now() - lastSaveTime) <= DATA_RECOVER_60_MIN_AGE_LIMIT_SEC) {
		sensData.recover_data_60_min_from_file();
		sd.logStatus("Recovered 60-min data.", millis());
		sd.logStatus(sensData.dataPoints_60_min_as_String().c_str(), tStr);
	}
	else {
		msg = "No 60-min data found within configured age limit of ";
		msg += String(DATA_RECOVER_60_MIN_AGE_LIMIT_SEC / 60.) + "min";
		sd.logStatus(msg, tStr);
	}

	// day lists
	Serial.println("recoverData(): day lists " + sensData.label());
	if ((now() - lastSaveTime) <= DATA_RECOVER_DAY_AGE_LIMIT_SEC) {
		sensData.recover_data_dayMaxMin_from_file();
		sd.logStatus("Recovered dayMaxMin data.", millis());
		sd.logStatus(sensData.dataPoints_dayMaxMin_as_String().c_str(), tStr);
	}
	else {
		msg = "No dayMaxMin data found within configured age limit of ";
		msg += String(DATA_RECOVER_DAY_AGE_LIMIT_SEC / 3600.) + "hr";
		sd.logStatus(msg, tStr);
	}
}



/****************************************************************************/
/******************************      SETUP      *****************************/
/****************************************************************************/

void setup() {
	Serial.begin(115200);
	// Print status message now because SD is not yet online.
	String msg = "\n\n" + LINE_SEPARATOR_MAJOR + "\n";
	msg += String(millis() / 1000.);
	msg += "s ENTERING SETUP \nSD card not yet online.\n";
	msg += LINE_SEPARATOR_MAJOR + "\n\n";
	Serial.print(msg);

	//  SETUP: ==========  CREATE SD CARD   ========== //
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  CREATE SD CARD   ==========");
#endif
	// (Do this first - need SD card for logging.)
	isGood_SDCard = sd.create(SPI_CS_PIN, isDEBUG_BypassSDCard);

#if defined(VM_DEBUG)
	//Serial.println("Listing SD card dir to Serial:");
	//FileOps::dirList(SD, "/", 3);
#endif

	// Begin status log entries to SD card.
	sd.logStatus();	// Empty line
	sd.logStatus(LINE_SEPARATOR_MAJOR);
	sd.logStatus("SETUP continues after SD card initialization.", millis());

	// SD log file.
	FileStatus status;
	status = FileOps::fileCreateOrExists(SD, LOG_STATUS_FILEPATH_SD);
	sd.logStatus("Status of " + LOG_STATUS_FILEPATH_SD + " = " + fileStatus_toString(status), millis());
	// SD data file.
	status = FileOps::fileCreateOrExists(SD, LOG_DATA_FILEPATH_SD);
	sd.logStatus("Status of " + LOG_DATA_FILEPATH_SD + " = " + fileStatus_toString(status), millis());


	// Log the settings to the status file.
	sd.logDebugStatus(
		isDEBUG_BypassGPS,
		isDEBUG_BypassWifi,
		isDEBUG_BypassSDCard,
		isDEBUG_ListLittleFS,
		isDEBUG_BypassWebServer,
		isDEBUG_run_test_in_setup,
		isDEBUG_run_test_in_loop,
		isDEBUG_addDummyDataLists,
		isDEBUG_simulateSensorReadings,
		isDEBUG_simulateWindReadings,
		isDEBUG_AddDelayInLoop,
		LOOP_DELAY_DEBUG_ms);
	sd.logApp_Settings();

	//  SETUP: ==========  CREATE WIFI NETWORK   ========== //	
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  CREATE WIFI NETWORK   ==========");
#endif
	wifi.Initialize(sd);
	wifi.wifiSetupAndConnect(gps.dateTime_Str(), isDEBUG_BypassWifi);

	//  ==========  CREATE ASYNC WEB SERVER   ========== //	
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  CREATE ASYNC WEB SERVER   ==========");
#endif
	createServerRouteHandler();	// Define routes for server requests.
	sd.logStatus("Async web server routes defined.", millis());
	server.begin();				// Start async web server.
	sd.logStatus("Async web server beginning.", millis());

	// ==========   CREATE GPS AND SYNC TO GET TIME   ========== //
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  CREATE GPS AND SYNC TO GET TIME   ==========");
#endif
	// XXX  Need code to alter power of GPS!!!  XXX

	/* 	Format for setting s serial port:
		SerialObject.begin(baud-rate, protocol, RX pin, TX pin);
		(Wrong baud rate gives serial garbage.)
	*/
	// Connect to GPS

#define SERIAL_CONFIGURATION SERIAL_8N1		// data, parity, and stop bits
	gps.begin(GPS_BAUD_RATE, SERIAL_CONFIGURATION, RX2_PIN, TX2_PIN);
	sd.logStatus("Connecting to GPS.", millis());
	// Get time and location from GPS.
	// This code is BLOCKING until gps syncs.
	bool isGpsSuccess = false;
	if (!gps.isSynced())
	{
		isGpsSuccess = gps.syncToGPS(sd, isDEBUG_BypassGPS);
		isGood_GPS = true;
	}
	if (!isGpsSuccess)
	{
		String msg = "ERROR: GPS did not sync after ";
		msg += String(GPS_CYCLES_COUNT_MAX) + " cycles.";
		sd.logStatus(msg, millis());
	}
	// What do we now say time is?
	String msg2 = "System time assigned = " + gps.dateTime_Str();
	sd.logStatus(msg2, millis());
	sd.logStatus(msg2, gps.dateTime_Str());


	// ==========  SETUP LittleFS  ========== //
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  SETUP LittleFS   ==========");
#endif
	littleFS_initialize();

#if defined(VM_DEBUG)
	////////  TESTING   ////////
	Serial.println("SETUP: ==========  TESTING   ==========");
	if (isDEBUG_addDummyDataLists) {
		Serial.println("SETUP: ==========  ADD DUMMY DATA   ==========");
		addDummyData();
		saveLastReadTime_toFile(now());
		Serial.println();
		Serial.println("XXX  saveLastReadTime_toFile(now())  XXX");
		Serial.println();
	}
	if (isDEBUG_run_test_in_setup) {
		bool isDataInFilesys = true;
		bool isReportDailyMaxOnly = false;
		SensorData d(isDataInFilesys, isReportDailyMaxOnly);
		d.addLabels("Test sensor", "testPrefix", "units");
		d.createSensorDataFiles(false, 2);

		unsigned long int t = 0;
		float val = 40;
		float delta = 15. / 150.;
		Serial.printf("Delta = %.3f\n", delta);

		int num = 150;
		for (size_t i = 0; i < num; i++)
		{
			t += SECONDS_PER_DAY;	// Increase datetime by 1 day.
			d.addReading(DataPoint(t, val));
			Serial.printf("Add reading (%li, %.3f), countReadings = %i\n", t, val, d.countReadings());
			Serial.printf("avg_now = %.3f, avg_10_min = %.3f\n", d.avg_now(), d.avg_10_min());
			Serial.printf("Point just added = (%li, %.3f)\n", d.dataPointLastAdded().time, d.dataPointLastAdded().value);
			Serial.printf("min_10_min_dp = (%i, %.3f), max_10_min_dp = (%i, %.3f)\n",
				d.min_10_min_dp().time,
				d.min_10_min_dp().value,
				d.max_10_min_dp().time,
				d.max_10_min_dp().value);
			Serial.printf("min_today_dp = (%i, %.3f), max_today_dp = (%i, %.3f)\n",
				d.min_today_dp().time,
				d.min_today_dp().value,
				d.max_today_dp().time,
				d.max_today_dp().value);
			//d.process_data_10_min();	// XXX	will reset averages
			Serial.printf("dataPoints_10_min_as_String: %s\n", d.dataPoints_10_min_as_String().c_str());
			Serial.println(LINE_SEPARATOR);
			//delay(1000);
			val += delta;	// Change value for next time.
		}
		Serial.println(LINE_SEPARATOR_MAJOR);
		Serial.printf("avg_now() = %.3f\n", d.avg_now());
		Serial.printf("max_10_min_dp() = %.3f\n", d.max_10_min_dp().value);
		Serial.printf("min_10_min_dp() = %.3f\n", d.min_10_min_dp().value);
		Serial.printf("dataPoints_10_min_as_String: %s\n", d.dataPoints_10_min_as_String().c_str());
		Serial.println("process_data_10_min!!!");
		d.process_data_10_min();
		Serial.printf("min_10_min_dp = (%i, %.3f), max_10_min_dp = (%i, %.3f)\n",
			d.min_10_min_dp().time,
			d.min_10_min_dp().value,
			d.max_10_min_dp().time,
			d.max_10_min_dp().value);
		Serial.printf("min_today_dp = (%i, %.3f), max_today_dp = (%i, %.3f)\n",
			d.min_today_dp().time,
			d.min_today_dp().value,
			d.max_today_dp().time,
			d.max_today_dp().value);
		Serial.printf("dataPoints_10_min_as_String: %s\n", d.dataPoints_10_min_as_String().c_str());

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
#endif

	// ==========  CREATE SENSORS  ========== //
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  CREATE SENSORS   ==========");
#endif
	sensors_AddLabels();	// Add labels and units to the SensorData instances.
	sensors_begin();		// Initialize all sensors.
	sensors_createFiles();	// Create LittleFS files.

	// ==========  RECOVER DATA   ==========
	if (isRECOVER_FILESYS_DATA) {
#if defined(VM_DEBUG)
		Serial.println("SETUP: ==========  RECOVER DATA FROM FILE SYSTEM   ==========");
#endif
		// Retrieve recent saved data from LittleFS. (In case of inadvertent reboot.)

		String lastTimeStr = fileRead(LittleFS, SENSOR_LAST_SAVE_TIME_FILEPATH_FS.c_str());
		if (lastTimeStr.length() > 0) {
			msg = "Last data save time = " + GPSModule::dateTime_Str(lastTimeStr.toInt());
			sd.logStatus(msg, now());

			// RECOVER DATA FOR ALL SENSORS !!!!!!!!!!!!!!!
			recoverData(d_TempF, lastTimeStr.toInt(), now());

		}
		else {
			msg = "Did not find last data save time. No data recovery.";
			sd.logStatus(msg, now());
		}


	}
	else {
		Serial.println("SETUP: BYPASS RECOVER DATA FROM FILE SYSTEM");
	}

	// Date info to determine when new day begins.
	oldDay = day();
	oldMonth = month();
	oldYear = year();

	sd.logData(columnNames());	// Write column names to data log.
	sd.logStatus_indent("DATA COLUMNS:\t" + columnNames());	// Write column names to status log.

	/// ==========  CONFIGURE FAN PWM  ========== //

	constexpr int FAN_PWM_FREQUENCY = 25000;	// 25kHz
	constexpr int FAN_PWM_CHANNEL = 0;
	constexpr int FAN_PWM_RESOLUTION = 8;

	// VERSION FOR ESP32 2.X
	ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	// Attach the channel to the pin for PWM output.
	ledcAttachPin(FAN_PWM_PIN, FAN_PWM_CHANNEL);
	// Set fan speed using PWM.
	ledcWrite(FAN_PWM_CHANNEL, FAN_DUTY_PERCENT / 100. * 256);
	/*
	// VERSION FOR ESP32 3.X
	ledcAttach(FAN_PWM_PIN, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	ledcWrite(FAN_PWM_PIN, FAN_DUTY_PERCENT / 100. * 256);
	*/


	// ==========  SETUP: CREATE INTERRUPTS  ========== //
#if defined(VM_DEBUG)
	Serial.println("SETUP: ==========  CREATE INTERRUPTS   ==========");
#endif
	// ANEMOMETER HARDWARE INTERRUPT (every rotation).
	pinMode(WIND_SPEED_PIN, INPUT_PULLUP);
	// Specify ISR to handle WIND_SPEED_PIN.
	attachInterrupt(
		digitalPinToInterrupt(WIND_SPEED_PIN),	// attach to pin
		&onAnemRotate,							// ISR handler
		FALLING);

	// HARDWARE TIMER INTERRUPT to capture data 
	// for wind speed calc every ANEM_READ_PERIOD_SEC.
	// For ESP32 Arduino IDE 2.X:
	timerAnem = timerBegin(0, 80, true);
	// Attach ISR (interrupt service routine) to hardware timer created with timerBegin().
	timerAttachInterrupt(timerAnem, &onAnemTimer, true);
	timerAlarmWrite(timerAnem,					// hardware timer
		ANEM_READ_PERIOD_SEC * MICROSEC_PER_SECOND,	// every ANEM_READ_PERIOD_SEC.
		true);									// auto reload	
	timerAlarmEnable(timerAnem);
	/*
	For ESP32 Arduino IDE 3.X ...
	int duration_count = ANEM_READ_PERIOD_SEC * MICROSEC_PER_SECOND;	// Timer duration (microsec).
	timerAnem = timerBegin(duration_count);
	timerAttachInterrupt(timerAnem, &onAnemRotate);
	timerAlarm(timerAnem, duration_count, true, 0);		// Trigger every ANEM_READ_PERIOD_SEC.
	*/

	Serial.println("SETUP: ==========  COMPLETED CREATE TIMER INTERRUPT   ==========");

	msg = "CURRENT LOCAL TIME is " + gps.dateTime_Str();
	(IS_DAYLIGHT_TIME) ? msg = " Daylight time." : msg = " Standard time.";
	sd.logStatus(msg);
	sd.logStatus("SETUP END " + gps.dateTime_Str(), millis());

#if defined(VM_DEBUG)
	//Serial.println("Listing SD card dir to Serial:");
	//FileOps::dirList(SD, "/", 3);
#endif

	time_start_first_loop = millis();
}

/****************************************************************************/
/************************        END SETUP       ****************************/
/****************************************************************************/



// Initialize loop timers (immediately before loop starts!).
unsigned long millis_sensors_read = 0;			// Start time of reading sensors
unsigned long millis_sensors_Process_10 = 0;	// Start time of processing sensors at 10-min.
unsigned long millis_sensors_Process_60 = 0;	// Start time of processing sensors at 60-min.

//unsigned int countSensorsRead = 0;


/****************************************************************************/
/***************************       LOOP      ********************************/
/****************************************************************************/

void loop() {
	time_start_current_loop = millis();	// To monitor loop execution time.

	//bool isProcessWind = false;

	//// Check for timer interrupts that were 
	//// not handled during code delays.
	//catchUnhandledBaseTimerInterrupts();

	/************************************************
		Read sensors and process data at intervals
		determined from timer interrupt.
	*************************************************/

	/*
	 XXX	After every 4 sec Base Period, get anemometer counts as well as
	 all sensor values. Use values to accumulate 10-min averages.
	 (This is ESSENTIAL for anemometer wind speed, which counts
	 anemRots for every Base Period. It is CONVENIENT for timing
	 reading of other sensors.

	 At 10-min intervals, save 10-min averages and start
	   accumulating next 10-min avg.

	   At 60-min intervals, save 60-min averages and
	   start accumulating next 60-min avg.
	*/

	// ====================================================
	//		ANEM_READ_PERIOD_SEC (4 sec)
	// ====================================================	
		
	if (_anemTimer_ticks > 0) {
		portENTER_CRITICAL(&muxAnem);
		_anemTimer_ticks--;	// Anemometer timer interrupt handled.
		portEXIT_CRITICAL(&muxAnem);
					
		//// anemRots holds _anemCount from latest timer ISR.
		//processWind(anemRots);		// Wind speed, gust, direction.


		processWind(18);		// XXX	XXX	XXX	XXX !!!


		// Check for unhandled timer.
		if (_anemTimer_ticks > 0) {
			// If still > 0, we missed a timer tick
			String msg = "WARNING: Missed _anemTimer_ticks = " + _anemTimer_ticks;
			sd.logStatus(msg, gps.dateTime_Str());
		}
	}
	
	//// ====================================================
	////		READ SENSORS
	//// ====================================================
	if ((millis() - millis_sensors_read) >= SENSOR_READ_PERIOD_SEC * MILLISEC_PER_SECOND) {
		// Read data for sensors one at a time.
		readSensor_by_index(sensor_idx);
		sensor_idx++;
		if (sensor_idx > COUNT_SENSORS_TO_READ - 1) {
			sensor_idx = 0;					// restart sensor cycle.
			millis_sensors_read = millis();	// reset timer
		};
	}

	//// ====================================================
	////		READ FAN
	//// ====================================================
	if ((millis() - millis_fan_read) >= FAN_READ_PERIOD_SEC * MILLISEC_PER_SECOND) {

		//Serial.printf("\t%.3fs READ FAN\n", millis() / 1000.);

		// Get fan speed
		d_fanRPM.addReading(DataPoint(now(), fanRPM(fanHalfRots, FAN_READ_PERIOD_SEC)));
		fanHalfRots = 0;			// Reset fan rotations_v.
		millis_fan_read = millis();	// reset timer
	}

	//// ====================================================
	////		10-MIN INTERVAL
	//// ====================================================

	if ((millis() - millis_sensors_Process_10) >= 10 * MILLISECONDS_PER_MINUTE) {


		//if ((millis() - millis_sensors_Process_10) >= 3 * MILLISECONDS_PER_MINUTE) {	// XXX


			// Get 10-min avgs.

		Serial.printf("========> %.3fs 10-MIN INTERVAL\n", millis() / 1000.);

		// Process 10-min data for sensors one at a time.
		processSensor_10_min_by_index(sensor_idx_10);
		sensor_idx_10++;
		if (sensor_idx_10 > COUNT_SENSORS_TO_PROCESS - 1) {	// all sensors read
			sensor_idx_10 = 0;			// Restart sensor cycle.
			// Save last 10-min reading ttime to LittleFS. Used 
			// to check whether to recover data at reboot.
			saveLastReadTime_toFile(now());
			millis_sensors_Process_10 = millis();	// restart timer
			sd.logData(sensorsDataString_10_min());	// Save readings to SD card.
			sd.logStatus("Logged 10-min avgs.", gps.dateTime_Str());
		};
	}

	//// ====================================================
	////		60-MIN INTERVAL
	//// ====================================================   

	if ((millis() - millis_sensors_Process_60) >= 60 * MILLISECONDS_PER_MINUTE) {

		//if ((millis() - millis_sensors_Process_60) >= 10 * MILLISECONDS_PER_MINUTE) {	// XXX


			// Get 60-min avgs.

			// Process 60-min data for sensors one at a time.
		processSensor_60_min_by_index(sensor_idx_60);
		sensor_idx_60++;
		if (sensor_idx_60 > COUNT_SENSORS_TO_PROCESS - 1) {
			sensor_idx_60 = 0;			// Restart sensor cycle.
			sd.logStatus("Calculate 60-min avgs.", gps.dateTime_Str());
			millis_sensors_Process_60 = millis();	// restart timer
		};
	}

	//// ====================================================
	////		DAY ROLLOVER
	//// ====================================================   
	if (day() > oldDay || month() > oldMonth || year() > oldYear) {
		// NEW DAY. 
		// Save minima and maxima for previous day.

		// Process day rollover data for sensors one at a time.
		processSensor_day_by_index(sensor_idx_day);
		sensor_idx_day++;
		if (sensor_idx_day > COUNT_SENSORS_TO_PROCESS - 1) {
			sensor_idx_day = 0;			// Restart sensor cycle.
			// Log and reset after all sensors processed.
			oldDay = day();
			oldMonth = month();
			oldYear = year();
			sd.logStatus("New day rollover.", gps.dateTime_Str());
		};

	}

	///// ==========  TEST FOR LOST WIFI CONNECTION  ========== //
	///*
	//If WiFi is lost, we're screwed because the time
	//to reconnect may throw of the sensor read timings.
	//Just bite the bullet and take the time to reconnect,
	//then recover.

	//If WiFi was lost, the time to reconnect will cause
	//the timer interrupt counts to delta beyond where
	//they should have been handled.
	//*/
	//if (!isDEBUG_BypassWifi) {
	//	if (WiFi.status() != WL_CONNECTED) {
	//		wifi.checkWifiConnection(gps.dateTime_Str());

	//		// XXX Moved here from WiFiTools::checkWifiConnection()
	//		resetInterruptCounts();
	//		String msg = "Read cycle skipped after WiFi was lost.";
	//		sd.logStatus(msg, gps.dateTime_Str());
	//	}
	//}

#if defined(VM_DEBUG)
	// Add delay for DEBUG.
	if (isDEBUG_AddDelayInLoop) {
		vTaskDelay(LOOP_DELAY_DEBUG_ms / portTICK_PERIOD_MS);
	}
#endif

	// Watch for excessive processing time in loop.
	if (millis() - time_start_current_loop > LOOP_TIME_WARN_THRESHOLD_MS) {
		String msg = "Loop " + String(millis() - time_start_current_loop) + "ms";
		sd.logStatus(msg, gps.dateTime_Str());
		//Serial.printf("long loop %lims\n", millis() - time_start_current_loop);
	}
}
/******************************        END LOOP        **********************************/
/****************************************************************************************/
/****************************************************************************************/
