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

	DataPoint _gust_now = DataPoint(0, 0);					// Current 10-min gust.
	DataPoint _gust_60_min_dp = DataPoint(0, -SENSOR_VAL_LIMIT);	// Current 60-min maximum gust.
	DataPoint _gust_today_dp = DataPoint(0, 0);				// Today's highest gust.	
		
	/// <summary>
	/// Clears all direction values and averages.
	/// </summary>
	void _clear_10_min() override;

public:

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
	/// Calculates 10-min max gust and saves to 10-min 
	/// list. Writes this list to file system. (Resets 
	/// accumulated sums for 10-min avg and resets 
	/// 10-min min and max.)
	/// </summary>
	/// <remarks>OVERRIDES SensorData!!!</remarks>
	void process_data_10_min() override;

	/// <summary>
	/// Calculates 60-min max gust and saves to 60-min 
	/// list. Writes this list to file system.
	/// </summary>
	/// <remarks>
	/// OVERRIDES SensorData. WindGust uses max, not avg.
	/// </remarks>
	void process_data_60_min() override;

	/// <summary>
	/// Calculates day max gust and saves to 60-min 
	/// list. Writes this list to file system.
	/// </summary>
	/// <remarks>
	/// OVERRIDES SensorData. WindGust uses max, not avg.
	/// </remarks>
	void process_data_day() override;

	/// <summary>
	/// The maximum gust in current 10-min period. 
	/// </summary>
	/// <returns>Maximum gust in current 10-min period.</returns>
	DataPoint gust_now_dp() const;

	/// <summary>
	/// The maximum gust in found today. 
	/// </summary>
	/// <returns>Maximum gust found today.</returns>
	DataPoint gust_today_dp() const;

};

#endif
