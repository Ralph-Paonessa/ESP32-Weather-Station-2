// WindGust.h

#ifndef _WINDGUST_h
#define _WINDGUST_h

#include <Arduino.h>

#include "SensorData.h"

/// <summary>
/// Object that reads and handles wind gust measurements.
/// </summary>
class WindGust : public SensorData {	// Inherits from SensorData.

private:

	int _countGusts = 0;

	DataPoint _gust_10_min_dp = DataPoint(0, 0);		// Current 10-min gust.
	DataPoint _gust_today_dp = DataPoint(0, 0);		// Today's highest gust.
		
	/// <summary>
	/// Clears all direction values and averages.
	/// </summary>
	void _clear_10_min() override;

public:

	// Constructor

	/// <summary>
	/// Create WindGust object.
	/// </summary>
	WindGust();

	/// <summary>
	/// Adds wind speed for gust determination in 10-min interval.
	/// </summary>
	/// <param name="dp">Data point with time and dir angle.</param>
	/// <remarks>Overloads inherited SensorData::addReading</remarks>
	/// <param name="speedAvg">Avg speed at time of reading, mph.</param>
	void addReading(DataPoint dp, float speedAvg);

	/// <summary>
	/// Adds wind speed for gust determination in 10-min interval.
	/// </summary>
	/// <param name="time">Reading time.</param>
	/// <param name="val">Wind direction reading.</param>
	/// <param name="speedAvg">Avg speed at time of reading, mph.</param>
	void addReading(long int time, float val, float speedAvg);

	/// <summary>
	/// Calculates 10-min avg and saves data to 10-min 
	/// list. Writes this list to file system. WARNING: This 
	/// will RESET ACCUMULATED SUMS for 10-min avg and reset 
	/// 10-min min and max.
	/// </summary>
	/// <remarks>OVERRIDES SensorData!!!</remarks>
	void process_data_10_min() override;


	DataPoint gust_10_min_dp();

	DataPoint gust_today_dp();

};

#endif
