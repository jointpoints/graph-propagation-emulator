/**
 * @file exceptions.hpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#ifndef __GPE_EXCEPTIONS_HPP__
#define __GPE_EXCEPTIONS_HPP__





#include <string>





/**
 * @namespace error
 * @brief Namespace containing all possible GPE errors
 *
 * This namespace defines exceptions used within Graph Propagation Emulator to signal
 * any erroneus behaviour.
 */
namespace gpe::error
{





/**
 * @class BaseError
 * @brief Base class for GPE exception
 *
 * Shall never be called, only used as a base for more specific errors.
 */
class BaseError
{
public:
	/**
	 * @brief Default destructor
	 *
	 * Deletes the error object.
	 */
	virtual ~BaseError(void) = default;

	/**
	 * @brief what
	 *
	 * Returns human-readable description of the problem
	 */
	virtual inline std::string const what(void) = 0;
};





/**
 * @class BaseFileError
 * @brief Base class for file errors
 *
 * Shall never be called, only used as a base for more specific errors.
 */
class BaseFileError : public BaseError
{



protected:

	/// A name of the file that caused the error
	std::string const file_name;



public:

	/// @name Constructors & destructors
	/// @{
	
	BaseFileError(void) = delete;

	/**
	 * @brief Constructor
	 *
	 * Creates a file error object.
	 * 
	 * @param file_name A name of the file that caused the error.
	 */
	explicit BaseFileError(std::string const file_name);

	/// @}



};





/**
 * @class FileDoesNotExist
 * @brief "File does not exist" error
 *
 * Shall be raised when any file needed to be opened does not exist.
 */
class FileDoesNotExist : public BaseFileError
{
public:
	explicit FileDoesNotExist(std::string const file_name);
	inline std::string const what(void) {return "File " + this->file_name + " does not exist.";};
};

/**
 * @class WrongFileFormat
 * @brief "Wrong file format" error
 *
 * Shall be raised when opened file does not contain information that it is supposed to
 * contain.
 */
class WrongFileFormat : public BaseFileError
{
public:
	explicit WrongFileFormat(std::string const file_name);
	inline std::string const what(void) {return "File " + this->file_name + " cannot be properly interpreted.";};
};





}; // gpe::error





#endif // __GPE_EXCEPTIONS_HPP__
