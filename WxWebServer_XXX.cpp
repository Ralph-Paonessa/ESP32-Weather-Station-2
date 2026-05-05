//// 
//// 
//// 
//
//#include <AsyncTCP.h>
//#include "WxWebServer.h"
//#include "SensorData.h"
//#include <SD.h>
////#include <SD.h>
//
////// ==========   Async Web Server   ================== //
////AsyncWebServer server(80);	// Async web server instance on port 80.
//
//chartRequested _chart_request = CHART_NONE;	// Chart requested from server.
//
//bool _isChart_max_min = false;	// True when chart from server is max/min.
//
//AsyncWebServer server(80);	// Async web server instance on port 80.
//
///// <summary>
///// Default constructor for WxWebServer; initializes a new instance.  XXX ???
///// </summary>
//WxWebServer::WxWebServer_XXX()
//{
//}
//
///// <summary>
///// Starts async web server.
///// </summary>
//void WxWebServer::begin() {
//	server.begin();
//}
//
//void WxWebServer::addSensorData(
//	const SensorData& d_Insol,
//	const SensorData& d_IRSky_C,
//	const SensorData& d_Temp_F,
//	const SensorData& d_Pres_seaLvl_mb,
//	const SensorData& d_Pres_mb,
//	const SensorData& d_RH,
//	const SensorData& d_UVIndex,
//	const SensorData& d_UVA,
//	const SensorData& d_UVB,
//	const WindDirection& windDir,
//	const WindSpeed& windSpeed,
//	const SensorData& windGust,
//	const SensorData& d_fanRPM)
//{
//	_d_Insol = &d_Insol;
//	_d_IRSky_C = &d_IRSky_C;
//
//	_d_Temp_F = &d_Temp_F;
//	_d_Pres_seaLvl_mb = &d_Pres_seaLvl_mb;
//	_d_Pres_mb = &d_Pres_mb;
//	_d_RH = &d_RH;
//	_d_UVIndex = &d_UVIndex;
//	_d_UVA = &d_UVA;
//	_d_UVB = &d_UVB;
//	_windDir = &windDir;
//	_windSpeed = &windSpeed;
//	_windGust = &windGust;
//	_d_fanRPM = &d_fanRPM;
//}
//
//void WxWebServer::addGPS(GPSModule& gps)
//{
//	_gps = &gps;
//}
//
//void WxWebServer::addData(bool isGood_UV)
//{
//	_isGood_UV = isGood_UV;
//}
//
///// <summary>
///// Defines uri routes for async web server.
///// </summary>
//void WxWebServer::serverRouteHandler() {
//	/*
//	Configure url routes where server will be listening. Route in url
//	is: "http://[ IP Address ][Route]",	such as [Route]= "/data".
//
//	Serving files from both LittleFS (flash memory) and SD (SD card).
//
//	STATIC FILES:
//
//		"Static" files are generally files that are "not changed
//		by the server" such as css, js, and non-dynamic html pages.
//
//		Usage:
//			Format is server.serveStatic([ Route ], [ File system ], [ File path ]);
//
//		Example:
//		To serve the file "/dir/page.htm" when request url is "/page.htm":
//
//			server.serveStatic("/page.htm", LittleFS, "/dir/page.htm");
//
//	DYNAMIC FILES:
//
//		Files can contain templates of the form	%REPLACE_ME% that are
//		replaced by a processor function attached to the request.
//
//		server.on adds a new instance of AsyncStaticWebHandler
//		to the server to handle the specified file.
//
//		(Note: ESPAsyncWebServer ReadMe says you can use template processor
//		with "static" files. This seems contradictory; here, I refer to
//		them as dynamic files.)
//
//		Usage:
//			server.on([ Route ], HTTP_GET, [](AsyncWebServerRequest* request)
//				{
//				request->send([ File system ], [ File path ] , [ Mime type ], [ isDownload ], [ template processor ]);
//				});
//
//		Example:
//			server.on("/Admin", HTTP_GET, [](AsyncWebServerRequest* request)
//				{
//				request->send(LittleFS, "/html/Admin.html", "text/html", false, processor);
//				});
//
//		IMPORTANT: false is for bool download; otherwise browser will
//		download file instead of display it!
//
//
//	SETTING CACHE FOR STATIC FILES:
//
//		Usage:
//			server.serveStatic([Route], [File system], [File path]).setCacheControl("max-age=[seconds]");
//
//		Example:
//			server.serveStatic("/highcharts.css", LittleFS, "/css/highcharts.alt.css").setCacheControl("max-age=864000");
//
//			10 days = 864,000 seconds
//			 2 days = 172,800 seconds
//			 1 day  =  86,400 seconds
//	*/
//
//	//#if defined(VM_DEBUG)		XXX MOVE THIS ELSEWHERE!!!
//	//	if (!_isDEBUG_BypassWebServer) {
//	//#endif
//			// Set cache for static files.
//			// css
//	server.serveStatic("/highcharts.css", LittleFS, "/css/highcharts.alt.css").setCacheControl("max-age=864000");
//	server.serveStatic("/highcharts-custom.css", LittleFS, "/css/highcharts-custom.css").setCacheControl("max-age=864000");
//	server.serveStatic("/style.light.min.css", LittleFS, "/css/style.light.min.css").setCacheControl("max-age=864000");
//	server.serveStatic("/style.min.css", LittleFS, "/css/style.min.css").setCacheControl("max-age=864000");
//	// js
//	server.serveStatic("/highcharts.js", LittleFS, "/js/highcharts.js").setCacheControl("max-age=864000");
//	server.serveStatic("/chart.js", LittleFS, "/js/chart.js").setCacheControl("max-age=864000");
//	//server.serveStatic("/chart_2.js", LittleFS, "/js/chart_2.js").setCacheControl("max-age=864000");
//	// img
//	server.serveStatic("/chart-icon.png", LittleFS, "/img/chart-icon-red-150px.png").setCacheControl("max-age=864000");
//	server.serveStatic("/home-icon.png", LittleFS, "/img/home-icon-red-150px.png").setCacheControl("max-age=864000");
//	server.serveStatic("/img/loading.gif", LittleFS, "/img/loading.gif").setCacheControl("max-age=864000");
//	server.serveStatic("/favicon-32.png", LittleFS, "/img/favicon-32.png").setCacheControl("max-age=864000");
//	server.serveStatic("/favicon-32.180", LittleFS, "/img/favicon-32.180").setCacheControl("max-age=864000");
//	// html 
//	// Our html pages are dynamic and can't be cached.
//
//	/*****  WEB PAGES.  *****/
//
//	// Default.
//	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/html/sensors.html", "text/html", false, processor);
//		});
//
//	// Summary data.
//	server.on("/summary", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/html/summary.html", "text/html", false, processor);
//		});
//
//	// GPS info.
//	server.on("/gps", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/html/gps.html", "text/html", false, processor);
//		});
//
//	// Admin page.
//	server.on("/Admin", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/html/Admin.html", "text/html", false, processor);
//		});
//
//	// Log file from SD card.
//	server.on("/log", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(SD, "/log.txt", "text/plain");
//		});
//
//	// Data file from SD card.
//	server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(SD, "/data.txt", "text/plain");
//		});
//
//	/*****  GRAPH PAGES.  *****/
//
//	// Pages use javascript to get data asynchronously.
//
//	// Temperature graph page.
//	server.on("/chart_T", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_TEMPERATURE_F;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Wind speed graph page.
//	server.on("/chart_W", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_WIND_SPEED;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Wind gust graph page.
//	server.on("/chart_Wgst", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_WIND_GUST;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Wind direction graph page.
//	server.on("/chart_Wdir", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_WIND_DIRECTION;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Pressure (sea level) graph page.
//	server.on("/chart_P", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_PRESSURE_SEA_LEVEL;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Relative Humidity graph page.
//	server.on("/chart_RH", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_RELATIVE_HUMIDITY;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Sky Infrared graph page.
//	server.on("/chart_IR", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_IR_SKY;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// UV Index graph page.
//	server.on("/chart_UVIndex", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_UV_INDEX;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	// Insolation graph page.
//	server.on("/chart_Insol", HTTP_GET, [](AsyncWebServerRequest* request) {
//		_chart_request = CHART_INSOLATION;
//		request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
//		});
//
//	/*****  Images.  *****/
//
//	server.on("/favicon-32.png", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/img/favicon-32.png", "image/png");
//		});
//	server.on("/favicon-180.png", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/img/favicon-180.png", "image/png");
//		});
//	server.on("/chart-icon.png", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/img/chart-icon-red-150px.png", "image/png");
//		});
//	server.on("/home-icon.png", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/img/home-icon-red-150px.png", "image/png");
//		});
//	server.on("/img/loading.gif", HTTP_GET, [](AsyncWebServerRequest* request) {
//		request->send(LittleFS, "/img/loading.gif", "image/gif");
//		});
//
//	/*****  CSS and Javascript.  *****/
//
//	// CSS style sheet.
//	server.on("/style.min.css",
//		HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			request->send(LittleFS, "/css/style.min.css", "text/css");
//		});
//	// CSS light-theme style sheet.
//	server.on("/style.light.min.css",
//		HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			request->send(LittleFS, "/css/style.light.min.css", "text/css");
//		});
//	// Highcharts css style sheet.
//	server.on("/highcharts.css",
//		HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			request->send(LittleFS, "/css/highcharts.alt.css", "text/css");
//		});
//	// Highcharts customized css style sheet.
//	server.on("/highcharts-custom.css",
//		HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			request->send(LittleFS, "/css/highcharts-custom.css", "text/css");
//		});
//	// highcharts javascript file.
//	server.on("/highcharts.js",
//		HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			request->send(LittleFS, "/js/highcharts.js", "text/javascript");
//		});
//	// highcharts custom javascript file.
//	server.on("/chart.js",
//		HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			request->send(LittleFS, "/js/chart.js", "text/javascript");
//		});
//
//	/*****  DATA SOURCES FOR GRAPHS  ***********************************/
//	/*
//		 Asynchronously Send string with data to html
//		 page where Javascript parses and plots the data.
//	*/
//
//	/*****  10-MIN CHARTS  *****/
//
//	server.on("/data_10", HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//
//			_isChart_max_min = false;
//
//			// Which chart?
//			switch (_chart_request)
//			{
//			case CHART_NONE:
//				request->send(200, "text/plain", "");
//				break;
//			case CHART_INSOLATION:
//				request->send(200, "text/plain", _d_Insol->data_10_min_string().c_str());
//				break;
//			case CHART_IR_SKY:
//				request->send(200, "text/plain", _d_IRSky_C->data_10_min_string().c_str());
//				break;
//			case CHART_TEMPERATURE_F:
//				request->send(200, "text/plain", _d_Temp_F->data_10_min_string().c_str());
//				break;
//			case CHART_PRESSURE_SEA_LEVEL:
//				request->send(200, "text/plain", _d_Pres_seaLvl_mb->data_10_min_string().c_str());
//				break;
//			case CHART_RELATIVE_HUMIDITY:
//				request->send(200, "text/plain", _d_RH->data_10_min_string().c_str());
//				break;
//			case CHART_UV_INDEX:
//				request->send(200, "text/plain", _d_UVIndex->data_10_min_string().c_str());
//				break;
//			case CHART_WIND_DIRECTION:
//				request->send(200, "text/plain", _windDir->data_10_min_string().c_str());
//				break;
//			case CHART_WIND_SPEED:
//				request->send(200, "text/plain", _windSpeed->data_10_min_string().c_str());
//				break;
//			case CHART_WIND_GUST:
//				request->send(200, "text/plain", _windGust->data_10_min_string().c_str());
//				break;
//			default:
//				request->send(200, "text/plain", "");
//				break;
//			}
//		});
//
//	/*****  60-MIN CHARTS  *****/
//
//	server.on("/data_60", HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//
//			_isChart_max_min = false;
//
//			// Which chart?
//			switch (_chart_request)
//			{
//			case CHART_NONE:
//				request->send(200, "text/plain", "");
//				break;
//			case CHART_INSOLATION:
//				request->send(200, "text/plain", _d_Insol->data_60_min_string().c_str());
//				break;
//			case CHART_IR_SKY:
//				request->send(200, "text/plain", _d_IRSky_C->data_60_min_string().c_str());
//				break;
//			case CHART_TEMPERATURE_F:
//				request->send(200, "text/plain", _d_Temp_F->data_60_min_string().c_str());
//				break;
//			case CHART_PRESSURE_SEA_LEVEL:
//				request->send(200, "text/plain", _d_Pres_seaLvl_mb->data_60_min_string().c_str());
//				break;
//			case CHART_RELATIVE_HUMIDITY:
//				request->send(200, "text/plain", _d_RH->data_60_min_string().c_str());
//				break;
//			case CHART_UV_INDEX:
//				request->send(200, "text/plain", _d_UVIndex->data_60_min_string().c_str());
//				break;
//			case CHART_WIND_DIRECTION:
//				request->send(200, "text/plain", _windDir->data_60_min_string().c_str());
//				break;
//			case CHART_WIND_SPEED:
//				request->send(200, "text/plain", _windSpeed->data_60_min_string().c_str());
//				break;
//			case CHART_WIND_GUST:
//				request->send(200, "text/plain", _windGust->data_60_min_string().c_str());
//				break;
//			default:
//				request->send(200, "text/plain", "");
//				break;
//			}
//		});
//
//	/*****  DAILY MIN MAX CHARTS  *****/
//
//	server.on("/data_max_min", HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//
//			_isChart_max_min = true;
//
//			// Which chart?
//			switch (_chart_request)
//			{
//			case CHART_NONE:
//				request->send(200, "text/plain", "");
//				break;
//			case CHART_INSOLATION:
//				request->send(200, "text/plain", _d_Insol->data_dayMaxMin_string().c_str());
//				break;
//			case CHART_IR_SKY:
//				request->send(200, "text/plain", _d_IRSky_C->data_dayMaxMin_string().c_str());
//				break;
//			case CHART_TEMPERATURE_F:
//				request->send(200, "text/plain", _d_Temp_F->data_dayMaxMin_string().c_str());
//				break;
//			case CHART_PRESSURE_SEA_LEVEL:
//				request->send(200, "text/plain", _d_Pres_seaLvl_mb->data_dayMaxMin_string().c_str());
//				break;
//			case CHART_RELATIVE_HUMIDITY:
//				request->send(200, "text/plain", _d_RH->data_dayMaxMin_string().c_str());
//				break;
//			case CHART_UV_INDEX:
//				request->send(200, "text/plain", _d_UVIndex->data_dayMaxMin_string().c_str());
//				break;
//				/*case CHART_WIND_DIRECTION:
//					request->send(200, "text/plain", _windDir->data_max_min_string_delim().c_str());
//					break;*/
//			case CHART_WIND_SPEED:
//				request->send(200, "text/plain", _windSpeed->data_dayMaxMin_string().c_str());
//				break;
//			case CHART_WIND_GUST:
//				request->send(200, "text/plain", _windGust->data_dayMaxMin_string().c_str());
//				break;
//			default:
//				request->send(200, "text/plain", "");
//				break;
//			}
//		});
//
//	/*****  DAILY MAXIMA CHARTS  *****/
//
//	server.on("/data_max", HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			// Which chart?
//			switch (_chart_request)
//			{
//			case CHART_NONE:
//				request->send(200, "text/plain", "");
//				break;
//			case CHART_INSOLATION:
//				request->send(200, "text/plain", _d_Insol->data_dayMax_string().c_str());
//				break;
//			case CHART_IR_SKY:
//				request->send(200, "text/plain", _d_IRSky_C->data_dayMax_string().c_str());
//				break;
//			case CHART_TEMPERATURE_F:
//				request->send(200, "text/plain", _d_Temp_F->data_dayMax_string().c_str());
//				break;
//			case CHART_PRESSURE_SEA_LEVEL:
//				request->send(200, "text/plain", _d_Pres_seaLvl_mb->data_dayMax_string().c_str());
//				break;
//			case CHART_RELATIVE_HUMIDITY:
//				request->send(200, "text/plain", _d_RH->data_dayMax_string().c_str());
//				break;
//			case CHART_UV_INDEX:
//				request->send(200, "text/plain", _d_UVIndex->data_dayMax_string().c_str());
//				break;
//			case CHART_WIND_DIRECTION:
//				request->send(200, "text/plain", _windDir->data_dayMax_string().c_str());
//				break;
//			case CHART_WIND_SPEED:
//				request->send(200, "text/plain", _windSpeed->data_dayMax_string().c_str());
//				break;
//			case CHART_WIND_GUST:
//				request->send(200, "text/plain", _windGust->data_dayMax_string().c_str());
//				break;
//			default:
//				request->send(200, "text/plain", "");
//				break;
//			}
//		});
//
//	/*****  DAILY MINIMA CHARTS  *****/
//
//	server.on("/data_min", HTTP_GET,
//		[](AsyncWebServerRequest* request) {
//			// Which chart?
//			switch (_chart_request)
//			{
//			case CHART_NONE:
//				request->send(200, "text/plain", "");
//				break;
//			case CHART_INSOLATION:
//				request->send(200, "text/plain", _d_Insol->data_dayMin_string().c_str());
//				break;
//			case CHART_IR_SKY:
//				request->send(200, "text/plain", _d_IRSky_C->data_dayMin_string().c_str());
//				break;
//			case CHART_TEMPERATURE_F:
//				//request->send(200, "text/plain", _d_Temp_F->data_dayMin_string().c_str());
//				request->send(200, "text/plain", _d_Temp_F->data_dayMin_string().c_str());
//				break;
//			case CHART_PRESSURE_SEA_LEVEL:
//				request->send(200, "text/plain", _d_Pres_seaLvl_mb->data_dayMin_string().c_str());
//				break;
//			case CHART_RELATIVE_HUMIDITY:
//				request->send(200, "text/plain", _d_RH->data_dayMin_string().c_str());
//				break;
//			case CHART_UV_INDEX:
//				request->send(200, "text/plain", _d_UVIndex->data_dayMin_string().c_str());
//				break;
//			case CHART_WIND_DIRECTION:
//				request->send(200, "text/plain", _windDir->data_dayMin_string().c_str());
//				break;
//			case CHART_WIND_SPEED:
//				request->send(200, "text/plain", _windSpeed->data_dayMin_string().c_str());
//				break;
//			case CHART_WIND_GUST:
//				request->send(200, "text/plain", _windGust->data_dayMin_string().c_str());
//				break;
//			default:
//				request->send(200, "text/plain", "");
//				break;
//			}
//		});
//
//#if defined(VM_DEBUG)
//}
//#else 
//	Serial.println("BYPASSING WEB SERVER INITIALIZATION");
//}
//#endif
//
//
//// ==========   SERVER DYNAMIC HTML   ================ //
//
///// <summary>
///// Replaces %PLACEHOLDER% elements in 
///// files served from async web server.
///// </summary>
///// <param name="var">Placeholder identifier.</param>
///// <returns>String substituted for placeholder.</returns>
//String WxWebServer::processor(const String& var) const {
//
//	/// CSS LIGHT STYLE DURING DAY  //////////////
//
//	if (var == "CSS_LIGHT_STYLE") {
//		// Switch display theme when ambient light is,
//		// detected by normalized insolation %.
//
//
//		return"";
//
//
//		///////*if (_d_Insol.valueLastAdded() > 0.01) {
//		//////	return "<link href = ""style.light.min.css"" rel = ""stylesheet"" media = ""all"" type = ""text/css"" />";
//		//////}
//		//////else {
//		//////	return "";
//		//////}*/
//	}
//
//	///  CURRENT SENSOR READINGS  ///////////////////
//
//	// Returns running avg of current 10-min 
//	// period (except for gust and wind dir).
//
//	if (var == "LAST_READINGS_DATETIME") {
//		return _gps.dateTime();
//	}
//	if (var == "CURRENT_TIME") {
//		return _gps.time();
//	}
//	if (var == "WEEKDAY") {
//		return String(_gps.dayName());
//	}
//	if (var == "TEMPERATURE_F")
//		return String(_d_Temp_F.avg_now(), 0);
//	if (var == "WIND_SPEED") {
//		return String(_windSpeed.avg_now(), 0);	// 10-min avg
//	}
//	if (var == "WIND_GUST") {
//		return String(_windGust.max_10_min().value, 0);	// 10-min max for gusts
//	}
//	if (var == "WIND_DIRECTION") {
//		return String(_windDir.directionCardinal());		// avg since last cleared (<= 10 min)
//	}
//	if (var == "WIND_ANGLE") {
//		return String(_windDir.angleAvg_now(), 0);		// avg since last cleared (<= 10 min)
//	}
//	if (var == "GPS_ALTITUDE") {
//		return String(_gps.data.altitude(), 0);
//	}
//	if (var == "PRESSURE_MB_SL") {
//		return String(_d_Pres_seaLvl_mb.avg_now(), 0);
//	}
//	if (var == "PRESSURE_MB_ABS") {
//		return String(_d_Pres_mb.avg_now(), 0);
//	}
//	if (var == "WATER_BOILING_POINT") {
//		return String(Utilities::waterBoilingPoint_F(_d_Pres_mb.avg_now()), 0);
//	}
//	if (var == "INSOLATION_PERCENT") {
//		return String(_d_Insol.avg_now(), 0);
//	}
//	if (var == "REL_HUMIDITY") {
//		return String(_d_RH.avg_now(), 0);
//	}
//	if (var == "UV_A") {
//		if (_isGood_UV) {
//			return String(_d_UVA.avg_now(), 0);
//		}
//		else {
//			return String("na");
//		}
//	}
//	if (var == "UV_B") {
//		if (_isGood_UV) {
//			return String(_d_UVB.avg_now(), 0);
//		}
//		else {
//			return String("na");
//		}
//	}
//	if (var == "UV_INDEX") {
//		//if (_isGood_UV) {
//		return String(_d_UVIndex.avg_now(), 1);
//		//}
//		//else {
//		//	return String("na");
//		//}
//	}
//	if (var == "IR_T_SKY") {
//		return String(_d_IRSky_C.avg_now(), 0);
//	}
//
//	///  DAILY MAXIMA  ///////////////////
//
//	if (var == "TEMPERATURE_F_HI") {
//		return String(_d_Temp_F.max_today().value, 0);
//	}
//	if (var == "WIND_SPEED_HI") {
//		return String(_windSpeed.max_today().value, 0);
//	}
//	if (var == "WIND_GUST_HI") {
//		return String(_windGust.max_today().value, 0);
//	}
//	if (var == "WIND_ANGLE_HI") {
//		return "??";		// avg since last cleared (<= 10 min)
//	}
//	if (var == "PRESSURE_MB_SL_HI") {
//		return String(_d_Pres_seaLvl_mb.max_today().value, 0);
//	}
//	if (var == "INSOLATION_PERCENT_HI") {
//		return String(_d_Insol.max_today().value, 0);
//	}
//	if (var == "REL_HUMIDITY_HI") {
//		return String(_d_RH.max_today().value, 0);
//	}
//	if (var == "UV_A_HI") {
//		if (_isGood_UV) {
//			return String(_d_UVA.max_today().value, 0);
//		}
//		else {
//			return String("na");
//		}
//	}
//	if (var == "UV_B_HI") {
//		if (_isGood_UV) {
//			return String(_d_UVB.max_today().value, 0);
//		}
//		else {
//			return String("na");
//		}
//	}
//	if (var == "UV_INDEX_HI") {
//		if (_isGood_UV) {
//			return String(_d_UVIndex.max_today().value, 1);
//		}
//		else {
//			return String("na");
//		}
//	}
//	if (var == "IR_T_SKY_HI") {
//		return String(_d_IRSky_C.max_today().value, 0);
//	}
//
//	///  DAILY MINIMA  ///////////////////
//
//	if (var == "TEMPERATURE_F_LO") {
//		return String(_d_Temp_F.min_today().value, 0);
//	}
//	if (var == "WIND_SPEED_LO") {
//		return String(_windSpeed.min_today().value, 0);	// 10-min avg
//	}
//	if (var == "WIND_GUST_LO") {
//		return String(_windGust.min_today().value, 0);
//	}
//	if (var == "PRESSURE_MB_SL_LO") {
//		return String(_d_Pres_seaLvl_mb.min_today().value, 0);
//	}
//	if (var == "REL_HUMIDITY_LO") {
//		return String(_d_RH.min_today().value, 0);
//	}
//	if (var == "IR_T_SKY_LO") {
//		return String(_d_IRSky_C.min_today().value, 0);
//	}
//
//	///  GPS DATA   ////////////////////////
//
//	if (var == "GPS_IS_SYNCED") {
//		if (_gps.isSynced())
//			return String("Synced");
//		else
//			return String("Not Synced");
//	}
//	if (var == "GPS_LOCATIONS_UPDATE_COUNTER") {
//		return String(_gps.cyclesCount());
//	}
//	if (var == "GPS_LATITUDE") {
//		return String(_gps.data.latitude(), 6);
//	}
//	if (var == "GPS_LONGITUDE") {
//		return String(_gps.data.longitude(), 6);
//	}
//	if (var == "GPS_ALTITUDE") {
//		return String(_gps.data.altitude());
//	}
//	if (var == "GPS_DATE") {
//		return String(_gps.date_UTC_GPS());
//	}
//	if (var == "GPS_TIME") {
//		return String(_gps.time_UTC_GPS());
//	}
//	/*if (var == "GPS_UTC_OFFSET") {
//		return String(UTC_OFFSET_HOURS);
//	}*/
//	if (var == "GPS_DAYLIGHT_TIME_USED") {
//		return Utilities::bool_Yes_No(IS_DAYLIGHT_TIME);
//	}
//	if (var == "GPS_HDOP") {
//		return String(_gps.data.HDOP() / 100.);
//	}
//	if (var == "GPS_SATELLITES") {
//		return String(_gps.data.satellites());
//	}
//	if (var == "ELAPSED_TIME_STRING") {
//		return   String(_gps.data.timeToSync_sec(), 2);
//	}
//	if (var == "FAN_RPM") {
//		return String(_d_fanRPM.valueLastAdded());
//	}
//
//	/// CHART FIELDS  //////////////////////////////////////////////
//
//
//
//	if (var == "UTC_CURRENT_OFFSET_HOURS") {
//		if (IS_DAYLIGHT_TIME) {
//			return String(UTC_OFFSET_HOURS + 1);
//		}
//		else {
//			return  String(UTC_OFFSET_HOURS);
//		}
//	}
//
//
//	/// Y-AXIS LABEL  //////////////
//
//	if (var == "CHART_Y_AXIS_LABEL") {
//		// Based on chart requested.
//		switch (_chart_request)
//		{
//		case CHART_NONE:
//			return "Chart not specified!";
//		case CHART_INSOLATION:
//			return String(_d_Insol.label() + ", " + _d_Insol.units_html());
//		case CHART_IR_SKY:
//			return String(_d_IRSky_C.label() + ", " + _d_IRSky_C.units_html());
//		case CHART_TEMPERATURE_F:
//			return String(_d_Temp_F.label() + ", " + _d_Temp_F.units_html());
//		case CHART_PRESSURE_SEA_LEVEL:
//			return String(_d_Pres_seaLvl_mb.label() + ", " + _d_Pres_seaLvl_mb.units());
//		case CHART_RELATIVE_HUMIDITY:
//			return String(_d_RH.label() + ", " + _d_RH.units_html());
//		case CHART_UV_INDEX:
//			return String(_d_UVIndex.label() + ", " + _d_UVIndex.units());
//		case CHART_WIND_DIRECTION:
//			return String(_windDir.label() + ", " + _windDir.units_html());
//		case CHART_WIND_SPEED:
//			return String(_windSpeed.label() + ", " + _windSpeed.units());
//		case CHART_WIND_GUST:
//			return String("Wind Gusts, " + _windGust.units());
//		default:
//			return "Chart not found";
//		}
//	}
//
//	/// CHART TITLE  //////////////
//
//	if (var == "CHART_TITLE") {
//		// Based on chart requested.
//		switch (_chart_request)
//		{
//		case CHART_NONE:
//			return "Chart not specified!";
//		case CHART_INSOLATION:
//			return String(_d_Insol.label());
//		case CHART_IR_SKY:
//			return String(_d_IRSky_C.label());
//		case CHART_TEMPERATURE_F:
//			return String(_d_Temp_F.label());
//		case CHART_PRESSURE_SEA_LEVEL:
//			return String(_d_Pres_seaLvl_mb.label());
//		case CHART_RELATIVE_HUMIDITY:
//			return String(_d_RH.label());
//		case CHART_UV_INDEX:
//			return String(_d_UVIndex.label());
//		case CHART_WIND_DIRECTION:
//			return String(_windDir.label());
//		case CHART_WIND_SPEED:
//			return String(_windSpeed.label());
//		case CHART_WIND_GUST:
//			return "Wind Gusts";
//		}
//	}
//
//	/// Y-AXIS MIN  //////////////
//
//	if (var == "Y_MIN") {
//		// Based on chart requested.
//		switch (_chart_request)
//		{
//		case CHART_NONE:
//			return "min: -500";
//		case CHART_INSOLATION:
//			return "min: 0";
//		case CHART_IR_SKY:
//			return "min: -50";
//		case CHART_TEMPERATURE_F:
//			return "min: 0";
//		case CHART_PRESSURE_SEA_LEVEL:
//			return "min: 950";
//		case CHART_RELATIVE_HUMIDITY:
//			return "min: 0";
//		case CHART_UV_INDEX:
//			return "min: 0";
//		case CHART_WIND_DIRECTION:
//			return "min: 0";
//		case CHART_WIND_SPEED:
//			return "min: 0";
//		case CHART_WIND_GUST:
//			return "min: 0";
//		default:
//			return "min: -2000";
//		}
//	}
//
//	/// Y-AXIS MAX  //////////////
//
//	if (var == "Y_MAX") {
//		// Based on chart requested.
//		switch (_chart_request)
//		{
//		case CHART_NONE:
//			return ", max: 500";
//		case CHART_INSOLATION:
//			return ", max: 100";
//		case CHART_IR_SKY:
//			return ", max: 50";
//		case CHART_TEMPERATURE_F:
//			return ", max: 100";
//		case CHART_PRESSURE_SEA_LEVEL:
//			return ", max: 1050";
//		case CHART_RELATIVE_HUMIDITY:
//			return ", max: 100";
//		case CHART_UV_INDEX:
//			return ", max: 20";
//		case CHART_WIND_DIRECTION:
//			return ", max: 360";
//		case CHART_WIND_SPEED:
//			return ", max: 50";
//		case CHART_WIND_GUST:
//			return ", max: 50";
//		default:
//			return ", max: 2000";
//		}
//	}
//
//	/// Y-AXIS TICK AMOUNT  //////////////
//
//	if (var == "Y_TICK_AMOUNT") {
//		// Based on chart requested.
//		switch (_chart_request)
//		{
//		case CHART_IR_SKY:
//			return ", tickAmount: 5";
//		case CHART_TEMPERATURE_F:
//			return ", tickAmount: 5";
//		case CHART_PRESSURE_SEA_LEVEL:
//			return ", tickAmount: 3";
//		case CHART_WIND_DIRECTION:
//			return ", tickAmount: 2";
//		case CHART_WIND_SPEED:
//			return ", tickAmount: 6";
//		case CHART_WIND_GUST:
//			return ", tickAmount: 6";
//		default:
//			return "";
//		}
//	}
//
//	/*/// LEGEND DISPLAY  //////////////
//
//	if (var == "IS_LEGEND") {
//		if (_isChart_max_min) {
//			return "true";
//		}
//		else {
//			return "false";
//		}
//	}*/
//
//	return var + String(" not found");
//}