#pragma once

#include "ywdb.h"
#include "configuration.h"
#include <stack>

namespace yw {
    namespace extract {
        class OutlineExporter {

            yw::db::YesWorkflowDB& ywdb;
            static yw::config::Configuration defaultSettings;
            size_t blockIndentSize;
            int qualifierIndentSize;
            bool qualifiersOnSameLine;
            std::stack<yw::db::Annotation> blockStack;
            std::stringstream outline;

            void append(const std::string& keyword, const std::string& value);
            void appendOnNewLine(const std::string& keyword, const std::string& value, const size_t extraIndent = 0);
            void appendOnSameLine(const std::string& keyword, const std::string& value);

        public:

            OutlineExporter(
                yw::db::YesWorkflowDB& ywdb,
                const yw::config::Configuration& userConfiguration = yw::config::Configuration{}
            );

            static const yw::config::Configuration& getSoftwareSettings();

            std::string getOutline();
            std::string getOutline(const nullable_row_id& rootAnnotation);
        };
    }
}
