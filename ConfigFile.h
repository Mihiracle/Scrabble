//
// Class for reading the Scrabble configuration file
//

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "Exceptions.h"

class ConfigFile
{
public:
	size_t handSize;
	std::string tilesetFile;
	std::string dictionaryFile;
	std::string boardFile;
	uint32_t randomSeed;

	// construct with path to file on disk.
	// Throws a FileException("MISSINGCONFIG") if the file could not be found.
	ConfigFile(std::string const & configPath);

};

#endif //CONFIGFILE_H
