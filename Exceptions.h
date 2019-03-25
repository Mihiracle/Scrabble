
/* This file contains custom-defined exceptions thrown to signal errors
   or problems in various parts of the code.
   All of them have a constructor with a string, and the string can
   be retrieved using the what() function.
   The specific strings that are used to signal particular errors are
   discussed where they are thrown.
*/

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <stdexcept>
#include <string>

/* This exception is thrown when there are problems opening data files,
   or when data files have format errors. */

class FileException : public std::runtime_error {
public:
	FileException (std::string const & message): std::runtime_error(message) { }
	virtual ~FileException () throw () {}
};

/* This exception is thrown when the player tried to make an invalid move. */

class MoveException : public std::runtime_error {
public:
	MoveException (std::string const & message): std::runtime_error(message) { }
	virtual ~MoveException () throw () {}
};


#endif /* EXCEPTIONS_H_ */
