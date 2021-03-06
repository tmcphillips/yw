#include "misplaced_begin_child_exception.h"
#include <sstream>

namespace yw {
    namespace parse {

        MisplacedBeginChildException::MisplacedBeginChildException(
            const std::string& token,
            int column,
            int line,
            const nullable_string& source,
            const std::string& what
        ) : UnexpectedAnnotationException(token, column, line, source, what)
        {
            updateMessage();
            setDetails("The " + std::string(token) + " annotation must be used within a program block and before any nested blocks.");
        }

        void MisplacedBeginChildException::updateMessage() {
            auto source = getSource();
            std::stringstream ss;
            ss << "The '" << token << "' annotation";
            ss << " was unexpected at column " << column;
            ss << " of line " << line;
            if (source.hasValue()) {
                ss << " in source file '" << source.getValue() << "'";
            }
            ss << ".";
            messageBacking = ss.str();
        }
    }
}

