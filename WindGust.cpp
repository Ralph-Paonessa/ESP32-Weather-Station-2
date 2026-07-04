// 
// XXX
// 

#include "WindGust.h"
#include "DataPoint.h"

/// <summary>
/// Creates WindGust object.
/// </summary>
WindGust::WindGust() {
}

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
			_gust_10_min_dp = dp;
		}
	}
	_countReadings++;

	Serial.printf("WindGust::addReading: _countReadings = %i, _countGusts = %i\n", _countReadings, _countGusts);
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
/// Creates a 10-min data point and adds to 10-min list.
/// </summary>
/// <remarks>NOTE: WindGust uses max, not avg.</remarks>
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

DataPoint WindGust::gust_10_min_dp() {
	return _gust_10_min_dp;
}

DataPoint WindGust::gust_today_dp() {
	return _gust_today_dp;
}

/// <summary>
/// Clears accumulated 10-min gust data.
/// </summary>
void WindGust::_clear_10_min() {
	_countReadings = 0;
	_countGusts = 0;
	_gust_10_min_dp = DataPoint(0, 0);	// reset gust

	// Reset to extremes. (Real values will always surpass these.)
	_min_10_min_dp = DataPoint(0, +VAL_LIMIT);	// not used anywhere!
	_max_10_min_dp = DataPoint(0, -VAL_LIMIT);	// not used anywhere!
}
