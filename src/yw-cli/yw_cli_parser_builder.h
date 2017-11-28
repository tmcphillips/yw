#pragma once

#include "antlr4-runtime.h"
#include "YW_CLI_Parser.h"
#include "YW_CLI_Lexer.h"
#include "yw_cli_parser_error_listener.h"

namespace yw {
    namespace cli {

        class YW_CLI_ParserBuilder {

        private:
            std::unique_ptr<std::stringstream> text_stream;
            std::unique_ptr<antlr4::ANTLRInputStream> antlr_input_stream;
            std::unique_ptr<YW_CLI_Lexer> yw_lexer;
            std::unique_ptr<antlr4::CommonTokenStream> antlr_token_stream;
            std::shared_ptr<YW_CLI_Parser> yw_cli_parser;
            std::shared_ptr<YWCliParserErrorListener> errorListener;

        public:
            YW_CLI_ParserBuilder(const std::string& text, bool useCustomErrorListener = false);
            std::shared_ptr<YW_CLI_Parser> parse() { return yw_cli_parser; }
        };
    }
}