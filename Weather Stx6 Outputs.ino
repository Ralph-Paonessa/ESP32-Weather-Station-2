// ==========================   Output   =================== //

/// <summary>
/// Prints opening header to serial port when app starts.
/// </summary>
void printHeader() {
#if defined(VM_DEBUG)
	Serial.println();
	Serial.println(LINE_SEPARATOR);
	Serial.println(F("ESP32 WEATHER STATION"));
	Serial.print(F("  Speed sampling interval = "));
	Serial.print(ANEM_READ_PERIOD_SEC, 2); Serial.println(F(" sec."));
	Serial.println(LINE_SEPARATOR);
#endif
}

/// <summary>
/// Returns a string of tab-delimited column 
/// names for logging to SD card.
/// </summary>
/// <returns>String</returns>
String columnNames() {
	String s = "Time";				// (1)
	s += "\t\t\tT, F";				// (1)
	s += "\tP, mbSL\tP, mbSt";		// (2)
	s += "\tRH %\tBME T,C";			// (2)
	s += "\tSolar%";				// (1)
	s += "\tUV-A\tUV-B\tUV idx";	// (3)
	s += "\tIR, C";					// (1)
	s += "\tWind10\tGust";		// (2)
	s += "\tMax";					// (1)
	s += "\tVane\tFrom";			// (2)
	s += "\tFan rpm";				// (1)
	return s;						// (18 total)
}

/// <summary>
/// Returns current readings for all sensors, 
/// as a delimited string.
/// </summary>
/// <returns>String</returns>
String sensorsDataString_current() {
	// time (1)
	String s = gps.dateTime();
	// temperature (1)
	s += "\t" + String(d_TempF.valueLastAdded());
	// pressure (2)
	s += "\t" + String(d_Pres_seaLvl_mb.valueLastAdded());	// mb adjusted to sea level (MSLP)
	s += "\t" + String(d_Pres_mb.valueLastAdded());			// absolute mb (hPa) (Station P)
	// RH (2)
	s += "\t" + String(d_RH.valueLastAdded());				// %RH
	s += "\t" + String(d_TempC_for_RH.valueLastAdded());	// temp recorded by BME280
	// Solar (1)
	s += "\t" + String(d_Insol.valueLastAdded());			// PV solar cell %
	// UV (3)
	if (isGood_UV) {
		s += "\t" + String(d_UVA.valueLastAdded());
		s += "\t" + String(d_UVB.valueLastAdded());
		s += "\t" + String(d_UVIndex.valueLastAdded());		// Scale 0-10+
	}
	else {
		s += "\tna\tna\tna";
	}
	// IR sky (1)
	s += "\t" + String(d_IRSky_C.valueLastAdded());
	// Wind speed (3)
	s += "\t" + String(windSpeed.avg_10_min());
	s += "\t" + String(windGust.max_10_min_dp().value);
	s += "\tna";	// + String(windSpeed.max_last_10_min);  XXX  ???
	// Wind direction (2)
	if (windSpeed.avg_10_min() > 0.5)
	{
		s += "\t" + String(windDir.angleAvg_now());
		s += "\t" + windDir.directionCardinal();
	}
	else {
		// WindSpeed too low to determine direction.
		s += "\tna";
		s += "\tna";
	}
	// Fan	(1)
	s += "\t" + String(d_fanRPM.valueLastAdded());
	return s;
}

/// <summary>
/// Returns 10-min averages for all 
/// sensors as a delimited string.
/// </summary>
/// <returns>String</returns>
String sensorsDataString_10_min() {
	// time (1)
	String s = gps.dateTime();
	// temperature (1)
	s += "\t" + String(d_TempF.avg_10_min());
	// pressure (2)
	s += "\t" + String(d_Pres_seaLvl_mb.avg_10_min());	// mb adjusted to sea level
	s += "\t" + String(d_Pres_mb.avg_10_min());			// absolute mb (hPa)
	// RH (2)
	s += "\t" + String(d_RH.avg_10_min());				// %RH
	s += "\t" + String(d_TempC_for_RH.avg_10_min());	// temp recorded by BME280
	// Solar (1)
	s += "\t" + String(d_Insol.avg_10_min());			// PV solar cell mV
	// UV (3)
	if (isGood_UV) {
		s += "\t" + String(d_UVA.avg_10_min());
		s += "\t" + String(d_UVB.avg_10_min());
		s += "\t" + String(d_UVIndex.avg_10_min());		// Scale 0-10+
	}
	else {
		s += "\tna\tna\tna";
	}
	// IR sky (1)
	s += "\t" + String(d_IRSky_C.avg_10_min());
	// Wind speed (3)
	s += "\t" + String(windSpeed.avg_10_min());
	s += "\t" + String(windGust.max_10_min_dp().value);
	s += "\tmax?";	// + String(windSpeed.max_last_10_min);  XXX  ???
	// Wind direction (2)
	if (windSpeed.avg_10_min() >= WIND_DIR_SPEED_THRESHOLD)
	{
		s += "\t" + String(windDir.avg_10_min());
		s += "\t" + windDir.directionCardinal();
	}
	else {
		// Speed too low to report wind direction.
		s += "\tna";
		s += "\tna";
	}
	// Fan	(1)
	s += "\t" + String(d_fanRPM.avg_10_min());
	return s;
}

/// <summary>
/// Prints tabbed column headings to serial monitor.
/// </summary>
void PrintColumnHeadings() {
#if defined(VM_DEBUG)
	// Printing column headings to the serial monitor
	// (Tabs are every 8 characters.)
	Serial.print(F("Temperature\t"));
	Serial.print(F("P (sea lvl)\tP (sea lvl)\tP (abs)"));
	Serial.print(F("\t\tHumidity\tTemp (BME)\t"));
	Serial.print(F("Solar\t\t"));
	Serial.print(F("UV - A\tUV - B\tUV Index\t"));
	Serial.print(F("IR\tamb\t"));
	Serial.print(F("Wind avg\tMov Avg\t\tMaximum\t\t"));
	Serial.print(F("From\t"));
	Serial.print(F("Fan rpm"));
	// Advance to new line.
	Serial.println();
#endif
}

/// <summary>
/// Prints a tabbed line of current 
/// sensor data to serial monitor.
/// </summary>
void PrintSensorOutputs() {
#if defined(VM_DEBUG)
	// Temperature.
	Serial.print(d_TempF.valueLastAdded(), 1);
	Serial.print(F("ºF")); Serial.print(F("\t"));
	// Adjust to sea level:
	Serial.print(d_Pres_seaLvl_mb.valueLastAdded(), 1);
	Serial.print(F(" mbar\t"));	// mbar = hPa
	// Absolute pressure
	Serial.print(d_Pres_mb.valueLastAdded(), 1); Serial.print(F(" mb\t"));
	// Relative Humidity
	Serial.print(d_RH.valueLastAdded(), 1); Serial.print(F(" % \t\t"));
	Serial.print(d_TempC_for_RH.valueLastAdded()); Serial.print(F("ºC\t\t"));
	// Solar radiation.
	Serial.print(d_Insol.valueLastAdded(), 1); Serial.print(F(" mV\t"));
	// UV sensor.
	// Use the uva, uvb, and index functions to
	// read calibrated UVA and UVB values and a
	// calculated UV index value between 0-11.
	if (isGood_UV) {
		Serial.print(d_UVA.valueLastAdded()); Serial.print(F("\t"));
		Serial.print(d_UVB.valueLastAdded()); Serial.print(F("\t"));
		Serial.print(d_UVIndex.valueLastAdded()); Serial.print(F("\t\t"));
	}
	else {
		Serial.print("na"); Serial.print(F("\t"));
		Serial.print("na"); Serial.print(F("\t"));
		Serial.print("na"); Serial.print(F("\t\t"));
	}// Remote Infrared.
	Serial.print(d_IRSky_C.valueLastAdded(), 1);
	Serial.print(F("ºC\t"));
	//////Serial.print(windSpeed.speed_last_2_min(), 1);
	//////Serial.print(F(" mph  \t"));

	Serial.print(windSpeed.avg_10_min(), 1);
	Serial.print(F(" mph  \t"));

	Serial.print(windGust.max_10_min_dp().value, 1);
	Serial.print(F(" mph  \t"));

	// Wind direction.
	Serial.print(windDir.angleAvg_now());
	Serial.print(windDir.angleAvg_now()); Serial.print(F("deg\t"));
	Serial.print(windDir.directionCardinal()); Serial.print(F("\t"));
	Serial.print(d_fanRPM.valueLastAdded()); Serial.print(F(" rpm"));
	// Advance to new line.
	Serial.println();
#endif
}
