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

	constexpr unsigned int	BASE_PERIOD_SEC = 4; // Period to sample anemometer rots. Current default 4 sec.

	constexpr unsigned int	DATA_RECOVERY_10_MIN_AGE_LIMIT = 30 * SECONDS_PER_MINUTE;	// Recover 10-min data no older than this.
	constexpr unsigned int	DATA_RECOVERY_60_MIN_AGE_LIMIT = 3 * SECONDS_PER_HOUR;		// Recover 60-min data no older than this.
	constexpr unsigned int	DATA_RECOVERY_DAY_AGE_LIMIT = 3 * SECONDS_PER_DAY;			// Recover day data no older than this.

	constexpr unsigned long GPS_BAUD_RATE = 9600;		// Baud rate for NEO-6M 9600 GPS.
	constexpr unsigned int	GPS_SATELLITES_REQUIRED = 5;// Minimum number of GPS satellites before syncing.
	constexpr unsigned int	GPS_CYCLES_FOR_SYNC = 4;	// Minimum GPS cycles before syncing.
	constexpr unsigned int	GPS_CYCLE_DELAY_SEC = 2;	// Delay before getting another GPS fix, sec.
	constexpr unsigned int	GPS_CYCLES_COUNT_MAX = 70;	// Max number of GPS cycles before quitting.
	constexpr float			GPS_MAX_ALLOWED_HDOP = 2;	// Minimum HDOP precision for syncing.

	constexpr unsigned int	GPS_DUMMY_HOUR = 23;		// Hour for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_MIN = 05;			// Minute for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_SEC = 0;			// Second for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_DAY = 1;			// Day for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_MONTH = 1;		// Month for dummy GPS time.
	constexpr unsigned int	GPS_DUMMY_YEAR = 2099;		// Year for dummy GPS time.
	constexpr int			UTC_OFFSET_HOURS = -8;		// Time zone offset from UTC.
	constexpr bool			IS_DAYLIGHT_TIME = true;	// True if adjusted to Taylight Time.

	constexpr float			WIFI_CONNECT_TIMEOUT_SEC = 10;		// Timeout for connecting to WiFi SSID, sec.
	constexpr float			WIFI_CONNECT_TIMEOUT_LOST_SEC = 60;	// Timeout for connecting to WiFi SSID, sec.

	constexpr unsigned int	FAN_DUTY_PERCENT = 30;		// PWM duty cycle for fan speed.

	constexpr int	COUNT_SENSORS_TO_READ = 9;			// No. sensors to read in readSensors(index).
	const String	SENSOR_DATA_DIR_PATH = "/Sensor data";	// Absolute path to sensor data files directory.
	const String	SENSOR_DATA_TIME_FILE_PATH = "/Sensor data/last_time.txt";	// Absolute path to sensor read time file.
	constexpr int	DATA_FILE_BUFFER_SIZE = 1024;		// Size of buffer when reading a readings data file from file sys.
	const String	LOGFILE_PATH_DATA = "/data.txt";	// Data file path on SD card.
	const String	LOGFILE_PATH_STATUS = "/log.txt";	// Log file path on SD card.

	const String	LINE_SEPARATOR_LOG_BEGINS = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
	const String	LINE_SEPARATOR_MAJOR = "=====================================================";
	const String	LINE_SEPARATOR = "-------------------------------------------------";

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
	constexpr float			VANE_OFFSET = 0;					// Degrees that wind direction reading exceeds north.
	constexpr float			WIND_DIR_SPEED_THRESHOLD = 1;		// WindSpeed below which wind direction is not reported.
	constexpr float			WIND_SPEED_OUTLIER_DELTA = 10;		// Factor applied to moving avg for outlier comparison
	constexpr unsigned int	WIND_SPEEDS_IN_MOVING_AVG = 5;		// no. of speeds for moving avg.

	constexpr unsigned int	LOOP_TIME_WARN_THRESHOLD_MS = 120;	// threshold for reporting long loop cycle.

	/// <summary>
	/// Enumerate lists of sensor data of different periods.
	/// </summary>
	enum dataPeriod {
		PERIOD_10_MIN,	// 10-minute period
		PERIOD_60_MIN,	// 60-minute period
		PERIOD_DAY		// 24-hour period
	};

	/// <summary>
	/// Max size of data lists.
	/// </summary>
	enum speedListSize {
		SIZE_10_MIN_LIST = 24,	// Max. no. of 10-min readings in that list. (At least 6 needed for 60-min avg)!
		SIZE_60_MIN_LIST = 24,	// Max. no. of 60-min readings in that list. (At least 12 needed for 12-hr avg)!
		SIZE_DAY_LIST = 30		// Max. no. of daily readings in list. (Hold data for 30 days.)
	};

	/// <summary>
	/// The number of base periods in a report interval. (Assume 4s BASE_PERIOD_SEC)
	/// </summary>
	enum BasePeriodsInInterval {
		BASE_PERIODS_IN_10_MIN = 10 * SECONDS_PER_MINUTE / BASE_PERIOD_SEC,	// No. of Base Periods in 10 minutes.
		BASE_PERIODS_IN_60_MIN = 60 * SECONDS_PER_MINUTE / BASE_PERIOD_SEC,	// No. of Base Periods in 60 minutes.
		BASE_PERIODS_IN_24_HR = 24 * SECONDS_PER_HOUR / BASE_PERIOD_SEC		// No. of Base Periods in 24 hours.
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