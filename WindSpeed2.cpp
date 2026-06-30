// 
// 
// 

#include "WindSpeed2.h"

// Constructor.

/// <summary> Creates WindSpeed instance that exposes 
/// methods to read and process wind speed data.
/// </summary>
/// <param name="calibrationFactor">
/// Calibration factor for anemometer.</param>
WindSpeed::WindSpeed(float calibrationFactor) {
	_calibrationFactor = calibrationFactor;
}

/// <summary>
/// Returns wind speed from anemometer rotaton count.
/// </summary>
/// <param name="rots"> Number of rotations.</param>
/// <param name="period">Time period of rotations, sec.</param>
/// <returns>Wind speed, mph</returns>
float WindSpeed::speedInstant(int rots, unsigned int period)
{
	/*************************************************************
	Davis anemometer formula:
		speed = rots * 2.25 / time		[from Davis spec].
			where
			time = time during which rots are counted (sec).
	*************************************************************/
	
	
	/*	XXX	return rots * _calibrationFactor / period;*/

	return 111.;	// XXX	XXX
}

/// <summary>
/// Checks for and returns a gust datPoint if the speed satisfies 
/// gust criteria. Otherwise, the returned value will be zero if 
/// the speed doesn't satisfy gust criteria.
/// </summary>
/// <param name="speed">(time, value) point to evaluate for gust.</param>
/// <param name="avgSpeed">Current average wind speed.</param>
/// <returns>Gust as (time, value) data point.</returns>
DataPoint WindSpeed::gust(DataPoint speed, float avgSpeed)
{
	if (speed.value >= GUST_THRESHOLD &&			// Gust exceeds threshold		
		(speed.value - avg_now() >= GUST_SPREAD)) {	// Gust exceeds avg by GUST_SPREAD		
		// Report this as a gust.
		return speed;
	}
	else {
		// Not a gust, so return value of zero.
		return DataPoint(speed.time, 0);
	}
}

/// <summary>
/// Returns wind speed description in Beaufort 
/// wind strength scale.
/// </summary>
/// <param name="speed">Wind speed.</param>
/// <returns>Beaufort wind strength description.</returns>
String WindSpeed::beaufortWind(float speed)
{
	if (speed < 1)
		return "Calm";
	else if (speed < 4)
		return "Light Air";
	else if (speed < 8)
		return "Light Breeze";
	else if (speed < 13)
		return "Gentle Breeze";
	else if (speed < 19)
		return "Moderate Breeze";
	else if (speed < 25)
		return "Fresh Breeze";
	else if (speed < 32)
		return "Strong Breeze";
	else if (speed < 39)
		return "High Wind";
	else
		return "RUN";
}
