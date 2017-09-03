#include <stdlib.h>
#include <memory.h>
#include "Database.h"
#include "IOException.h"
#include "NotUsedException.h"

Database::Database()
{
	throw NotUsedException();
}

Database::Database(char * filepath)
{
	sqlFile = fopen(filepath, "w");
	if(sqlFile == nullptr)
	{
		free(sqlFile);
		throw IOException();
	}
	
	fprintf(sqlFile, "CREATE TABLE IF NOT EXISTS `MR`(\n");
	fprintf(sqlFile, "\tID INT PRIMARY KEY NOT NULL AUTO_INCREMENT,\n");
	fprintf(sqlFile, "\tRAt DATETIME NOT NULL,\n");
	fprintf(sqlFile, "\tType INT NOT NULL,\n");
	fprintf(sqlFile, "\tDuration INT,\n");
	fprintf(sqlFile, "\tInfos VARCHAR(50)\n");
	fprintf(sqlFile, ");\n");
	fprintf(sqlFile, "CREATE UNIQUE INDEX MR_ID_uindex ON MR (ID);\n\n");
}

Database::~Database()
{
	fclose(sqlFile);
}

void Database::registerPicture(Database * database, char * filename)
{
	char buffer[500];
	fprintf(database->sqlFile, "INSERT INTO `MR`(`RAt`, `Type`) VALUES(\"%s\", 0);\n", getDatetime(buffer, filename));
}

const char * Database::getDatetime(char * buffer, const char * filename)
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

void Database::registerVideo(Database * database, VInfos * vInfos)
{
	char buffer[500];
	fprintf(database->sqlFile, "INSERT INTO `MR`(`RAt`, `Type`, `Duration`, `Infos`) VALUES(\"%s\", 1, %lf, \"%s %lf\");\n", this->getDatetime(buffer, vInfos->filename), vInfos->duration, vInfos->codec, vInfos->fps);
}
