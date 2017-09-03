#ifndef VIDEONORMALIZER_DATABASE_H
#define VIDEONORMALIZER_DATABASE_H

#include <stdio.h>
#include "VInfos.h"

class Database
{
private:
	FILE * sqlFile;
	
	/**************************************************************
	 * Get the date and time part of the filename.
	 **************************************************************
	 *
	 * Input:
	 *      buffer:     A char array where to write the result.
	 *      filename:   The name of the file.
	 * Output:
	 *      A pointer to the result, buffer.
	 * PreCond:
	 * PostCond:
	 */
	const char * getDatetime(char * buffer, const char * filename);
	
public:
	/**************************************************************
	 * Do not use.
	 **************************************************************
	 *
	 * Input:
	 * Output:
	 * PreCond:
	 * PostCond:
	 *      Throws a NotUsedException.
	 */
	Database();
	
	/**************************************************************
	 * Open a SQL file to write into.
	 **************************************************************
	 *
	 * Input:
	 *      filepath: The path to the file to write.
	 * Output:
	 * PreCond:
	 * PostCond:
	 *      Throws a IOException if the file cannot be opened.
	 */
	explicit Database(char * filepath);
	
	/**************************************************************
	 * Close a database.
	 **************************************************************
	 *
	 * Input:
	 * Output:
	 * PreCond:
	 * PostCond:
	 */
	~Database();
	
	/**************************************************************
	 * Register a picture in the database.
	 **************************************************************
	 *
	 * Input:
	 *      database:   The database to write to.
	 *      filename:   The name of the picture, formatted as the date.
	 * Output:
	 * PreCond:
	 * PostCond:
	 */
	void registerPicture(Database * database, char * filename);
	
	/**************************************************************
	 * Register a video in the database.
	 **************************************************************
	 *
	 * Input:
	 *      database:   The database to write to.
	 *      vInfos:     The infos of the video.
	 * Output:
	 * PreCond:
	 * PostCond:
	 */
	void registerVideo(Database * database, VInfos * vInfos);
};

#endif
