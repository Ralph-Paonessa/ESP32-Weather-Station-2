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

	//float _offsetAngle = 0;		// Degrees that reading exceeds true North.

	//float _eSum = 0;	// Accumulated sum of x-component of direction vector.
	//float _nSum = 0;	// Accumulated sum of y-component of direction vector.
	//float _bearing = 0;	// Wind direction in degrees.
	//bool _isReadingTaken = false;	// true if direction calculated from valid input

	///// <summary>
	///// Returns angle (0-360 deg) of direction vector 
	///// using North and East components from any of 
	///// four quadrants.
	///// </summary>
	///// <param name="n">North component.</param>
	///// <param name="e">East component.</param>
	///// <returns>Wind direction angle (0-360 deg)</returns>
	///// <remarks>Number of readings is not used in this "average" 
	///// because it is a vector calculation.</remarks>
	//float _angleFromComponents(float n, float e);

	///// <summary>
	///// Extracts East component of wind direction unit vector.
	///// </summary>
	///// <param name="degrees">Angle, degrees.</param>
	///// <returns>East component of wind direction unit vector.</returns>.
	//float _e_Component(float degrees);

	///// <summary>
	///// Extracts North component of wind direction unit vector.
	///// </summary>
	///// <param name="degrees">Angle, degrees.</param>
	///// <returns>North component of wind direction unit vector.</returns>.
	//float _n_Component(float degrees);

	///// <summary>
	///// Custom arctangent function that returns direction 
	///// vector angle based on 0-360 degrees.
	///// </summary>
	///// <param name="e">East component of direction vector.</param>
	///// <param name="n">North component of direction vector.</param>
	///// <returns>Direction vector angle from 0-360 degrees.</returns>
	///// <remarks>Treats atan2(y, x) as atan2(e, n), with 
	///// angle from N(=x) through E(=y) increasing from 0 to 360.</remarks>
	//float _atan_360(float e, float n);

	////// <summary>
	///// Converts an angle to a positive representation from 0-360 degrees.
	///// </summary>
	///// <param name="angle_deg">Angle in degrees.</param>
	///// <returns>Positive angle from 0-360 degrees.</returns>
	//float _normalizedAngle360(float angle_deg);

	/// <summary>
	/// Clears all direction values and averages.
	/// </summary>
	void _clear_10_min() override;
	
	/*/// <summary>
	/// Return cardinal direction for specified wind angle (deg).
	/// </summary>
	/// <param name="angle">Wind angle, deg.</param>
	/// <returns>Wind cardinal direction.</returns>
	String _dirCardinal(float angle);*/

public:

	// Constructor

	/// <summary>
	/// Create WindGust object.
	/// </summary>
	WindGust();

	/// <summary>
	/// Create the WindGust object.
	/// </summary>
	void begin();

	/// <summary>
	/// Adds wind direction reading for calculating 10-min 
	/// average direction, weighted by speed.
	/// </summary>
	/// <param name="dp">Data point with time and dir angle.</param>
	/// <param name="speed">Speed at time of reading, mph.</param>
	/// <remarks>Overloads inherited SensorData::addReading</remarks>
	void addReading_WhatToDoWithThis(DataPoint dp, float speed);

	/// <summary>
	/// Adds wind direction reading for calculating 10-min 
	/// average direction, weighted by speed.
	/// </summary>
	/// <param name="time">Reading time.</param>
	/// <param name="val">Wind direction reading.</param>
	/// <param name="speed">Speed at time of reading, mph.</param>
	void addReading_WhatToDoWithThis(long int time, float val, float speed);

	/// <summary>
	/// Returns true if speed is a wind gust.
	/// </summary>
	/// <param name="speed">Wind speed reading.</param>
	/// <param name="speed_avg">Current average wind speed.</param>
	/// <returns>True if speed is a wind gust.</returns>
	bool isGust(float speed, float speed_avg);

	/// <summary>
	/// Calculates 10-min avg and saves data to 10-min 
	/// list. Writes this list to file system. WARNING: This 
	/// will RESET ACCUMULATED SUMS for 10-min avg and reset 
	/// 10-min min and max.
	/// </summary>
	/// <remarks>OVERRIDES SensorData!!!</remarks>
	void process_data_10_min() override;

	//// <summary>
	///// Average wind direction from current accumulated readings, deg.
	///// </summary>
	///// <returns>Average wind direction.</returns>
	//virtual float avg_now() override;
	
};

#endif
