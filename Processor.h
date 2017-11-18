#ifndef VIDEONORMALIZER_PROCESSOR_H
#define VIDEONORMALIZER_PROCESSOR_H

#include <string>
#include <vector>
#include "Database.h"

using namespace std;

class Processor
{
public:
	/**************************************************************
	 * Do not use.
	 **************************************************************
	 *
	 * Input:
	 * Output:
	 * PreCond:
	 * PostCond:
	 *      Throw a NotUsedException.
	 */
	Processor();
	
	/**
	 *
	 * @param database The database where to save processed infos.
	 * @param folderInProcess The folder where to scan for files.
	 * @param folderInWindows The folder the batch file will take the inputs.
	 * @param folderOutWindows The folder the batch file will put the outputs.
	 * @param folderOutProcess The folder where to save the batch files.
	 * @param folderBatWindows The folder where the BAT files are on windows.
	 * @param folderBannedWindows
	 */
	Processor(Database * database, string &folderInProcess, string &folderInWindows, string &folderOutWindows, string &folderOutProcess, string &folderBatWindows, vector<string> &folderBannedWindows);
	
	/**
	 * Start this processor.
	 */
	void process();
	
	/**
	 * Concatenate 2 strings into a new one.
	 * @param s1 The first part of the string.
	 * @param s2 The second part of the string.
	 * @return The concatenated string. The user have the responsability to free it.
	 */
	static char * scat(const char * s1, const char * s2);

private:
	
	/**
	 * Convert a duration into a char
	 * @param out The char where to write the infos.
	 * @param time The duration to convert.
	 * @return The parameter out with the data written.
	 */
	static string convertTime(int time);
	
	Database * database;
	string folderInProcess;
	string folderOutProcess;
	string folderInWindows;
	string folderOutWindows;
	string folderBatWindows;
	vector<string> folderBannedWindows;
	
	/**
	 * Get the informations about a file.
	 * @param filename The path to the file.
	 * @param name The name of the file.
	 * @return The file's infos.
	 */
	VInfos * getVInfos(string &filename, const string &name);
	
	/**
	 * Tell if a file is a system one based on its name and should be skipped.
	 * @param filename The name of the file.
	 * @return True if a system's file, false else.
	 */
	bool isSystemFile(string filename);
	
	/**
	 * Tell if a file is a picture based on its name.
	 * @param filename The name of the file.
	 * @return True if a picture, false else.
	 */
	bool isPictureFile(string filename);
	
	/**
	 * Tell if a file should be skipped.
	 * @param filename The name of the file.
	 * @return True should be skipped, false else.
	 */
	bool shouldSkip(string filename);
	
	bool fileExists(const string &name);
	
	bool ends_with(std::string const &value, std::string const &ending);
};

#endif
