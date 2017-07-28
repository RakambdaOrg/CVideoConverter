#ifndef VIDEONORMALIZER_DATABASE_H
#define VIDEONORMALIZER_DATABASE_H

#include <stdio.h>
#include "main.h"

typedef struct {
	FILE * sqlFile;
} Database;

Database * databaseOpen(char * filepath);
void databaseClose(Database * database);
void databaseRegisterPicture(Database * database, char * filename);
void databaseRegisterVideo(Database * database, VInfos * vInfos);
const char * databaseGetDatetime(char * buffer, const char * filename);

#endif
