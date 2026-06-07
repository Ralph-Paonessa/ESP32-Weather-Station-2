// App_Settings.h

//#pragma once
#ifndef _APP_SETTINGS_h
#define _APP_SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <WString.h>

/// <summary>
/// Exposes parameters for weather station.
/// </summary>
namespace App_Settings {

	/*****************      OPTION FLAGS      ******************/
	constexpr bool isRECOVER_FILESYS_DATA = true;			// Recover data from file system.

	constexpr unsigned int	SECONDS_PER_MINUTE = 60;
	constexpr unsigned int	MINUTES_PER_HOUR = 60;
	constexpr unsigned int	MILLISEC_PER_SECOND = 1000;
	constexpr unsigned long	MICROSEC_PER_SECOND = 1000000;
	constexpr unsigned int	HOURS_PER_DAY = 24;
	constexpr unsigned int	SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;				// 3,600 s
	constexpr unsigned long SECONDS_PER_DAY = SECONDS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY;// 86,400 s
	constexpr unsigned long MILLISECONDS_PER_HOUR = MILLISEC_PER_SECOND * SECONDS_PER_HOUR;			// 3,600,000 ms
	constexpr unsigned long MILLISECONDS_PER_MINUTE = MILLISEC_PER_SECOND * SECONDS_PER_MINUTE;		// 60,000 ms
	constexpr float			DEGREES_PER_RADIAN = 57.2957795130823;	// Degrees in 1 radian.

	constexpr unsigned int	ANEM_READ_PERIOD_SEC = 4; // Period to sample anemometer rots. Current default 4 sec.
	constexpr unsigned int	FAN_READ_PERIOD_SEC = 100; // Period to sample radiation shield fan half-rots. Current default 60 sec.
	constexpr unsigned int	SENSOR_READ_PERIOD_SEC = 60; // Period to sample radiation shield fan half-rots. Current default 60 sec.

	constexpr unsigned int	DATA_RECOVER_10_MIN_AGE_LIMIT_SEC = 30 * SECONDS_PER_MINUTE;	// Recover 10-min data no older than 30 min.
	constexpr unsigned int	DATA_RECOVER_60_MIN_AGE_LIMIT_SEC = 3 * SECONDS_PER_HOUR;		// Recover 60-min data no older than 3 hr.
	constexpr unsigned int	DATA_RECOVER_DAY_AGE_LIMIT_SEC = 3 * SECONDS_PER_DAY;			// Recover day data no older than 3 days.

	constexpr unsigned long GPS_BAUD_RATE = 9600;		// Baud rate for NEO-6M 9600 GPS.
	constexpr unsigned int	GPS_SATELLITES_REQUIRED = 5;// Minimum number of GPS satellites before syncing.
	constexpr unsigned int	GPS_CYCLES_FOR_SYNC = 4;	// Minimum GPS cycles before syncing.
	constexpr unsigned int	GPS_CYCLE_DELAY_SEC = 2;	// Delay before getting another GPS fix, sec.
	constexpr unsigned int	GPS_CYCLES_COUNT_MAX = 70;	// Max number of GPS cycles before quitting.
	constexpr float			GPS_MAX_ALLOWED_HDOP = 2;	// Minimum HDOP precision for syncing.

	constexpr unsigned int	GPS_DUMMY_ALTITUDE = 700;	// Dummy altitude for GPS. (used Ridgecrest)
	constexpr unsigned int	GPS_DUMMY_HOUR = 12;		// Hour for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_MIN = 0;			// Minute for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_SEC = 0;			// Second for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_DAY = 1;			// Day for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_MONTH = 1;		// Month for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_YEAR = 2020;		// Year for dummy GPS time.
	constexpr int			UTC_OFFSET_HOURS = -8;		// Time zone offset from UTC.
	constexpr bool			IS_DAYLIGHT_TIME = true;	// True if adjusted to Taylight Time.

	constexpr float			WIFI_CONNECT_TIMEOUT_SEC = 10;		// Timeout for connecting to WiFi SSID, sec.
	constexpr float			WIFI_CONNECT_TIMEOUT_LOST_SEC = 60;	// Timeout for connecting to WiFi SSID, sec.

	constexpr unsigned int	FAN_DUTY_PERCENT = 30;		// PWM duty cycle for fan speed.

	constexpr int	COUNT_SENSORS_TO_READ = 10;			// No. sensors to read in readSensor_by_index(index).
	constexpr int	COUNT_SENSORS_TO_PROCESS = 10;		// No. sensors to process at longer intervals.
	const String	SENSOR_DATA_DIR_PATH = "/Sensor data";	// Absolute path to sensor data files directory.
	const String	SENSOR_DATA_TIME_FILE_PATH = "/Sensor data/last_time.txt";	// Absolute path to sensor read time file.
	constexpr int	DATA_FILE_BUFFER_SIZE = 1024;		// Size of buffer when reading a readings data file from file sys.
	const String	LOGFILE_PATH_DATA = "/data.txt";	// Data file path on SD card.
	const String	LOGFILE_PATH_STATUS = "/log.txt";	// Log file path on SD card.

	const String	LINE_SEPARATOR_LOG_BEGINS = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
	const String	LINE_SEPARATOR_MAJOR = "==================================";
	const String	LINE_SEPARATOR = "------------------------------";

	/*
	ESTIMATE of max. achievable insolation, for
	normalizing insolation percent.
	Value of 2500 for early June, 2022, yielded max. insolation 51%.
	Accidentally used 100mV in August, 2022!!
	Reset to 1500 in Sept., which would have given 85% max.
	in June, close to the summer solstice.
	*/

	constexpr float			INSOL_REFERENCE_MAX = 1500;			// Estimated maximum insolation, mV
	constexpr float			DAVIS_SPEED_CAL_FACTOR = 2.25;		// WindSpeed calibration factor for Davis anemometer.
	constexpr unsigned int	DAVIS_ANEM_DEBOUNCE_TIMEOUT_MS = 15;// Debounce timeout (millisec)
	constexpr float			VANE_OFFSET = 0;					// Degrees that wind direction reading exceeds north.
	constexpr float			WIND_DIR_SPEED_THRESHOLD = 1;		// WindSpeed below which wind direction is not reported.
	constexpr float			WIND_SPEED_OUTLIER_DELTA = 10;		// Factor applied to moving avg for outlier comparison
	constexpr unsigned int	WIND_SPEEDS_IN_MOVING_AVG = 5;		// no. of speeds for moving avg.

	constexpr unsigned int	LOOP_TIME_WARN_THRESHOLD_MS = 120;	// threshold for reporting long loop cycle.
		
	/// <summary>
	/// Max size of data lists in memory. Affects 
	/// </summary>
	enum speedListSize {
		SIZE_10_MIN_LIST = 24,	// Max num 10-min readings in 10-min list. 4 hrs. (At least 6 needed for 60-min avg)!
		SIZE_60_MIN_LIST = 24,	// Max num 60-min readings in 60-min list. 24 hrs. (At least 12 needed for 12-hr avg)!
		SIZE_DAY_LIST = 28		// Max num daily readings in day list. 28 days. (Hold data for 4 weeks.)
	};

	/// <summary>
	/// Enumerates data charts requested from the web server.
	/// </summary>
	enum chartRequested {
		CHART_NONE,
		CHART_WIND_SPEED,
		CHART_WIND_GUST,
		CHART_WIND_DIRECTION,
		CHART_TEMPERATURE_F,
		CHART_PRESSURE_SEA_LEVEL,
		CHART_RELATIVE_HUMIDITY,
		CHART_UV_INDEX,
		CHART_INSOLATION,
		CHART_IR_SKY
	};
}

#endif