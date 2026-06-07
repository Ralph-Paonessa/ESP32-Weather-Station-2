// 
// 
// 

#include "SensorData.h"

/*****************************************************************
	CONSTRUCTOR AND INITIALIZATION
******************************************************************/

/// <summary>
/// Creates SensorData instance that exposes 
/// methods to read and process sensor data.
/// </summary>
/// <param name="isDatafile">
/// Set true to store data in LittleFS file system.</param>
/// <param name="isReportDailyMaxOnly">
/// Set true to save maxima but not minima LittleFS file system.</param>
SensorData::SensorData(bool isDataInFileSys, bool isReportDailyMaxOnly) {
	_isDatafile = isDataInFileSys;
	_isReportDayMaxOnly = isReportDailyMaxOnly;
}

/// <summary>
/// Adds label information to the data.
/// </summary>
/// <param name="label">Label for the data.</param>
/// <param name="filenamePrefix">Text that begins data file name.
/// </param>
/// <param name="units">Data units.</param>
void SensorData::addLabels(
	String label,
	String filenamePrefix,
	String units) {
	_label = label;
	_filenamePrefix = filenamePrefix;
	_units = units;
}

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
void SensorData::addLabels(
	String label,
	String labelFile,
	String units,
	String units_html)
{
	addLabels(label, labelFile, units);
	_units_html = units_html;
}

/*****************************************************************
	DATA READINGS AND MANIPULATION
******************************************************************/

/// <summary>
/// Adds (time, value) DataPoint, accumulates average, 
/// and updates min and max.
/// </summary>
/// <param name="dp">(time, value) DataPoint.</param>
void SensorData::addReading(DataPoint dp) {
	// Hold most recent
	_dataPointLastAdded = dp;
	// Accumulate avg.
	_countReadings++;
	_sumReadings += dp.value;
	// Check for 10-min and daily min max.
	_updateMinMax(dp);

	/*

	XXX   XXX   ELIMINATED ALL MOVING AVG AND OUTLIER CODE!   XXX   XXX

	Want outlier detection so that a large wind gust won't "pollute"
	the moving avg wind speed. Outlier will still be reported as the last
	value read and it will still be compared by windGust to the moving
	avg to decide if it's a wind gust.
	*/

	/*
	* XXX
	*
	USING A FACTOR TO DEFINE THE OUTLIER RANGE *SUCKS*
	FOR READINGS NEAR ZERO!!!
	*/
	/*
	DANGER:
	On the first pass, _avg_moving = 0, and so the high and low outlier
	comparisons will all be zero! That means that any reading value will
	be ouside the range [0, 0] and will be declared an outlier, and won't
	be added to the moving avg. Thus, the moving avg will always be zero,
	and all values will be outliers!

	Have to do something to break out of this cycle when starting.

	Can skip outlier check the first time through.

		- This will fail if the first value happens to be 0!
	*/
	/*
	NOTE: If this is the first cycle, there's not yet
	a value assigned to _avg_moving nor any data points
	in _avg_moving_List.

		_avg_moving = 0 (as initialized).

	When _avg_moving is zero, the multiples used for the
	outlier range will be [0, 0]. Therefore, any nonzero
	value will be outside this range and marked as an outlier!

	So, NO VALUE will be saved!!!
	*/
	//// First value begins moving avg. (First value CAN'T BE AN OUTLIER!!)
	//if (!_isMovingAvgStarted) {
	//	_avg_moving = dp.value;
	//	_isMovingAvgStarted = true;
	//}

	//if (!_isUseSmoothing) {
		// No smoothing. No moving avg.
		/*_countReadings++;
		_sumReadings += dp.value;*/
		//}
		//else
		//{
		//	// Apply SMOOTHING and OUTLIER REJECTION.
		//	// First value begins moving avg. (First value CAN'T BE AN OUTLIER!!)
		//	if (!_isMovingAvgStarted) {
		//		_avg_moving = dp.value;
		//		_isMovingAvgStarted = true;
		//	}
		//	if (!isOutlier(dp))
		//	{
		//		// Not an outlier, so include in 10-min avg.
		//		_countReadings++;
		//		_sumReadings += dp.value;
		//		// Not an outlier, so include in moving avg.
		//		addDataPoint_to_List(_avg_moving_List, dp.value, _avgMoving_Num);
		//		_avg_moving = listAverage(_avg_moving_List, _avgMoving_Num);
		//		_isMovingAvgStarted = true;
		//	}
		//	else {
		//		Serial.printf("OUTLIER DETECTED for (%li, %f)\n", dp.time, dp.value);
		//	}
		//}
		//_updateMinMax(dp);
}

/// <summary>
/// Updates saved min and max values for 
/// current 10-min period and all of today.
/// </summary>
/// <param name="dp">Data point with value to evaluate.</param>
void SensorData::_updateMinMax(DataPoint dp) {
	// Update min and max so far for this 10-min period.
	_min_10_min_dp = (dp.value < _min_10_min_dp.value) ? dp : _min_10_min_dp;
	_max_10_min_dp = (dp.value > _max_10_min_dp.value) ? dp : _max_10_min_dp;

	// Update min and max so far for all of today.
	_min_today_dp = (dp.value < _min_today_dp.value) ? dp : _min_today_dp;
	_max_today_dp = (dp.value > _max_today_dp.value) ? dp : _max_today_dp;
}

/// <summary>
/// Clears running average and min, max for 10-min period.
/// </summary>
void SensorData::_clear_10_min() {
	_sumReadings = 0;
	_countReadings = 0;
	// Reset to highest possible.
	_min_10_min_dp = DataPoint(0, VAL_LIMIT);
	_max_10_min_dp = DataPoint(0, -VAL_LIMIT);
}

int SensorData::countReadings()
{
	return _countReadings;
}

/// <summary>
/// Clears saved minimum and maximum for today.
/// </summary>
void SensorData::clearMinMax_day() {
	// Reset to highest possible.
	_min_today_dp = DataPoint(0, VAL_LIMIT);
	_max_today_dp = DataPoint(0, -VAL_LIMIT);
}

/*****************************************************************
	PERIODIC DATA PROCESSING
******************************************************************/

/// <summary>
/// Calculates 10-min avg and saves data to 10-min 
/// list. Writes this list to file system. WARNING: This 
/// will RESET ACCUMULATED SUMS for 10-min avg and reset 
/// 10-min min and max.
/// </summary>
void SensorData::process_data_10_min() {
	// Avg over last 10 min.
	_avg_10_min = _sumReadings / _countReadings;
	// Add to 10-min list of observations.
	addDataPoint_to_List(_dataPoints_10_min,
		DataPoint(_dataPointLastAdded.time, _avg_10_min),
		SIZE_10_MIN_LIST);
	// Store in LittleFS
	if (_isDatafile) {
		fileWrite(LittleFS,
			_sensorFilepath("_10_min").c_str(),
			dataPoints_10_min_as_String().c_str());
	}
	_clear_10_min();	// Start another 10-min period.
}

/// <summary>
/// Calculates 60-min avg and saves data to 60-min 
/// list. Writes this list to file system.
/// </summary>
void SensorData::process_data_60_min() {
	// Average last 6 x 10 min and add to 60-min list.
	_avg_60_min = listAverage(_dataPoints_10_min, 6);	// Save latest average.
	addDataPoint_to_List(_dataPoints_60_min,
		DataPoint(_dataPointLastAdded.time, _avg_60_min),
		SIZE_60_MIN_LIST);
	// Store in LittleFS
	if (_isDatafile) {
		fileWrite(LittleFS,
			_sensorFilepath("_60_min").c_str(),
			dataPoints_60_min_as_String().c_str());
	}
}

/// <summary>
/// Adds day maximum to dayMax list and day minimum 
/// to dayMin list. Writes a combination of these 
/// lists to the file system.
/// </summary>
void SensorData::process_data_day() {
	// Save list of daily minima and maxima.
	addDataPoint_to_List(_dataPoints_dayMin, _min_today_dp, SIZE_DAY_LIST);
	addDataPoint_to_List(_dataPoints_dayMax, _max_today_dp, SIZE_DAY_LIST);
	clearMinMax_day();
	// Store in LittleFS
	if (_isDatafile) {
		fileWrite(LittleFS,
			_sensorFilepath("_dayMaxMin").c_str(),
			dataPoints_dayMaxMin_as_String().c_str());
	}
}

/*****************************************************************
	DATA RECOVERY FROM FILE SYSTEM
******************************************************************/

/// <summary>
/// Retrieves data points from file system and uses 
/// them to initialize 10-min list in memory. Used to retrieve 
/// any data lost at reboot.
/// </summary>
/// <remarks>
/// LIMIT SIZE OF LIST?! Shouldn't be necessary if 
/// saved data list never exceeds max size!
/// </remarks>
void SensorData::recover_data_10_min_from_file() {
	// Get 10-min data from file system and place in memory.
	if (_isDatafile) {
		// Read file from flash LittleFS.
		String str = fileRead(LittleFS, _sensorFilepath("_10_min").c_str());
		serial_println_DEBUG("In SensorData::recover_data_10_min_from_file with prefix = " + _filenamePrefix + ":\n", str.c_str());
		_dataPoints_10_min = getDataPoints_from_String(str);
	}
}

/// <summary>
/// Retrieves data points from file system and uses 
/// them to initialize 60-min list in memory. Used to retrieve 
/// any data lost at reboot.
/// </summary>
void SensorData::recover_data_60_min_from_file() {
	// Get 10-min data from file system and place in memory.
	if (_isDatafile) {
		// Read file from flash LittleFS.
		String str = fileRead(LittleFS, _sensorFilepath("_60_min").c_str());
		serial_println_DEBUG("SensorData::recover_data_60_min_from_file with prefix = " + _filenamePrefix + ":\n", str.c_str());
		_dataPoints_60_min = getDataPoints_from_String(str);
	}
}

/// <summary>
/// Retrieves data points from file system and uses 
/// them to initialize Day max or min list in memory. Used to retrieve 
/// any data lost at reboot.
/// </summary>
void SensorData::recover_data_dayMaxMin_from_file() {
	// Get 10-min data from file system and place in memory.
	if (_isDatafile) {
		// Read file from flash LittleFS.
		String str = fileRead(LittleFS, _sensorFilepath("_dayMaxMin").c_str());
		serial_println_DEBUG("SensorData::recover_data_dayMaxMin_from_file with prefix = " + _filenamePrefix + ":\n", str.c_str());
		list<String> parts = getStringList_from_String(str, '|');
		int index = 0;
		for (list<String>::iterator it = parts.begin(); it != parts.end(); ++it) {
			String s = *it;
			// Convert string to list of either maxima or minima data points.
			switch (index) {
			case 0:
				// maxima list.
				_dataPoints_dayMax = getDataPoints_from_String(s);
				break;
			case 1:
				// minima list.							
				_dataPoints_dayMin = getDataPoints_from_String(s);
				break;
			default:
				// Unexpected index!
				Serial.printf("ERROR: SensorData::recover_data_dayMaxMin_from_file. Invalid index = %i\n", index);
				break;
			}
			index++;
		}
	}
}

/*****************************************************************
	DELIMITED STRINGS FROM MEMORY
******************************************************************/

/// <summary>
/// Returns list of 10-min dataPoints as delimited string.
/// </summary>
/// <returns>List of 10-min dataPoints as delimited string.</returns>
String SensorData::dataPoints_10_min_as_String() {
	return getString_from_List(_dataPoints_10_min,
		_isConvertZeroToEmpty,
		_decimalPlaces);
}
/// <summary>
/// Returns list of 60-min dataPoints as delimited string.
/// </summary>
/// <returns>List of 60-min dataPoints as delimited string.</returns>
String SensorData::dataPoints_60_min_as_String() {
	return 	getString_from_List(_dataPoints_60_min,
		_isConvertZeroToEmpty,
		_decimalPlaces);
}

/// <summary>
/// Returns combined delimited lists of list of daily 
/// maxima and minima dataPoints,delimited by "|"string.
/// For some sensors, only returns maxima without "|".
/// </summary>
/// <returns>
/// Delimited string of max and min (time, value) lists, 
/// separated by "|". For some sensors, only returns 
/// maxima without "|".</returns>
String SensorData::dataPoints_dayMaxMin_as_String()
{
	if (!_isReportDayMaxOnly) {
		return getString_from_List(
			_dataPoints_dayMax,
			_dataPoints_dayMin,
			_isConvertZeroToEmpty,
			_decimalPlaces);
	}
	else {
		// Do not include minima in list.
		return getString_from_List(
			_dataPoints_dayMax,
			_isConvertZeroToEmpty,
			_decimalPlaces);
	}
}

/// <summary>
/// Returns list of daily maxima dataPoints as delimited string.
/// </summary>
/// <returns>List of maxima dataPoints as delimited string.</returns>
String SensorData::dataPoints_dayMax_as_String() {
	return 	getString_from_List(_dataPoints_dayMax,
		_isConvertZeroToEmpty,
		_decimalPlaces);
}

/// <summary>
/// Returns list of minima dataPoints as delimited string.
/// </summary>
/// <returns>List of minima dataPoints as delimited string.</returns>
String SensorData::dataPoints_dayMin_as_String() {
	return 	getString_from_List(_dataPoints_dayMin,
		_isConvertZeroToEmpty,
		_decimalPlaces);
}


/*****************************************************************
	DATA FILES
******************************************************************/

/// <summary>
/// Creates files that hold sensor data points at various intervals.
/// </summary>
/// <param name="isConvertZeroToEmpty">
/// Set to true to convert zero to empty in output strings.</param>
/// <param name="decimalPlaces">Decimal places in output strings.</param>
void SensorData::createFiles(bool isConvertZeroToEmpty, unsigned int decimalPlaces) {
	_isConvertZeroToEmpty = isConvertZeroToEmpty;
	_decimalPlaces = decimalPlaces;
#if defined(VM_DEBUG)
	if (LittleFS.mkdir(SENSOR_DATA_DIR_PATH)) {
		Serial.printf("Created or found folder %s for %s.\n", SENSOR_DATA_DIR_PATH.c_str(), _filenamePrefix.c_str());
	}
	else {
		Serial.printf("Failed to create or find folder %s for %s.\n", SENSOR_DATA_DIR_PATH.c_str(), _filenamePrefix.c_str());
	}
#endif
	if (!fileCreateOrExists(LittleFS, _sensorFilepath("_10_min"))) {
		Serial.printf("ERROR: Could not create or find %s\n", _sensorFilepath("_10_min").c_str());
	}
	if (!fileCreateOrExists(LittleFS, _sensorFilepath("_60_min"))) {
		Serial.printf("ERROR: Could not create or find %s\n", _sensorFilepath("_60_min").c_str());
	}
	if (!fileCreateOrExists(LittleFS, _sensorFilepath("_dayMaxMin"))) {
		Serial.printf("ERROR: Could not create or find %s\n", _sensorFilepath("_dayMaxMin").c_str());
	}
}

/// <summary>
/// Returns the path for a sensor data text file in LittleFS.
/// </summary>
/// <param name="fileSuffix">A suffix to append to the file name.</param>
/// <returns>Path to a sensor data .txt file.</returns>
String SensorData::_sensorFilepath(String fileSuffix) {
	return SENSOR_DATA_DIR_PATH + "/" + _filenamePrefix + fileSuffix + ".txt";
}

/*****************************************************************
	DUMMY DATA
******************************************************************/

/// <summary>
/// Adds the specified number of elements of dummy data, 
/// incrementing the value each time. Also copies valueStart
/// to valueLastAdded and avg_10_min.
/// </summary>
/// <param name="valueStart">Initial value.</param>
/// <param name="increment">Amount to increment the value each time.
/// </param>
/// <param name="numElements">Number of elements to add.</param>
/// <param name="_timeStartLoop">Time assigned to first data point.
/// </param>
void SensorData::addDummy_data_10_min(float valueStart,
	float increment,
	int numElements,
	unsigned long timeStart) {
	for (int elem = 0; elem < numElements; elem++)
	{
		DataPoint dp{ timeStart, valueStart };
		addDataPoint_to_List(_dataPoints_10_min, dp, SIZE_10_MIN_LIST);
		valueStart += increment;	// increment value each time.
		timeStart += SECONDS_PER_MINUTE * 10;	// 10-min interval
	}
	_dataPointLastAdded = DataPoint(timeStart, valueStart);
	_avg_10_min = valueStart;
}

/// <summary>
/// Adds the specified number of elements of dummy data to the 
/// 10-min list, incrementing the value each time.
/// </summary>
/// <param name="valueStart">Initial value.</param>
/// <param name="increment">Amount to increment the value each time.
/// </param>
/// <param name="numElements">Number of elements to add.</param>
/// <param name="_timeStartLoop">Time assigned to first data point.
/// </param>
void SensorData::addDummy_data_60_min(float valueStart,
	float increment,
	int numElements,
	unsigned long timeStart) {
	// Add artificial data to a 60-min list.	
	for (int elem = 0; elem < numElements; elem++)
	{
		DataPoint dp{ timeStart, valueStart };
		addDataPoint_to_List(_dataPoints_60_min, dp, SIZE_60_MIN_LIST);
		valueStart += increment;		// increment value each time.
		timeStart += SECONDS_PER_HOUR;	// 1-hr interval
	}
	_avg_60_min = valueStart;
}

/// <summary>
/// Adds the specified number of elements of dummy data to the 
/// daily maxima list, incrementing the value each time.
/// </summary>
/// <param name="valueStart">Initial value.</param>
/// <param name="increment">Amount to increment the value each time.
/// </param>
/// <param name="numElements">Number of elements to add.</param>
/// <param name="_timeStartLoop">Time assigned to first data point.
/// </param>
void SensorData::addDummy_data_dayMax(
	float valueStart,
	float increment,
	int numElements,
	unsigned long timeStart) {
	// Add artificial data to a 60-min list.	
	for (int elem = 0; elem < numElements; elem++)
	{
		DataPoint dp{ timeStart, valueStart };
		addDataPoint_to_List(_dataPoints_dayMax, dp, SIZE_60_MIN_LIST);
		valueStart += increment;		// increment value each time.
		timeStart += SECONDS_PER_DAY;	// 1-day interval
	}
	DataPoint dpMax{ timeStart, valueStart };
	_max_today_dp = dpMax;
}

/// <summary>
/// Adds the specified number of elements of dummy data to the 
/// daily minima list, incrementing the value each time.
/// </summary>
/// <param name="valueStart">Initial value.</param>
/// <param name="increment">Amount to increment the value each time.</param>
/// <param name="numElements">Number of elements to add.</param>
/// <param name="_timeStartLoop">Time assigned to first data point.</param>
void SensorData::addDummy_data_dayMin(
	float valueStart,
	float increment,
	int numElements,
	unsigned long timeStart) {
	// Add artificial data to a 60-min list.	
	for (int elem = 0; elem < numElements; elem++)
	{
		DataPoint dp{ timeStart, valueStart };
		addDataPoint_to_List(_dataPoints_dayMin, dp, SIZE_60_MIN_LIST);
		valueStart += increment;		// increment value each time.
		timeStart += SECONDS_PER_DAY;	// 1-day interval
	}
	DataPoint dpMin{ timeStart, valueStart };
	_min_today_dp = dpMin;
}

/*****************************************************************
	RETURN PROPERTIES
******************************************************************/

/// <summary>
/// Returns a (time, value) data point containing the 
/// minimum sensor reading in the current 10-min 
/// period. Updates at every reading and resets when new 
/// 10-min period starts.
/// </summary>
/// <returns>Data point with (time, value) minimum 
/// reading in current 10-min period.</returns>
DataPoint SensorData::min_10_min_dp() {
	return _min_10_min_dp;
}

/// <summary>
/// Returns a (time, value) data point containing the 
/// maximum sensor reading in the current 10-min 
/// period. Updates at every reading and resets when new 
/// 10-min period starts.
/// </summary>
/// <returns>Data point with (time, value) maximum 
/// reading in current 10-min period.</returns>
DataPoint SensorData::max_10_min_dp() {
	return _max_10_min_dp;
}

/// <summary>
/// Data point (time, value) of latest sensor reading.
/// </summary>
/// <returns></returns>
DataPoint SensorData::dataPointLastAdded() {
	return _dataPointLastAdded;
}

/// <summary>
/// The most-recently added data value.
/// </summary>
/// <returns>Most recent reading value.</returns>
float SensorData::valueLastAdded() {
	return _dataPointLastAdded.value;
}

/// <summary>
/// The accumulated avg now (cleared every 10 minutes). When 
/// data smoothing is enabled, outlier values are excluded.
/// </summary>
/// <returns>Average now.</returns>
float SensorData::avg_now() {
	return _sumReadings / _countReadings;
}

///// <summary>
///// Returns moving average of last several reading values.
///// </summary>
///// <returns>Moving average.</returns>
//float SensorData::avg_moving() {
//	return _avg_moving;
//}

/// <summary>
/// The last average saved to the 10-min list.
/// </summary>
/// <returns>10-min average.</returns>
float SensorData::avg_10_min() {
	return _avg_10_min;
}

/// <summary>
/// The last average saved to the 60-min list.
/// </summary>
/// <returns></returns>
float SensorData::avg_60_min() {
	return _avg_60_min;
}

/// <summary>
/// Returns a (time, value) data point containing the 
/// minimum sensor reading today. Updates at 
/// every reading and resets when day rolls over.
/// </summary>
/// <returns>Data point with (time, value) of today's 
/// minimum reading.</returns>
DataPoint SensorData::min_today_dp() {
	return _min_today_dp;
}

/// <summary>
/// Returns a (time, value) data point containing the 
/// maximum sensor reading today. Updates at 
/// every reading and resets when day rolls over.
/// </summary>
/// <returns>Data point with (time, value) of today's 
/// maximum reading.</returns>
DataPoint SensorData::max_today_dp() {
	return _max_today_dp;
}

/// <summary>
/// List of (time, value) dataPoints at 10-min intervals.
/// </summary>
/// <returns>List of (time, value) dataPoints.</returns>
list<DataPoint> SensorData::dataPoints_10_min() {
	return _dataPoints_10_min;
}

/// <summary>
/// List of dataPoints at 60-min intervals.
/// </summary>
/// <returns></returns>
list<DataPoint> SensorData::dataPoints_60_min() {
	return _dataPoints_60_min;
}

/// <summary>
/// List of (time, value) dataPoints of daily minima.
/// </summary>
/// <returns>List of (time, value) dataPoints.</returns>
list<DataPoint> SensorData::dataPoints_day_minima() {
	return _dataPoints_dayMin;
}

/// <summary>
/// List of (time, value) dataPoints of daily maxima.
/// </summary>
/// <returns>List of (time, value) dataPoints.</returns>
list<DataPoint> SensorData::dataPoints_day_maxima() {
	return _dataPoints_dayMax;
}

/// <summary>
/// Returns display label for the data.
/// </summary>
/// <returns>Display label for the data.</returns>
String SensorData::label() {
	return _label;
}

/// <summary>
/// Returns string for constructing data file name.
/// </summary>
/// <returns>String for constructing data file name.</returns>
String SensorData::filenamePrefix() {
	return _filenamePrefix;
}

/// <summary>
/// Data units.
/// </summary>
/// <returns>String</returns>
String SensorData::units() {
	return _units;
}

/// <summary>
/// Sensor units with html encoding.
/// </summary>
/// <returns>String</returns>
String SensorData::units_html() {
	return _units_html;
}

// <summary>
/// Returns true if sensor is configured to report 
/// only daily maximum and not minimum.
/// </summary>
/// <returns>True if sensor is configured to report 
/// only daily maximum.</returns>
bool SensorData::isReportDayMaxOnly()
{
	return _isReportDayMaxOnly;
}

/// <summary>
/// Returns true if data has been saved to LittleFS.
/// </summary>
/// <returns>True if data has been saved to LittleFS.</returns>
bool SensorData::isDatafile() {
	return _isDatafile;
}