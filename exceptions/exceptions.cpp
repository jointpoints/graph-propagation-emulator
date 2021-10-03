/**
 * @file exceptions.cpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#include "exceptions.hpp"





// BaseFileError

gpe::error::BaseFileError::BaseFileError(std::string const file_name) : file_name(file_name) {}





// FileDoesNotExist

gpe::error::FileDoesNotExist::FileDoesNotExist(std::string const file_name) : BaseFileError(file_name) {}





// WrongFileFormat

gpe::error::WrongFileFormat::WrongFileFormat(std::string const file_name) : BaseFileError(file_name) {}
