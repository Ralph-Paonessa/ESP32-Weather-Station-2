// 
// 
// 

#include "WindGust.h"
#include "DataPoint.h"
#include "App_Settings.h"
#include "DataPoint_Lists.h"
using namespace DataPoint_Lists;
#include "FileOperations.h"
using namespace FileOps;


/// <summary>
/// Adds wind gust (time, value) DataPoint.
/// </summary>
/// <param name="dp">Data point with time and dir angle.</param>
/// <param name="speedAvg">Avg speed at time of reading, mph.</param>
/// <remarks>Overloads inherited SensorData::addReading</remarks>
void WindGust::addReading(DataPoint dp, float speedAvg) {
	_dataPointLastAdded = dp;
	// Only record gusts that match criteria.
	if (dp.value >= GUST_THRESHOLD &&			// Speed exceeds threshold		
		(dp.value - speedAvg >= GUST_SPREAD) &&	// Speed exceeds avg by GUST_SPREAD
		dp.value >= GUST_FACTOR * speedAvg) {	// Speed exceeds avg by GUST_FACTOR	
		// Qualifies as a gust.
		_countGusts++;
		if (dp.value >= _max_10_min_dp.value) {
			// Hold new gust.
			_gust_now = dp;
		}
	}
	_countReadings++;
}

/// <summary>
/// Adds wind direction reading for calculating
/// average direction, weighted by speed.
/// </summary>
/// <param name="time">Reading time.</param>
/// <param name="val">Wind direction reading.</param>
/// <param name="speedAvg">Avg speed at time of reading, mph.</param>
void WindGust::addReading(long int time, float val, float speedAvg) {
	addReading(DataPoint(time, val), speedAvg);
}

/// <summary>
/// Calculates 10-min max gust and saves to 10-min 
/// list. Writes this list to file system. (Resets 
/// accumulated sums for 10-min avg and resets 
/// 10-min min and max.)
/// </summary>
/// <remarks>OVERRIDES SensorData. WindGust uses max, not avg.</remarks>
void WindGust::process_data_10_min() {
	// Add to 10-min list of observations.
	// Gust is max from last 10 min.
	addDataPoint_to_List(_dataPoints_10_min, _max_10_min_dp, SIZE_10_MIN_LIST);
	// Store in LittleFS
	if (_isDatafile) {
		fileWrite(LittleFS,
			_sensorFilepath("_10_min").c_str(),
			dataPoints_10_min_as_String().c_str());
	}
	_clear_10_min();	// Start another 10-min period.
}

/// <summary>
/// Calculates 60-min max gust and saves to 60-min 
/// list. Writes this list to file system.
/// </summary>
/// <remarks>
/// OVERRIDES SensorData. WindGust uses max, not avg.
/// </remarks>
void WindGust::process_data_60_min() {
	// Average last 6 x 10 min and add to 60-min list.
	_gust_60_min_dp = listMaximum(_dataPoints_10_min, 6);	// Get largest gust over past hour.
	addDataPoint_to_List(_dataPoints_60_min,
		DataPoint(_dataPointLastAdded.time, _avg_60_min),	// use most recent data time
		SIZE_60_MIN_LIST);
	// Store in LittleFS
	if (_isDatafile) {
		fileWrite(LittleFS,
			_sensorFilepath("_60_min").c_str(),
			dataPoints_60_min_as_String().c_str());
	}
}

/// <summary>
/// Calculates day max gust and saves to 60-min 
/// list. Writes this list to file system.
/// </summary>
/// <remarks>
/// OVERRIDES SensorData. WindGust uses max, not avg.
/// </remarks>
void WindGust::process_data_day() {
	// Save list of daily minima and maxima.
	addDataPoint_to_List(_dataPoints_dayMin, _min_today_dp, SIZE_DAY_LIST);
	addDataPoint_to_List(_dataPoints_dayMax, _max_today_dp, SIZE_DAY_LIST);
	_clearMinMax_day();
	// Store in LittleFS
	if (_isDatafile) {
		fileWrite(LittleFS,
			_sensorFilepath("_dayMaxMin").c_str(),
			dataPoints_dayMaxMin_as_String().c_str());
	}
}

/// <summary>
/// The maximum gust in current 10-min period. 
/// </summary>
/// <returns>Maximum gust in current 10-min period.</returns>
DataPoint WindGust::gust_now_dp() const {
	return _gust_now;
}

/// <summary>
/// The maximum gust in found today. 
/// </summary>
/// <returns>Maximum gust found today.</returns>
DataPoint WindGust::gust_today_dp() const {
	return _gust_today_dp;
}

/// <summary>
/// Clears accumulated 10-min gust data.
/// </summary>
void WindGust::_clear_10_min() {
	_countReadings = 0;
	_countGusts = 0;
	_gust_now = DataPoint(0, 0);	// reset gust

	// Reset to extremes. (Real values will always surpass these.)
	_min_10_min_dp = DataPoint(0, +VAL_LIMIT);	// not used anywhere!
	_max_10_min_dp = DataPoint(0, -VAL_LIMIT);	// not used anywhere!
}
