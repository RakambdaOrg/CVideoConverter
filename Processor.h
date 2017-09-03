#ifndef VIDEONORMALIZER_PROCESSOR_H
#define VIDEONORMALIZER_PROCESSOR_H

#include "Database.h"

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
	 */
	Processor(Database * database, char * folderInProcess, char * folderInWindows, char * folderOutWindows, char * folderOutProcess);

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
	static char * convertTime(char * out, int time);
	
	Database * database;
	char * folderInProcess;
	char * folderOutProcess;
	char * folderInWindows;
	char * folderOutWindows;
	
	/**
	 * Get the informations about a file.
	 * @param filename The path to the file.
	 * @param name The name of the file.
	 * @return The file's infos.
	 */
	static VInfos * getVInfos(char * filename, const char * name);
	
	/**
	 * Tell if a file is a system one based on its name and should be skipped.
	 * @param filename The name of the file.
	 * @return True if a system's file, false else.
	 */
	static bool isSystemFile(char * filename);
	
	/**
	 * Tell if a file is a picture based on its name.
	 * @param filename The name of the file.
	 * @return True if a picture, false else.
	 */
	static bool isPictureFile(char * filename);
	
	/**
	 * Transform a file's name with the mp4 extension.
	 * @param filename The name of the file.
	 * @return The new name. The user is responsible of freeing it.
	 */
	static char * asMP4(const char * filename);
};

#endif
