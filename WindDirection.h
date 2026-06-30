// WindDirection.h

#ifndef _WINDDIRECTION_h
#define _WINDDIRECTION_h

#include <Arduino.h>

#include "SensorData.h"

/// <summary>
/// Object that reads and handles wind direction measurements.
/// </summary>
class WindDirection : public SensorData {	// Inherits from SensorData.

private:

	float _offsetAngle = 0;		// Degrees that reading exceeds true North.

	float _eSum = 0;	// Accumulated sum of x-component of direction vector.
	float _nSum = 0;	// Accumulated sum of y-component of direction vector.
	float _bearing = 0;	// Wind direction in degrees.

	float angleFromComponents(float n, float e);
	float e_Component(float degrees);
	float n_Component(float degrees);

	// <summary>
	/// Custom arctangent function that returns direction 
	/// vector angle based on 0-360 degrees.
	/// </summary>
	/// <param name="e">East component of direction vector.</param>
	/// <param name="n">North component of direction vector.</param>
	/// <returns>Direction vector angle from 0-360 degrees.</returns>
	/// <remarks>Treats atan2(y, x) as atan2(e, n), with 
	/// angle from N(=x) through E(=y) increasing from 0 to 360.</remarks>
	float atan_360(float e, float n);

	//// <summary>
	/// Converts an angle to a positive representation from 0-360 degrees.
	/// </summary>
	/// <param name="angle_deg">Angle in degrees.</param>
	/// <returns>Positive angle from 0-360 degrees.</returns>
	float normalizedAngle360(float angle_deg);

	/// <summary>
	/// Clears all direction values and averages.
	/// </summary>
	void clear_10_min() override;

	bool _isDirection_measured = false;	// true if direction calculated from valid input

	/// <summary>
	/// Return cardinal direction for specified wind angle (deg).
	/// </summary>
	/// <param name="angle">Wind angle, deg.</param>
	/// <returns>Wind cardinal direction.</returns>
	String directionCardinal(float angle);

public:

	// Constructor

	/// <summary>
	/// Create with an angle offset to subtract from readings.
	/// </summary>
	/// <param name="offsetAngle">
	/// Degrees by which reading exceeds true north.
	/// </param>
	WindDirection(float offsetAngle = 0);

	/// <summary>
	/// Create the WindDirection object.
	/// </summary>
	void begin();

	/// <summary>
	/// Calculates 10-min avg and saves data to 10-min 
	/// list. Writes this list to file system. WARNING: This 
	/// will RESET ACCUMULATED SUMS for 10-min avg and reset 
	/// 10-min min and max.
	/// </summary>
	/// <remarks>OVERRIDES SensorData!!!</remarks>
	void process_data_10_min() override;

	/*/// <summary>
	/// Clears all direction values and averages.
	/// </summary>
	/// <remarks>Should be PRIVATE! XXX</remarks>
	void clear_10_min() override;*/

	/// <summary>
	/// Adds wind direction reading for calculating 10-min 
	/// average direction, weighted by speed.
	/// </summary>
	/// <param name="time">Reading time, sec.</param>
	/// <param name="deg">Wind direction (uncorrected), degrees.</param>
	/// <param name="speed">Speed at time of reading, mph.</param>
	void addReading(long time, float deg, float speed);	// OVERLOAD

	// <summary>
	/// Average wind direction from current accumulated readings, deg.
	/// </summary>
	/// <returns>Average wind direction.</returns>
	virtual float avg_now() override;

	/// <summary>
	/// Returns true if direction has been calculated from accepted inputs.
	/// </summary>
	/// <returns>True if direction is valid.</returns>
	bool isDirection_measured();

	/// <summary>
	/// Returns latest cardinal direction using 
	/// latest 10-min avg wind angle.
	/// </summary>
	/// <returns>Cardinal direction as string.</returns>
	String directionCardinal();

	// <summary>
	/// Custom arctangent function that returns direction 
	/// vector angle based on 0-360 degrees. PUBLIC VERSION!
	/// </summary>
	/// <param name="e">East component of direction vector.</param>
	/// <param name="n">North component of direction vector.</param>
	/// <returns>Direction vector angle from 0-360 degrees.</returns>
	/// <remarks>Treats atan2(y, x) as atan2(e, n), with 
	/// angle from N(=x) through E(=y) increasing from 0 to 360.</remarks>
	float atan_360_pub(float e, float n);

	float e_Component_pub(float deg);
	float n_Component_pub(float deg);

	/// <summary>
	/// Returns list of cardinal directions from list of wind angles.
	/// </summary>
	/// <param name="angles">List containing wind angles.</param>
	/// <returns>List of cardinal direction strings.</returns>
	list<String> directions_cardinal_XXX(list<float>& angleList);
		
};

#endif
