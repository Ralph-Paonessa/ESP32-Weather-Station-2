// Testing.h

#ifndef _TESTING_h
#define _TESTING_h

#include <Arduino.h>

#include "WindSpeed2.h"
#include "WindDirection.h"

#include <list>
using std::list;

#include "App_settings.h"		// for namespace App_Settings
using namespace App_Settings;

#include "DataPoint_Lists.h"		// namespace DataPoint_Lists
using namespace DataPoint_Lists;

#include "SensorData.h"

#include "FileOperations.h"	
using namespace FileOps;

class Testing {

private:

	float _dummyVal;
	int _countPeriods;

public:

	// Tests WindSpeed2.h wind directionCardinal handling.
	static void windDirection(float angleStart, float angleIncrement, int cycles, float VANE_OFFSET);

	static void testCodeForSetup1(unsigned long runTime_sec, bool isInfiniteLoop);

	static void testCodeForSetup_convert_DelimString_to_ListOfDataPoints(bool isInfiniteLoop);

	static void testCodeForSetup_list_dataPoints_fromString(bool isInfiniteLoop);

	static void testCodeForSetup_getStringList_from_String(bool isInfiniteLoop);

	//static void testCodeForSetup_empty_strings(bool isInfiniteLoop);

	static void infiniteLoop();

	/// <summary>
	/// Test code for contents of LittleFS.
	/// </summary>
	/// <param name="isInfiniteLoop"></param>
	/// <param name="path">Full path to file, such as "/Sensor data/file1.txt".</param>
	void testCodeForSetup_printFileContents(bool isInfiniteLoop, String path);

	float testDummyReading(float seed, float increment1, float increment2, int periods, unsigned long count);

	int dummyRotations(float first, float last, float spike, int spikePeriod, int periods);

	//void test2();

	////void testList();
	void testStructList(long time);

	// Add a list of rotations_VOL to a WindSpeed instance, and return elapsed seconds.
	float addDummyRotations(list<float> srcList, WindSpeed& speedObj, float rawPeriod);

	//void test();

	String readData();
};



#endif


