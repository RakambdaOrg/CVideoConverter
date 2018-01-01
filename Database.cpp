#include <memory.h>
#include <cstdio>
#include <string>
#include <iostream>
#include "Database.h"
#include "IOException.h"
#include "NotUsedException.h"

#define ENABLED_LOG 0

Database::Database()
{
	throw NotUsedException();
}

void Database::setUseless(char * name)
{
	std::cout << "U";
	//std::cout << "\t" << "Setting useless " << name;
	int data = 0;
	char * zErrMsg = nullptr;
	sqlite3_exec(sqllite, (std::string("INSERT INTO Done(FilePath) VALUES('") + std::string(name) + std::string("');")).c_str(), callback, &data, &zErrMsg);
}

Database::Database(char * filepath)
{
#if ENABLED_LOG
	sqlFile = fopen(filepath, "w");
	if(sqlFile == nullptr)
	{
		throw IOException();
	}
#endif
	
	if(sqlite3_open("db.db", &sqllite))
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sqllite));
		fclose(sqlFile);
		throw IOException();
	}
	
	char * zErrMsg = nullptr;
	int data = 0;
	sqlite3_exec(sqllite, "PRAGMA synchronous=ON", nullptr, nullptr, &zErrMsg);
	sqlite3_exec(sqllite, "CREATE TABLE IF NOT EXISTS Done(FilePath VARCHAR(2048), PRIMARY KEY(FilePath));", callback, &data, &zErrMsg);

#if ENABLED_LOG
	fprintf(sqlFile, "CREATE TABLE IF NOT EXISTS `MR`(\n");
	fprintf(sqlFile, "\tID INT PRIMARY KEY NOT NULL AUTO_INCREMENT,\n");
	fprintf(sqlFile, "\tRAt DATETIME NOT NULL,\n");
	fprintf(sqlFile, "\tType INT NOT NULL,\n");
	fprintf(sqlFile, "\tDuration INT,\n");
	fprintf(sqlFile, "\tInfos VARCHAR(50)\n");
	fprintf(sqlFile, ");\n");
	fprintf(sqlFile, "CREATE UNIQUE INDEX MR_ID_uindex ON MR (ID);\n\n");
#endif
}

Database::~Database()
{
#if ENABLED_LOG
	fclose(sqlFile);
#endif
	sqlite3_close(sqllite);
}

void Database::registerPicture(Database * database, char * filename)
{
#if ENABLED_LOG
	char buffer[500];
	fprintf(database->sqlFile, "INSERT INTO `MR`(`RAt`, `Type`) VALUES(\"%s\", 0);\n", getDatetime(buffer, filename));
#endif
}

const char * Database::getDatetime(char * buffer, const char * filename)
{
	const char * dot = strrchr(filename, '.');
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
#if ENABLED_LOG
	char buffer[500];
	fprintf(database->sqlFile, "INSERT INTO `MR`(`RAt`, `Type`, `Duration`, `Infos`) VALUES(\"%s\", 1, %lf, \"%s %lf\");\n", this->getDatetime(buffer, vInfos->filename), vInfos->duration, vInfos->codec, vInfos->fps);
#endif
}

bool Database::isUseless(char * string, bool * result)
{
	char * zErrMsg = nullptr;
	int nrecs = 0;
	std::string tmp = std::string("SELECT * FROM Done WHERE FilePath='") + std::string(string) + std::string("';");
	*result = sqlite3_exec(sqllite, tmp.c_str(), select_callback, &nrecs, &zErrMsg) == SQLITE_OK && nrecs > 0;
	return *result;
}
