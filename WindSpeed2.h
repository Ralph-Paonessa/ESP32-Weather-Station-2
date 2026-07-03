// WindSpeed2.h

#ifndef _WINDSPEED2_h
#define _WINDSPEED2_h

#include <Arduino.h>

#include <list>
using std::list;

#include "DataPoint_Lists.h"
#include "App_settings.h"

using namespace DataPoint_Lists;
using namespace App_Settings;

#include "SensorData.h"

/// <summary>
/// Exposes methods to measure and record wind speeds.
/// </summary>
class WindSpeed : public SensorData {	// Inherits SensorData.

private:

	float _calibrationFactor;					// Anem cal factor to convert rots to speed.

	//// For identification of gusts.
	//static constexpr float GUST_THRESHOLD = 8;	// Minimum speed that qualifies as gust.
	//static constexpr float GUST_SPREAD = 10.36;	// Speed must exceed avg speed by this amount.
	//static constexpr float GUST_FACTOR = 1.5;	// Speed must exceed avg by this factor.

public:

	// Constructor	

	/// <summary> Creates WindSpeed instance that exposes 
	/// methods to read and process wind speed data.
	/// </summary>
	/// <param name="calibrationFactor">
	/// Calibration factor for anemometer.</param>
	WindSpeed(float calibrationFactor);

	/// <summary>
	/// Returns wind speed from anemometer rotations_VOL.
	/// </summary>
	/// <param name="rotations_VOL"> Number of rotations_VOL.</param>
	/// <param name="period">Time period of rotations_VOL, sec.</param>
	/// <returns>Wind speed, mph</returns>
	float speedInstant(int rotations_VOL, unsigned int period);

	/*/// <summary>
	/// Checks for and returns a gust datPoint if the speed satisfies 
	/// gust criteria. Otherwise, the returned value will be zero if 
	/// the speed doesn't satisfy gust criteria.
	/// </summary>
	/// <param name="speed">(time, value) point to evaluate for gust.</param>
	/// <param name="avgSpeed">Current average wind speed.</param>
	/// <returns>Gust as (time, value) data point.</returns>
	DataPoint gust(DataPoint speed, float avgSpeed);*/

	/// <summary>
	/// Returns wind speed description in Beaufort 
	/// wind strength scale.
	/// </summary>
	/// <param name="speed">Beaufort wind strength.</param>
	/// <returns></returns>
	String beaufortWind(float speed);
};

#endif
