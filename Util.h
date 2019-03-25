

#ifndef HW4_JAMIES_SOLUTION_UTIL_H
#define HW4_JAMIES_SOLUTION_UTIL_H

#include <string>

// function to make a string lowercase
inline void makeLowercase(std::string & toConvert)
{
	for(size_t index = 0; index < toConvert.length(); ++index)
	{
		toConvert[index] = (char)std::tolower(toConvert[index]);
	}
}

inline void makeUppercase(std::string & toConvert)
{
	for(size_t index = 0; index < toConvert.length(); ++index)
	{
		toConvert[index] = (char)std::toupper(toConvert[index]);
	}
}


#endif 
