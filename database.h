#ifndef VIDEONORMALIZER_DATABASE_H
#define VIDEONORMALIZER_DATABASE_H

#include <stdio.h>
#include "VInfos.h"

typedef struct {
	FILE * sqlFile;
} Database;

/**************************************************************
 * Open a SQL file to write into.
 **************************************************************
 *
 * Input:
 *      filepath: The path to the file to write.
 * Output:
 *      A Database struct representing this file. NULL if the file couldn't be open.
 * PreCond:
 * PostCond:
 */
Database * databaseOpen(char * filepath);

/**************************************************************
 * Close a database.
 **************************************************************
 *
 * Input:
 *      database: The database to close.
 * Output:
 * PreCond:
 * PostCond:
 *      The variable pointed will be freed.
 */
void databaseClose(Database * database);

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
void databaseRegisterPicture(Database * database, char * filename);

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
void databaseRegisterVideo(Database * database, VInfos * vInfos);

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
const char * databaseGetDatetime(char * buffer, const char * filename);

#endif
