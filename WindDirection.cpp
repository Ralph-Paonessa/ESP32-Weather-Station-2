// 
// 
// 

#include "WindDirection.h"

#include <cmath>
#include "DataPoint.h"

/// <summary>
/// Create with an angle offset to subtract from readings.
/// </summary>
/// <param name="offsetAngle">
/// Degrees by which reading exceeds true north.
/// </param>
WindDirection::WindDirection(float offsetAngle) {
	_offsetAngle = offsetAngle;
}

/// <summary>
/// Create the WindDirection object.
/// </summary>
void WindDirection::begin() {
	WindDirection::clear_10_min();
}



void WindDirection::process_data_10_min() {
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
	clear_10_min();	// Start another 10-min period.
}

/// <summary>
/// Clears all direction values and averages.
/// </summary>
/// <remarks>OVERRIDES SensorData to handle vector avgs!</remarks>
void WindDirection::clear_10_min() {
	_eSum = 0;
	_nSum = 0;
	_isDirection_measured = false;
	//_sumReadings = 0;		// invalid for vector avg
	_countReadings = 0;		// INHERITED - WILL WE EVEN USE THIS?! Vector avg doesn't need count!!
}

/*
		XXX  ???
		This solves the problem of updating the read time, even
		though we are ignoring the wind direction at low speeds.

		But what float value of degrees will be added to the list
		of DataPoint(time, value)?! We can't save an item to the
		10-min_today_dp list (and longer) without a numerical value for angle!

		Could also get around this problem by
			- Adding all angle readings
			- WEIGHTING the avg angle by speed.
			- Saving the (possibly meaningless) direction at each 10-min_today_dp
			- Not reporting a direction at times when speed < 1. (Easier
				to do with output strings).

		WEIGHTING BY SPEED will suppress the meaningless directions
		during the 10-min_today_dp average period. If the speed is always zero
		(for all 10 min_today_dp), it will still produce a meaningless direction
		number that will be logged. The REPORTING ROUTINE MUST IGNORE
		DIRECTIONS ASSOCIATED WITH NO SPEED.

		NOTE: If direction angle is logged as exactly zero, (which would
		be the case for a 10-min_today_dp period with no wind)then web server
		returns it as "" for that time.
*/

/// <summary>
/// Adds wind direction reading for calculating 10-min_today_dp 
/// average direction, weighted by speed.
/// </summary>
/// <param name="time">Reading time, sec.</param>
/// <param name="deg">Wind direction (uncorrected), degrees.</param>
/// <param name="speed">Speed at time of reading, mph.</param>
void WindDirection::addReading(long time, float deg, float speed) {
	_dataPointLastAdded = DataPoint(time, deg);
	///////////_timeLastAdded = time;
	// Only record direction for speeds greater than threshold.
	if (speed >= WIND_DIR_SPEED_THRESHOLD) {
		deg = normalizedAngle360(deg -= _offsetAngle);	// Adjust for any offset.
		// Converts angle into North and East components of
		// the direction vector and sums each component.
		// Weight these by the speed.
		_eSum += e_Component(deg) * speed;
		_nSum += n_Component(deg) * speed;
		_isDirection_measured = true;
	}
}

/// <summary>
/// Returns East component of a unit vector based on angle.
/// </summary>
/// <param name="degrees">Angle, degrees.</param>
/// <returns></returns>
float WindDirection::e_Component(float degrees) {
	return sin(degrees / DEGREES_PER_RADIAN);
}

/// <summary>
/// Returns North component of aunit vector based on angle.
/// </summary>
/// <param name="degrees">Angle, degrees.</param>
/// <returns></returns>
float WindDirection::n_Component(float degrees) {
	return cos(degrees / DEGREES_PER_RADIAN);
}

// <summary>
/// Custom arctangent function that returns direction 
/// vector angle based on 0-360 degrees.
/// </summary>
/// <param name="e">East component of direction vector.</param>
/// <param name="n">North component of direction vector.</param>
/// <returns>Direction vector angle from 0-360 degrees.</returns>
/// <remarks>Treats atan2(y, x) as atan2(e, n), with 
/// angle from N(=x) through E(=y) increasing from 0 to 360.</remarks>
float WindDirection::atan_360(float e, float n) {
	float deg = atan2(e, n) * DEGREES_PER_RADIAN;
	// modify negative quadrants 3, 4 (-180 - 180) to positive (0 - 360)
	return normalizedAngle360(deg);
}

/// <summary>
/// Converts an angle to a positive representation from 0-360 degrees.
/// </summary>
/// <param name="deg">Angle in degrees.</param>
/// <returns>Positive angle from 0-360 degrees.</returns>
float WindDirection::normalizedAngle360(float deg) {
	deg = std::fmod(deg, 360.0);	// angle from 0-360, but could be NEGATIVE!
	// Convert neg. angle to pos. representation.
	if (deg < 0.0) {
		deg += 360.0;
	}
	return deg;
}



/// <summary>
/// Returns angle (0-360 deg) of direction vector 
/// using North and East components from any of 
/// four quadrants.
/// </summary>
/// <param name="n">North component.</param>
/// <param name="e">East component.</param>
/// <returns>Wind direction angle (0-360 deg)</returns>
/// <remarks>Number of readings is not used in this "average" 
/// because it is a vector calculation.</remarks>
float WindDirection::angleFromComponents(float n, float e) {
	/*
		tan = Opposite/Adjacent
		tan = y/x -> normal xy coordinates, as used in atan2(y, x)

	Note: atan(y/x) does not distinguish quadrants from 0-360!
	atan2(y, x) does, but returns angles between 180-360 as
	negative angles between 0-180.

	Parameters for angle = atan2 are atan2(y, x) based on
	normal xy cartesian coordinates. The angle is defined
	as starting at the x-axis and increasing in a CCW
	direction towards the y-axis (as in polar coordinates!).
	Adjacent = y, Opposite = x.

	In the definition of direction (azimuth) angle, North = 0 and
	angle increases CW toward East = 90. For that angle, Adj = n
	and Opp = e. Therefore angle = atan2(n, e)  ~  arctangent(e, n)

		atan2(y, x) = atan2(Adj, Opp) = atan2(n, e)

	The azimuth coordinates are similar to polar coordinates but
	the direction of rotation is opposite. In that sense

		angle(azimuth) = 360 - angle(polar)

	NOTE:
		n is y-component (opposite)
		e is x-component (adjacent)
		tan = opposite / adjacent
		Therefore,
		atan2(y, x) = atan2(n, e)

	*/

	float angle = atan2(n, e) * DEGREES_PER_RADIAN;		// Use degrees.
	return normalizedAngle360(angle);
}

/// <summary>
/// Average wind direction from current accumulated readings, deg.
/// </summary>
/// <returns>Average wind direction.</returns>
float WindDirection::avg_now() {
	return normalizedAngle360(angleFromComponents(_eSum, _nSum));
}

/// <summary>
/// Returns true if direction has been calculated from accepted inputs.
/// </summary>
/// <returns>True if direction is valid.</returns>
bool WindDirection::isDirection_measured() {
	return _isDirection_measured;
}

/// <summary>
/// Returns latest cardinal direction using 
/// latest 10-min_today_dp avg wind angle.
/// </summary>
/// <returns>Cardinal direction as string.</returns>
String WindDirection::directionCardinal() {
	return directionCardinal(_avg_10_min);	//	XXX	ONLY VALID AFTER process_data_10_min()
}

/// <summary>
/// 
/// </summary>
/// <param name="n"></param>
/// <param name="e"></param>
/// <returns></returns>
float WindDirection::atan_360_pub(float e, float n) {
	return atan_360(e, n);
}

/// <summary>
/// Public version!
/// </summary>
/// <param name="deg"></param>
/// <returns></returns>
float WindDirection::e_Component_pub(float deg) {
	return e_Component(deg);
}

/// <summary>
/// Public version!
/// </summary>
/// <param name="deg"></param>
/// <returns></returns>
float WindDirection::n_Component_pub(float deg) {
	return n_Component(deg);
}

/// <summary>
/// Return cardinal direction for specified wind angle (deg).
/// </summary>
/// <param name="angle">Wind angle, deg.</param>
/// <returns>Wind cardinal direction.</returns>
String WindDirection::directionCardinal(float angle) {
	if (std::isnan(angle))	// hardware/math error
		return "err: NaN input";
	if (angle < 0.0 || angle >= 360.0)
		return "err bounds" + String(angle);
	if (angle == 0.0)
		return "N (=0)";	// XXX  DEBUGGING!
	if (angle < 22.5)
		return "N";
	if (angle < 67.5)
		return "NE";
	if (angle < 112.5)
		return "E";
	if (angle < 157.5)
		return "SE";
	if (angle < 202.5)
		return "S";
	if (angle < 247.5)
		return "SW";
	if (angle < 292.5)
		return "W";
	if (angle < 337.5)
		return "NW";
	if (angle >= 337.5 && angle < 360.0) // isolate 337.5-360 range
		return "N";
}

/// <summary>
/// Returns list of cardinal directions from list of wind angles.
/// </summary>
/// <param name="angles">List containing wind angles.</param>
/// <returns>List of cardinal direction strings.</returns>
list<String> WindDirection::directions_cardinal_XXX(list<float>& angleList) {
	// Create list to hold cardinal direction strings.
	list<String> cardinals;
	//int i = 0;
	for (list<float>::iterator it = angleList.begin(); it != angleList.end(); ++it) {
		// Add the corresponding cardinal directions.
		cardinals.push_back(directionCardinal(*it));
	}
	return cardinals;
}
