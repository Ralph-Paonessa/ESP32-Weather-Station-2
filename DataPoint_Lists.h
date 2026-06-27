// DataPoint_Lists.h

#ifndef _DATAPOINT_LISTS_h
#define _DATAPOINT_LISTS_h

#include <Arduino.h>

#include "dataPoint.h"
#include <string>

#include <list>
using std::list;

/// <summary>
/// Exposes methods that operate on lists.
/// </summary>
namespace DataPoint_Lists {

	/// <summary>
	/// Adds DataPoint to list and limits list size. (If adding 
	/// creates too many elements, the first element is removed.)
	/// </summary>
	/// <param name="targetList">List of dataPoints to add to.</param>
	/// <param name="dp">DataPoint to add.</param>
	/// <param name="numElements">Maximum allowed elements in list.</param>
	void addDataPoint_to_List(list<DataPoint>& targetList, DataPoint val, int numElements);

	/// <summary>
	/// Adds values to list and limits list size. (If adding 
	/// creates too many elements, the first element is removed.)
	/// </summary>
	/// <param name="targetList">List of values to add to.</param>
	/// <param name="val">Value to add.</param>
	/// <param name="numElements">Maximum allowed elements in list.</param>
	void addDataPoint_to_List(list<float>& targetList, float val, int numElements);

	/// <summary>
	/// Returns the average of the last values of members 
	/// of a DataPoint list.
	/// </summary>
	/// <param name="targetList">
	/// The list of dataPoints to average.</param>
	/// <param name="numToAverage">
	/// The number of elements at the end of the list to average.
	/// </param>
	/// <returns>Average value.</returns>
	float listAverage(list<DataPoint>& targetList, int numElements);

	/// <summary>
	/// Returns the average of the last values of members 
	/// of a list of values.
	/// </summary>
	/// <param name="targetList">
	/// The list of values to average.</param>
	/// <param name="numToAverage">
	/// The number of elements at the end of the list to average.
	/// </param>
	/// <returns>Average value.</returns>
	float listAverage(list<float>& targetList, int numElements);

	/// <summary>
	/// Returns the largest value of a list of dataPoints 
	/// from the last numElements.
	/// </summary>
	/// <param name="targetList">List of DataPoint to check.</param>
	/// <param name="numElements">Number of elements to check, starting from end.</param>
	/// <returns>Largest value of a list</returns>
	float listMaximum(list<DataPoint>& targetList, int numElements);

	/// <summary>
	/// Converts a list of data points to a string of "time, value" 
	/// pairs, each delimited by "," separate points delimited by 
	/// "~". Such as "t1,v1~t2,v2~t3,v3".
	/// </summary>
	/// <param name="targetList">List of DataPoint.</param>
	/// <returns>Delimited string of multiple (time, value) data points.</returns>
	String getString_from_List(list<DataPoint>& targetList);

	/// <summary>
	/// Converts a list of dataPoints to a string of 
	/// comma-separated "time,value" pairs delimited by "~". 
	/// Such as "t1,v1~t2,~t3,v3".
	/// </summary>
	/// <param name="targetList">List of DataPoint.</param>
	/// <param name="isConvertZeroToEmpty">
	/// Set true to convert zero value to empty string.</param>
	/// <param name="decimalPlaces">Decimal places to display.</param>
	/// <returns>
	/// Comma-separated "time,value" pairs delimited by "~"</returns>
	String getString_from_List(
		list<DataPoint>& targetList,
		bool isConvertZeroToEmpty,
		unsigned int decimalPlaces);

	/// <summary>
	/// Converts two lists of dataPoints to strings of 
	/// comma-separated "time,value" pairs delimited by "~".
	/// Then combines both lists, delimited by "|". Such as 
	/// "t1,v1~t2,v2~t3,v3|t1,v4~t2,v5~t3,v6"
	/// </summary>
	/// <param name="targetList_hi">First list of DataPoint.</param>
	/// <param name="targetList_lo">Second list of DataPoint.</param>
	/// <param name="isConvertZeroToEmpty">
	/// Set true to convert zero values to empty strings.
	/// </param>
	/// <param name="decimalPlaces">
	/// Decimal places to display.</param>
	/// <returns>Two String lists, respectively delimited by "|".</returns>
	String getString_from_List(
		list<DataPoint>& targetList_hi,
		list<DataPoint>& targetList_lo,
		bool isConvertZeroToEmpty,
		unsigned int decimalPlaces);

	/// <summary>
	/// Splits a delimited string into a list of Arduino String.
	/// </summary>
	/// <param name="str">String to split.</param>
	/// <param name="delim">Delimiter char, such as ','.</param>
	/// <returns>List of Strings after splitting.</returns>
	list<String> getStringList_from_String(const String& str, const char delimiter);

	/// <summary>
	/// Returns a list of dataPoints retrieved from a delimited 
	/// string of comma-separated "time,value" pairs.
	/// </summary>
	/// <param name="str">Delimited string of dataPoints.</param>
	/// <param name="maxSize">Maximum size of list.</param>
	/// <returns>
	/// List of "time,value" dataPoints retrieved from a delimited string.
	/// </returns>
	/// <remarks>
	/// Delimited string must separate points by "~" and time, val by ",".
	/// </remarks>
	list<DataPoint> getDataPoints_from_String(String& str, int maxSize);

	/// <summary>
	/// Prints the elements of a list of C++ std::string.
	/// </summary>
	/// <param name="targetList">The list to print.</param>
	void print_List(list<std::string> targetList);

	/// <summary>
	/// Prints the elements of a list of Arduino String.
	/// </summary>
	/// <param name="targetList">The list to print.</param>
	void print_List(list<String> targetList);

	/// <summary>
	/// Prints the elements of a list of DataPoints.
	/// </summary>
	/// <param name="targetList">The list to print.</param>
	/// <param name="isConvertTime_to_String">Set true to output numeric (Unix) time as string.</param>
	void print_List(list<DataPoint> targetList, bool isConvertTime_to_String = false);

};

#endif
