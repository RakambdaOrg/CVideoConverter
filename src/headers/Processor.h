#ifndef VIDEONORMALIZER_PROCESSOR_H
#define VIDEONORMALIZER_PROCESSOR_H

#include "Database.h"

typedef struct _fileinfo
{
	char name[260];
	bool isDirectory;
} fileinfo;

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
	 */
	Processor(Database * database, const char * folderInProcess, const char * folderInWindows, const char * folderOutWindows, const char * folderOutProcess, const char * folderBatWindows);
	
	/**
	 * Start this processor.
	 */
	int process();
	
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
	const char * folderInProcess;
	const char * folderOutProcess;
	const char * folderInWindows;
	const char * folderOutWindows;
	const char * folderBatWindows;
	
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
	static bool isSystemFile(const char * filename);
	
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
	
	/**
	 * Tell if a file should be skipped.
	 * @param filename The name of the file.
	 * @return True should be skipped, false else.
	 */
	static bool shouldSkip(char * filename);
	
	static bool fileExists(const char * name);
	
	static int getFiles(const char * dirp, fileinfo *** namelist);
	
	static void sortFiles(fileinfo ** namelist, int size);
	
	static int compareFileinfo(const void * a, const void * b);
};

#endif
