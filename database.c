#include <stdlib.h>
#include <memory.h>
#include "database.h"
#include "main.h"

Database * databaseOpen(char * filepath)
{
	Database * database = (Database *) malloc(sizeof(Database));
	if(database == NULL)
		return NULL;
	database->sqlFile = fopen(filepath, "w");
	if(database->sqlFile == NULL)
	{
		free(database);
		return NULL;
	}
	
	fprintf(database->sqlFile, "CREATE TABLE IF NOT EXISTS `MR`(\n");
	fprintf(database->sqlFile, "\tID INT PRIMARY KEY NOT NULL AUTO_INCREMENT,\n");
	fprintf(database->sqlFile, "\tRAt DATETIME NOT NULL,\n");
	fprintf(database->sqlFile, "\tType INT NOT NULL,\n");
	fprintf(database->sqlFile, "\tDuration INT,\n");
	fprintf(database->sqlFile, "\tInfos VARCHAR(50)\n");
	fprintf(database->sqlFile, ");\n");
	fprintf(database->sqlFile, "CREATE UNIQUE INDEX MR_ID_uindex ON MR (ID);\n\n");
	
	return database;
}

void databaseClose(Database * database)
{
	fclose(database->sqlFile);
	free(database);
}

void databaseRegisterPicture(Database * database, char * filename)
{
	char buffer[500];
	fprintf(database->sqlFile, "INSERT INTO `MR`(`RAt`, `Type`) VALUES(\"%s\", 0);\n", databaseGetDatetime(buffer, filename));
}

const char * databaseGetDatetime(char * buffer, const char * filename)
{
	char * dot = strrchr(filename, '.');
	unsigned int i = 0;
	while(filename + i != dot && filename[i] != '\0')
	{
		buffer[i] = filename[i];
		i++;
	}
	buffer[i] = '\0';
	return buffer;
}

void databaseRegisterVideo(Database * database, VInfos * vInfos)
{
	char buffer[500];
	fprintf(database->sqlFile, "INSERT INTO `MR`(`RAt`, `Type`, `Duration`, `Infos`) VALUES(\"%s\", 1, %lf, \"%s %lf\");\n", databaseGetDatetime(buffer, vInfos->filename), vInfos->duration, vInfos->codec, vInfos->fps);
}
