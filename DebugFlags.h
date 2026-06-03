// DebugFlags.h

#ifndef _DEBUGFLAGS_h
#define _DEBUGFLAGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/*****************      DEBUGGING FLAGS      ******************/

constexpr bool isDEBUG_BypassGPS = true;				// Bypass gps syncing.
constexpr bool isDEBUG_BypassWifi = false;				// Bypass WiFi connect.	 XXX FAILS!!!
constexpr bool isDEBUG_BypassSDCard = false;			// Bypass SD card.
constexpr bool isDEBUG_ListLittleFS = true;				// List contents of LittleFS.
constexpr bool isDEBUG_BypassWebServer = false;			// Bypass Web Server.
constexpr bool isDEBUG_run_test_in_setup = false;		// Run only test code inserted in Setup.
constexpr bool isDEBUG_run_test_in_loop = false;		// Run test code inserted in Loop.
constexpr bool isDEBUG_addDummyDataLists = false;		// Add dummy data.
constexpr bool isDEBUG_simulateSensorReadings = false;	// Add dummy sensor reading values.
constexpr bool isDEBUG_simulateWindReadings = false;	// Add dummy wind reading values.
constexpr bool isDEBUG_AddDelayInLoop = false;			// Add delay in loop.
constexpr int LOOP_DELAY_DEBUG_ms = 100;				// Debug delay in loop, msec.


#endif
