// 
// XXX
// 

#include "WindGust.h"
#include "DataPoint.h"

/// <summary>
/// Create WindGust object.
/// </summary>
WindGust::WindGust() {
}

/// <summary>
/// Create the WindGust object.
/// </summary>
void WindGust::begin() {
	WindGust::_clear_10_min();
}


/// <summary>
/// Adds wind gust (time, value) DataPoint.
/// </summary>
/// <param name="dp">Data point with time and dir angle.</param>
/// <param name="speed">Speed at time of reading, mph.</param>
/// <remarks>Overloads inherited SensorData::addReading</remarks>
void WindGust::addReading_WhatToDoWithThis(DataPoint dp, float speed) {
	_dataPointLastAdded = dp;
	///////////_timeLastAdded = time;
	// Only record direction for speeds greater than threshold.
	if (speed >= WIND_DIR_SPEED_THRESHOLD) {


		_countReadings++;

		Serial.printf("_countReadings = %d\n", _countReadings);

	}
}

/// <summary>
/// Adds wind direction reading for calculating
/// average direction, weighted by speed.
/// </summary>
/// <param name="time">Reading time.</param>
/// <param name="val">Wind direction reading.</param>
/// <param name="speed">Speed at time of reading, mph.</param>
void WindGust::addReading_WhatToDoWithThis(long int time, float val, float speed) {
	addReading_WhatToDoWithThis(DataPoint(time, val), speed);
}


/// <summary>
/// Returns true if speed is a wind gust.
/// </summary>
/// <param name="speed">Wind speed reading.</param>
/// <param name="speed_avg">Current average wind speed.</param>
/// <returns>True if speed is a wind gust.</returns>
bool WindGust::isGust(float speed, float speed_avg) {
	if (speed >= GUST_THRESHOLD &&			// Speed exceeds threshold		
		(speed - speed_avg >= GUST_SPREAD) &&	// Speed exceeds avg by GUST_SPREAD
		speed >= GUST_FACTOR * avg_now()) {	// Speed exceeds avg by GUST_FACTOR	
		// Record this as a gust.
		return true;
	}
	else {
		return false;
	}
}

/// <summary>
/// Creates a 10-min data point and adds to 10-min list.
/// </summary>
void WindGust::process_data_10_min() {
	// Avg over last 10 min.
	_avg_10_min = avg_now();
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
/// Clears all direction values and averages.
/// </summary>
void WindGust::_clear_10_min() {
	/*_eSum = 0;
	_nSum = 0;
	_isReadingTaken = false;*/
	_countReadings = 0;		// INHERITED - WILL WE EVEN USE THIS?! Vector avg doesn't need count!!
}

///// <summary>
///// Average wind direction from current accumulated readings, deg.
///// </summary>
///// <returns>Average wind direction.</returns>
//float WindGust::avg_now() {
//	return _normalizedAngle360(_angleFromComponents(_eSum, _nSum));
//}
