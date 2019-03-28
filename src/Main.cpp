#include <cstdlib>
#include <iostream>
#include "headers/Processor.h"

extern "C" {
}

int main(int argc, char ** argv)
{
	if(argc != 4 && argc != 6)
		exit(EXIT_FAILURE);
	
	//Configure folders to set in the batch files
	const char * folderInWindows = argv[1];
	const char * folderOutWindows = argv[2];
	const char * folderBatWindows = argv[3];
	
	//Configure the folders to build the batch files.
	const char * folderInProcess;
	const char * folderOutProcess;
	if(argc != 6)
	{
		folderInProcess = argv[1];
		folderOutProcess = argv[3];
	}
	else
	{
		folderInProcess = argv[4];
		folderOutProcess = argv[5];
	}
	
	char * databasePath = Processor::scat(folderOutProcess, "stats.sql");
	auto * database = new Database(databasePath);
	free(databasePath);
	
	auto * processor = new Processor(database, folderInProcess, folderInWindows, folderOutWindows, folderOutProcess, folderBatWindows);
	int scripts = processor->process();
	std::cout << std::endl << "New scripts created: " << scripts;
	
	delete processor;
	delete database;
	
	return 0;
}
