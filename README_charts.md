# How Charts are Added to Web Pages

*Charts are produced using publically-available code from Highcharts that is 
free for personal use.* We serve this from a local copy, `highcharts.js`.

## chart.html

  - The chart display page `chart.html` contains an empty chart that will be dynamically populated during a from server request by the function `processor(const String& var)`. Some parameters have placeholder strings that begin and end with `%` ***placeholders*** such as:

```
    %CHART_Y_AXIS_LABEL%
    %CHART_TITLE%
    %Y_MIN%
    %Y_MAX%
    %Y_TICK_AMOUNT%
```
  - Upon html load, javascript function `getChartData(dataRoute)` is called to 
  asynchronously request and display the appropriate chart data. The chart 
  page initially requests 10-min data (dataRoute = "/data_10"). Buttons on the 
  page allow the user to request 1-hr data and daily max/min values as well.
  
  - Depending on the chart data type (temperature, pressure, etc.), the Web Server Route Handler() parses this request and supplies data from the appropriate sensor as a delimited string via 
  `SensorData::data_10_min_string_delim()`.

  - This data string is processed by chart.js, which creates an array of
  (time, value) data points that are fed to the highcharts data series in 
  chart.html, which plots the data.

  - The data string is a series of comma-separated "time,value" pairs 
  delimited by "~". Time is in javascript format as the integer number of 
  millisec from 1/1/1970.

  - The Hourly button in chart.html asynchronously loads the 
  60-min data into the chart: `SensorData::data_60_min_string_delim()`.

  ## chart_min_max.html -- _UNDER DEVELOPMENT_

  - _The Daily button in chart.html should asynchronously load and plot 
  **both** the daily highs and daily lows into chart_min_max.html that can 
  display TWO data series._

  ### Mechanism for getting Daily Max/Min charts.
  **This is now in development.** *One option is to create a separate 
  chart_2.html set up to display two data series, along with chart_2.js to
  parse the delimited data string from a modified* **getChartData("/data_max_min")**.

  ## Server requests for charts

  Available charts are enumerated in `chartRequested`:

``` cpp
enum chartRequested {
	CHART_NONE,
	CHART_WIND_SPEED,
	CHART_WIND_GUST,
	CHART_WIND_DIRECTION,
	CHART_TEMPERATURE_F,
	CHART_TEMPERATURE_BME_F,
	CHART_PRESSURE_SEA_LEVEL,
	CHART_RELATIVE_HUMIDITY,
	CHART_UV_INDEX,
	CHART_INSOLATION,
	CHART_IR_SKY_C,
	CHART_IR_SKY_F
};	
```

A server request for for `chart.html` will set the global variable `_chart_request` to one of the enumerated values:

```
server.on("/chart_T", HTTP_GET, [](AsyncWebServerRequest* request) {
	_chart_request = CHART_TEMPERATURE_F;
	request->send(LittleFS, "/html/chart.html", "text/html", false, processor);
	});
```

Subsequent server requests data needed to display the chart (title, axis min and max, etc.) will return relevant data for the current global `_chart_request`.

The js function `getChartData(dataRoute, elem)` accepts various `dataRoute` url routes that dictate the data values for the chart duration:

```
\data_10
\data_60
\data_max_min
```

Sending a server request with a dataRoute will return the corresponding data points in a delimited string, based on global `_chart_request`:

```
/*****  10-MIN CHARTS  *****/

server.on("/data_10", HTTP_GET,
	[](AsyncWebServerRequest* request) {
			_isChart_max_min = false;

	// Which chart?
	switch (_chart_request)
	{
	case CHART_NONE:
		request->send(200, "text/plain", "");
		break;
	case CHART_INSOLATION:
			request->send(200, "text/plain", d_Insol.dataPoints_10_min_as_String().c_str());
			break;
	case CHART_IR_SKY_C:
		request->send(200, "text/plain", d_IRSky_C.dataPoints_10_min_as_String().c_str());
		break;
	case ...

    default:
		request->send(200, "text/plain", "");
		break;
	}
});
```

The js function `_chart_request` helps format the chart page:

```js
function getChartData(dataRoute, elem) {

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // Remove "active" class from all "nav" divs.
            const nodeList = document.querySelectorAll("div.nav");
            for (i = 0; i < nodeList.length; i++) {
                nodeList[i].classList.remove("active");
            }
            // Add "active" class to calling div.
            elem.classList.add("active");

            // Split into arrary of high and low delimited list strings.
            const dataSeries = this.responseText.split("|");
            // Reset all data in two series
            chart_1.series[0].setData([]);
            chart_1.series[1].setData([]);
            // Parse each list and add it to a series in the chart.
            for (var i_series = 0; i_series < dataSeries.length; i_series++) {
                // Split into array of (time,value) strings.
                const data_points = dataSeries[i_series].split("~");
                console.log(data_points);
                // JS time is in millisec from 1/1/1970.
                // Data is in seconds.
                const MILLISECONDS_PER_SECOND = 1000;
                for (let i_point = 0; i_point < data_points.length; i_point++) {
                    const data_point = data_points[i_point].split(","); // split into time, value
                    //console.log("data_point[" + i_point + "] " + data_point);
                    chart_1.series[i_series].addPoint([parseInt(data_point[0] * MILLISECONDS_PER_SECOND), parseFloat(data_point[1])], true, false, true);
                }
            }
        }
    };
    xhttp.open("GET", dataRoute, true);
    xhttp.send();
}
```