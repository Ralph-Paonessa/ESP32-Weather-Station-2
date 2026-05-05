//// WxWebServer.h
//
//#ifndef _WXWEBSERVER_h
//#define _WXWEBSERVER_h
//
//#if defined(ARDUINO) && ARDUINO >= 100
//#include "arduino.h"
//#else
//#include "WProgram.h"
//#endif
//
//
//#include <AsyncTCP.h>
//
//#include <ESPAsyncWebServer.h>
////#include <LittleFS.h>  XXX NATIVE NOW!!!
//#include "App_Settings.h"
//using namespace App_Settings;
//
//#include "Utilities.h"
//using namespace Utilities;
//
//#include <SD.h>
//#include "SensorData.h"
//#include "GPSModule.h"
//#include "WindDirection.h"
//#include "WindSpeed2.h"
//#include <WString.h>
//
//
///// <summary>
///// Exposes methods to serve weather data from an asynchronous web server.
///// </summary>
//class WxWebServer_XXX {
//
//private:
//
//	GPSModule _gps;
//
//	SensorData _d_Insol;
//	SensorData _d_IRSky_C;
//	SensorData _d_Temp_F;
//	SensorData _d_Pres_seaLvl_mb;
//	SensorData _d_Pres_mb;
//	SensorData _d_RH;
//	SensorData _d_UVIndex;
//	SensorData _d_UVA;
//	SensorData _d_UVB;
//	WindDirection _windDir;
//	WindSpeed _windSpeed;
//	SensorData _windGust;
//	SensorData _d_fanRPM;
//
//	bool _isGood_UV;
//
//public:
//
//	// Constructor???  XXX
//	WxWebServer();
//
//	/// <summary>
//	/// Starts async web server.
//	/// </summary>
//	void begin();
//
//	static void addSensorData(
//		const SensorData& d_Insol,
//		const SensorData& d_IRSky_C,
//		const SensorData& d_Temp_F,
//		const SensorData& d_Pres_seaLvl_mb,
//		const SensorData& d_Pres_mb,
//		const SensorData& d_RH,
//		const SensorData& d_UVIndex,
//		const SensorData& d_UVA,
//		const SensorData& d_UVB,
//		const WindDirection& windDir,
//		const WindSpeed& windSpeed,
//		const SensorData& windGust,
//		const SensorData& d_fanRPM);
//
//	static void addGPS(GPSModule& gps);
//
//
//	static void addData(bool isGood_UV);
//
//	/// <summary>
//	/// Defines uri routes for async web server.
//	/// </summary>
//	static void serverRouteHandler();
//
//	/// <summary>
//	/// Replaces %PLACEHOLDER% elements in 
//	/// files served from async web server.
//	/// </summary>
//	/// <param name="var">Placeholder identifier.</param>
//	/// <returns>String substituted for placeholder.</returns>
//	const String processor(const String& var) const;
//
//};
//
//#endif
