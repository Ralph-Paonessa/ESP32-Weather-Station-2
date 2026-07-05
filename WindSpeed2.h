// WindSpeed2.h

#ifndef _WINDSPEED2_h
#define _WINDSPEED2_h

#include <Arduino.h>

#include <list>
using std::list;

#include "SensorData.h"

/// <summary>
/// Exposes methods to measure and record wind speeds.
/// </summary>
class WindSpeed : public SensorData {	// Inherits SensorData.

private:

	float _calibrationFactor;			// Anem cal factor to convert rots to speed.

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

	/// <summary>
	/// Returns wind speed description in Beaufort 
	/// wind strength scale.
	/// </summary>
	/// <param name="speed">Beaufort wind strength.</param>
	/// <returns></returns>
	String beaufortWind(float speed);
};

#endif
