#pragma once

#include "preparation_exception.h"

#include <memory>
#include <string>

using std::string;

namespace yw {
    namespace sqlite {

        PreparationException::PreparationException(string message, string sql) :
            StatementException(message, sql) {}

        string PreparationException::what() { 
            return "Error preparing statement: " + getMessage(); 
        }
    }
}

