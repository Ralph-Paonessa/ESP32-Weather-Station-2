// Adapted from SD_Test.ino in the Arduino/ESP32 example sketches.
// 
// 

#include "FileOperations.h"
#include "Utilities.h"
#include <freertos/task.h>

/// <summary>
/// Lists contents of directory.
/// </summary>
/// <param name="fs">File system.</param>
/// <param name="dirname">Path of the directory.</param>
/// <param name="levels">Number of levels to list.</param>
void FileOps::dirList(fs::FS& fs, const char* dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\n", dirname);

	File root = fs.open(dirname);
	if (!root) {
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println("Not a directory");
		return;
	}
	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) {
				dirList(fs, file.path(), levels - 1);
			}
		}
		else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

void FileOps::dirCreate(fs::FS& fs, const char* path) {
	Serial.printf("Creating Dir: %s\n", path);
	if (fs.mkdir(path)) {
		Serial.println("Dir created.");
	}
	else {
		Serial.println("mkdir failed.");
	}
}

void FileOps::dirRemove(fs::FS& fs, const char* path) {
	Serial.printf("Removing Dir: %s\n", path);
	if (fs.rmdir(path)) {
		Serial.println("Dir removed.");
	}
	else {
		Serial.println("dirRemove failed.");
	}
}

/// <summary>
/// Returns the contents of a file.
/// </summary>
/// <param name="fs">File system to use.</param>
/// <param name="path">Target file path, such as "/readings/data.txt".
/// </param>
/// <returns>String containing file contents.</returns>
String FileOps::fileRead(fs::FS& fs, const char* path) {

	if (!fs.exists(path)) {
		Serial.printf("ERROR: file not found: %s\n", path);
		return "";
	}

	File file = fs.open(path, FILE_READ);
	
	if (!file) {
		Serial.print("ERROR: fileRead failed to open file for reading: ");
		Serial.println(path);
		return "";
	}
	char stringC[DATA_FILE_BUFFER_SIZE] = {  };	// C-string array to hold values from stream.
	int i = 0;
	while (file.available()) {
		stringC[i] += file.read();	// Add each character to C-string.
		i++;
	}
	file.close();
	return String(stringC);
}


void FileOps::fileWrite(fs::FS& fs, const char* path, const char* msg) {

	File file = fs.open(path, FILE_WRITE);
	if (!file) {
		Serial.print("ERROR: FileOps::fileWrite Failed to open file for writing: ");
		Serial.println(path);
		return;
	}
	if (file.print(msg)) {
		serial_println_DEBUG("FileOps::fileWrite to file ", path);
		serial_println_DEBUG("write msg = ", msg);
	}
	else {
		Serial.print("ERROR: FileOps::fileWrite Write failed: ");
		Serial.println(path);
	}
	file.close();
}

void FileOps::fileAppend(fs::FS& fs, const char* path, const char* msg) {
	File file = fs.open(path, FILE_APPEND);
	if (!file) {
		Serial.print("ERROR: FileOps::fileAppend failed to open file ");
		Serial.println(path);
		return;		// file did not open!
	}
	// File opened. Append msg.
	if (file.print(msg)) {
		/*serial_println_DEBUG("FileOps::fileAppend to file ", path);
		serial_println_DEBUG("append msg = ", msg); */
	}
	else {
		Serial.print("ERROR: fileAppend failed to ");
		Serial.println(path);
	}
	file.close();
}

void FileOps::fileRename(fs::FS& fs, const char* path1, const char* path2) {
	Serial.printf("Renaming file %s to %s.\n", path1, path2);
	if (fs.rename(path1, path2)) {
		Serial.println("File renamed.");
	}
	else {
		Serial.print("ERROR: Rename failed: ");
		Serial.print(path1); Serial.print(" to "); Serial.println(path2);
	}
}

void FileOps::fileDelete(fs::FS& fs, const char* path) {
	Serial.printf("Deleting file: %s\n", path);
	if (fs.remove(path)) {
		Serial.println("File deleted.");
	}
	else {
		Serial.print("ERROR: Delete failed.");
		Serial.println(path);
	}
}

/// <summary>
/// Creates a file if it doesn't exist. Returns true on 
/// success or if the file already exists; otherwise
/// returns false. XXX IS THIS NEEDED?!
/// </summary>
/// <param name="fs">File system to use.</param>
/// <param name="path">Full path including the filename and extension.</param>
/// <returns>True on success</returns>
bool FileOps::fileCreateOrExists(fs::FS& fs, const String& path) {
	// If the file doesn't exist, create it.
	if (fs.exists(path)) {
		Serial.printf("FileOps::fileCreateOrExists: %s exists.\n", path.c_str());
		return true;
	}
	// File not found, so create new file.
	Serial.printf("FileOps::fileCreateOrExists: %s does not exist.\n", path.c_str());
	File file = fs.open(path, FILE_WRITE);
	if (!file) {
		Serial.printf("FileOps::fileCreateOrExists: %s could not be created/opened.\n", path.c_str());
		return false;
	}
	else {
		Serial.printf("FileOps::fileCreateOrExists: %s was created/opened and will be closed.\n", path.c_str());
		file.close();
		return true;
	}
}

//// Preliminary code for offloading LittleFS writes to 2nd core
//
//// Structure to hold the combined calculations
//struct WindStruct_XXX {
//	float meanSpeed;
//	float meanDirection;
//	float maxGust;
//};
//
//
//// Create a queue to pass finished 10-min reports to the logging core
//QueueHandle_t logQueue;
//
//void setupQueue() {
//	logQueue = xQueueCreate(10, sizeof(WindStruct_XXX));
//
//	// Create a background logging task pinned to Core 0
//	xTaskCreatePinnedToCore(
//		loggingTask,    // Function name
//		"LogTask",      // Task name
//		4096,           // Stack size
//		NULL,           // Parameter
//		1,              // Priority
//		NULL,           // Task handle
//		0               // Pin to Core 0 (Main loop runs on Core 1)
//	);
//}
//
//void loggingTask(void* parameter) {
//	WindStruct_XXX dataToLog;
//	for (;;) {
//		// This halts efficiently until Core 1 pushes a new report into the queue
//		if (xQueueReceive(logQueue, &dataToLog, portMAX_DELAY)) {
//			File file = LittleFS.open("/wind_log.csv", FILE_APPEND);
//			if (file) {
//				file.printf("%f,%f,%f\n", dataToLog.meanSpeed, dataToLog.meanDirection, dataToLog.maxGust);
//				file.close(); // The 350ms delay happens here, safely isolated on Core 0
//			}
//		}
//	}
//}
