// SensorData.h

#ifndef _SENSORDATA_h
#define _SENSORDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <list>
using std::list;
#include "dataPoint.h"
#include "DataPoint_Lists.h"
#include "App_settings.h"
using namespace DataPoint_Lists;
using namespace App_Settings;
#include "FileOperations.h"
using namespace FileOps;

/// <summary>
/// Object that reads and processes data for a sensor.
/// </summary>
class SensorData {

protected:		// Protected items are accessible by inherited classes.

	/// <summary>
	/// Returns a sensor data text-file path.
	/// </summary>
	/// <param name="fileSuffix">Suffix to append to file name.</param>
	String _sensorFilepath(String fileSuffix);

	String _label, _filenamePrefix;	// Identifying info.
	String _units, _units_html;		// Units used.

	DataPoint _dataPointLastAdded;	// Data point (time, value) of most recent reading.

	float _sumReadings = 0;			// Accumulating sum of readings.
	unsigned int _countReadings = 0;// Number of readings in average.

	float _avg_10_min = 0;			// Average over 10 min.
	float _avg_60_min = 0;			// Average over 60 min.

	static constexpr float VAL_LIMIT = 999999;	// No reading absolute value will ever be greater.

	/// <summary>
	/// Updates saved min and max values for 10-min period and today.
	/// </summary>
	/// <param name="dp">Data point with value to evaluate.</param>
	void _updateMinMax(DataPoint dp);

	// Initialize at impossible extremes.

	DataPoint _min_today_dp = DataPoint(0, VAL_LIMIT);		// Today's minimum.
	DataPoint _max_today_dp = DataPoint(0, -VAL_LIMIT);	// Today's maximum.

	/// <summary>
	/// Minimum sensor reading in the current 10-min period.
	/// </summary>
	DataPoint _min_10_min_dp = DataPoint(0, VAL_LIMIT);	// Initialize at high extreme.

	/// <summary>
	/// Maximum sensor reading in the current 10-min period.
	/// </summary>
	DataPoint _max_10_min_dp = DataPoint(0, -VAL_LIMIT);	// Initialize at low extreme.

	/// <summary>
	/// Clears running average and min, max for 10-min period.
	/// </summary>
	void _clear_10_min();

	bool _isDatafile = true;			// Set true to save periodic data in LittleFS file system.
	bool _isReportDayMaxOnly = false;	// Set true to save maxima but not minima on LittleFS file system.
	bool _isConvertZeroToEmpty = true;	//
	unsigned int _decimalPlaces = 0;	//

	list<DataPoint> _dataPoints_10_min;		// List of Data_Points at 10-min intervals.
	list<DataPoint> _dataPoints_60_min;		// List of Data_Points at 60-min intervals.
	list<DataPoint> _dataPoints_dayMin;		// List of daily minima.
	list<DataPoint> _dataPoints_dayMax;		// List of daily maxima.

public:

	// Constructor.

	/// <summary>
	/// Creates SensorData instance that exposes 
	/// methods to read and process sensor data.
	/// </summary>
	/// <param name="isDatafile">
	/// Set true to store data in LittleFS file system.</param>
	/// <param name="isReportDailyMaxOnly">
	/// Set true to save maxima but not minima LittleFS file system.</param>
	/// <param name="isUseMovingAvg">
	/// Set true to smooth data.</param>
	/// <param name="numSmoothPoints">
	/// Number of points in moving avg.</param>
	/// <param name="outlierDelta">
	/// Range applied to moving avg for outlier rejection.</param>
	SensorData(bool isDataInFileSys = true,	bool isReportDailyMaxOnly = false);

	/// <summary>
	/// Creates files that hold sensor data points at various 
	/// intervals.
	/// </summary>
	/// <param name="isConvertZeroToEmpty">
	/// Set to true to convert zero to empty in output strings. (Default is true.)
	/// </param>
	/// <param name="decimalPlaces">Decimal places in output strings. (Default is 0.)
	/// </param>
	void createSensorDataFiles(bool isConvertZeroToEmpty = true, unsigned int decimalPlaces = 0);

	/// <summary>
	/// Adds (time, value) DataPoint, accumulates average, 
	/// and processes min, max.
	/// </summary>
	/// <param name="dp">(time, value) DataPoint.</param>
	void addReading(DataPoint dp);

	/// <summary>
	/// Calculates 10-min avg and saves data to 10-min 
	/// list. Writes this list to file system. WARNING: This 
	/// will RESET ACCUMULATED SUMS for 10-min avg and reset 
	/// 10-min min and max.
	/// </summary>
	void process_data_10_min();

	/// <summary>
	/// Calculates 60-min avg and saves data to 60-min 
	/// list. Writes this list to file system.
	/// </summary>
	void process_data_60_min();

	/// <summary>
	/// Adds day maximum to dayMax list and day minimum 
	/// to dayMin list. Writes a combination of these 
	/// lists to the file system.
	/// </summary>
	void process_data_day();

	/// <summary>
	/// Data point (time, value) of latest sensor reading.
	/// </summary>
	/// <returns>Data point with (time, value) of latest 
	/// sensor reading.
	/// </returns>
	DataPoint dataPointLastAdded();

	/// <summary>
	/// The most-recently added data value.
	/// </summary>
	/// <returns>Most recent reading value.</returns>
	float valueLastAdded();

	/// <summary>
	/// The accumulated avg now (cleared every 10 minutes). When 
	/// data smoothing is enabled, outlier values are excluded.
	/// </summary>
	/// <returns>Average now.</returns>
	float avg_now();

	/*/// <summary>
	/// Returns moving average of last several reading values.
	/// </summary>
	/// <returns>Moving average.</returns>
	float avg_moving();*/

	/// <summary>
	/// The last average saved to the 10-min list.
	/// </summary>
	/// <returns>The last average saved to the 10-min list.
	/// </returns>
	float avg_10_min();

	/// <summary>
	/// The last average saved to the 60-min list.
	/// </summary>
	/// <returns>The last average saved to the 60-min list.
	/// </returns>
	float avg_60_min();

	/// <summary>
	/// The number of readings since the last reset.
	/// </summary>
	/// <returns>The number of readings since the last reset.
	/// </returns>
	int countReadings();

	/// <summary>
	/// Clears saved minimum and maximum for today.
	/// </summary>
	void clearMinMax_day();

	/// <summary>
	/// Returns a (time, value) data point containing the 
	/// minimum sensor reading in the current 10-min 
	/// period. Updates at every reading and resets when new 
	/// 10-min period starts.
	/// </summary>
	/// <returns>Data point with (time, value) minimum 
	/// reading in current 10-min period.</returns>
	DataPoint min_10_min_dp();	// Minimum over 10-min period.

	/// <summary>
	/// Returns a (time, value) data point containing the 
	/// maximum sensor reading in the current 10-min 
	/// period. Updates at every reading and resets when new 
	/// 10-min period starts.
	/// </summary>
	/// <returns>Data point with (time, value) maximum 
	/// reading in current 10-min period.</returns>
	DataPoint max_10_min_dp();// Maximum over 10-min period.

	/// <summary>
	/// Returns a (time, value) data point containing the 
	/// minimum sensor reading today. Updates at 
	/// every reading and resets when day rolls over.
	/// </summary>
	/// <returns>Data point with (time, value) of today's 
	/// minimum reading.</returns>
	DataPoint min_today_dp();

	/// <summary>
	/// Returns a (time, value) data point containing the 
	/// maximum sensor reading today. Updates at 
	/// every reading and resets when day rolls over.
	/// </summary>
	/// <returns>Data point with (time, value) of today's 
	/// maximum reading.</returns>
	DataPoint max_today_dp();

	/// <summary>
	/// List of (time, value) dataPoints at 10-min intervals.
	/// </summary>
	/// <returns>List of (time, value) dataPoints.</returns>
	list<DataPoint> dataPoints_10_min();

	/// <summary>
	/// List of (time, value) dataPoints at 60-min intervals.
	/// </summary>
	/// <returns>List of (time, value) dataPoints.</returns>
	list<DataPoint> dataPoints_60_min();

	/// <summary>
	/// List of (time, value) dataPoints of daily minima.
	/// </summary>
	/// <returns>List of (time, value) dataPoints.</returns>
	list<DataPoint> dataPoints_day_minima();

	/// <summary>
	/// List of (time, value) dataPoints of daily maxima.
	/// </summary>
	/// <returns>List of (time, value) dataPoints.</returns>
	list<DataPoint> dataPoints_day_maxima();

	/// <summary>
	/// Adds label information to the data.
	/// </summary>
	/// <param name="label">Label for the data.</param>
	/// <param name="filenamePrefix">Text that begins data file name.
	/// </param>
	/// <param name="units">Data units.</param>
	void addLabels(String label,
		String filenamePrefix,
		String units);

	/// <summary>
	/// Adds label information to the data.
	/// </summary>
	/// <param name="label">Label for the data.</param>
	/// <param name="filenamePrefix">Text that begins data file name.
	/// </param>
	/// <param name="units">Data units.</param>
	/// <param name="units_html">
	/// Data units with html encoding, such as "&deg;F".
	/// </param>
	void addLabels(String label,
		String filenamePrefix,
		String units,
		String units_html);

	/// <summary>
	/// Returns display label for the data.
	/// </summary>
	/// <returns>Display label for the data.</returns>
	String label();

	/// <summary>
	/// Returns string for constructing data file name.
	/// </summary>
	/// <returns>String for constructing data file name.</returns>
	String filenamePrefix();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	String units();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	String units_html();

	/// <summary>
	/// Returns true if sensor is configured to report 
	/// only daily maximum and not minimum.
	/// </summary>
	/// <returns></returns>
	bool isReportDayMaxOnly();

	/******     DATA FROM MEMORY     ******/

	/// <summary>
	/// Returns list of 10-min dataPoints as delimited string.
	/// </summary>
	/// <returns>List of 10-min dataPoints as delimited string.</returns>
	String dataPoints_10_min_as_String();

	/// <summary>
	/// Returns list of 60-min dataPoints as delimited string.
	/// </summary>
	/// <returns>List of 60-min dataPoints as delimited string.</returns>
	String dataPoints_60_min_as_String();

	/// <summary>
	/// Returns list of 60-min dataPoints as delimited string.
	/// </summary>
	/// <returns>Delimited string of two (time, value) lists, separated by "|".</returns>
	String dataPoints_dayMaxMin_as_String();

	/// <summary>
	/// Returns list of daily maxima dataPoints as delimited string.
	/// </summary>
	String dataPoints_dayMax_as_String();

	/// <summary>
	/// Returns list of daily minima dataPoints as delimited string.
	/// </summary>
	String dataPoints_dayMin_as_String();


	/******     DATA FROM FILE SYSTEM     ******/

	/// <summary>
	/// Returns true if data has been saved to LittleFS.
	/// </summary>
	/// <returns>True if data has been saved to LittleFS.</returns>
	bool isDatafile();

	/// <summary>
	/// Retrieves data points from stored file uses 
	/// them to initialize 10-min list. Used to retrieve 
	/// any data lost at reboot.
	/// </summary>
	/// <remarks>
	/// LIMIT SIZE OF LIST?! Shouldn't be necessary if 
	/// saved data list never exceeds max size!
	/// </remarks> 
	void recover_data_10_min_from_file();

	/// <summary>
	/// Retrieves data points from stored file uses 
	/// them to initialize 60-min list. Used to retrieve 
	/// any data lost at reboot.
	/// </summary>
	void recover_data_60_min_from_file();

	/// <summary>
	/// Retrieves data points from stored file uses 
	/// them to initialize day list. Used to retrieve 
	/// any data lost at reboot.
	/// </summary>
	void recover_data_dayMaxMin_from_file();

	/******     DUMMY DATA     ******/

	void addDummy_data_10_min(float valueStart,
		float increment,
		int numElements,
		unsigned long timeStart);

	void addDummy_data_60_min(float valueStart,
		float increment,
		int numElements,
		unsigned long timeStart);

	/// <summary>
	/// Adds the specified number of elements of dummy data to the 
	/// daily maxima list, incrementing the value each time.
	/// </summary>
	/// <param name="valueStart">Initial value.</param>
	/// <param name="increment">Amount to increment the value each time.</param>
	/// <param name="numElements">Number of elements to add.</param>
	/// <param name="_timeStartLoop">Time assigned to first data point.</param>
	void addDummy_data_dayMax(
		float valueStart,
		float increment,
		int numElements,
		unsigned long timeStart);

	/// <summary>
	/// Adds the specified number of elements of dummy data to the 
	/// daily minima list, incrementing the value each time.
	/// </summary>
	/// <param name="valueStart">Initial value.</param>
	/// <param name="increment">Amount to increment the value each time.</param>
	/// <param name="numElements">Number of elements to add.</param>
	/// <param name="_timeStartLoop">Time assigned to first data point.</param>
	void addDummy_data_dayMin(
		float valueStart,
		float increment,
		int numElements,
		unsigned long timeStart);
};

#endif