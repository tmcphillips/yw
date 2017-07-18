#pragma once

#include "yw_parse_tests.h"

namespace yw {
    namespace parse {

        class StubYWListener : public YWBaseListener {
        private:
            std::stringstream _log;
        public:
            std::string log() { return _log.str(); }
            void enterScript(YWParser::ScriptContext *context) override { _log << "entered script" << std::endl; }
            void exitScript(YWParser::ScriptContext *context) override { _log << "exited script" << std::endl; }
            void enterBlock(YWParser::BlockContext *context) override { _log << "entered block" << std::endl; }
            void exitBlock(YWParser::BlockContext *context) override { _log << "exited block" << std::endl; }
            void enterBlockAttribute(YWParser::BlockAttributeContext *context) override { _log << "entered block attribute" << std::endl; }
            void exitBlockAttribute(YWParser::BlockAttributeContext *context) override { _log << "exited block attribute" << std::endl; }
            void enterPortAttribute(YWParser::PortAttributeContext *context) override { _log << "entered port attribute" << std::endl; }
            void exitPortAttribute(YWParser::PortAttributeContext *context) override { _log << "exited port attribute" << std::endl; }

            void enterBegin(YWParser::BeginContext *context) override { _log << "entered begin tag" << std::endl; }
            void exitBegin(YWParser::BeginContext *context) override { _log << "exited begin tag" << std::endl; }
            void enterEnd(YWParser::EndContext *context) override { _log << "entered end tag" << std::endl; }
            void exitEnd(YWParser::EndContext *context) override { _log << "exited end tag" << std::endl; }
            void enterDesc(YWParser::DescContext *context) override { _log << "entered desc tag" << std::endl; }
            void exitDesc(YWParser::DescContext *context) override { _log << "exited desc tag" << std::endl; }
            void enterPort(YWParser::PortContext *context) override { _log << "entered port tag" << std::endl; }
            void exitPort(YWParser::PortContext *context) override { _log << "exited port tag" << std::endl; }
            void enterAlias(YWParser::AliasContext *context) override { _log << "entered alias tag" << std::endl; }
            void exitAlias(YWParser::AliasContext *context) override { _log << "exited alias tag" << std::endl; }
            void enterCall(YWParser::CallContext *context) override { _log << "entered call tag" << std::endl; }
            void exitCall(YWParser::CallContext *context) override { _log << "exited call tag" << std::endl; }
            void enterUri(YWParser::UriContext *context) override { _log << "entered uri tag" << std::endl; }
            void exitUri(YWParser::UriContext *context) override { _log << "exited uri tag" << std::endl; }
            void enterFile(YWParser::FileContext *context) override { _log << "entered file tag" << std::endl; }
            void exitFile(YWParser::FileContext *context) override { _log << "exited file tag" << std::endl; }
            void enterResource(YWParser::ResourceContext *context) override { _log << "entered resource tag" << std::endl; }
            void exitResource(YWParser::ResourceContext *context) override { _log << "exited resource tag" << std::endl; }

            void enterInputKeyword(YWParser::InputKeywordContext *context) override { _log << "entered input port keyword" << std::endl; }
            void exitInputKeyword(YWParser::InputKeywordContext *context) override { _log << "exited input port keyword" << std::endl; }
            void enterOutputKeyword(YWParser::OutputKeywordContext *context) override { _log << "entered output port keyword" << std::endl; }
            void exitOutputKeyword(YWParser::OutputKeywordContext *context) override { _log << "exited output port keyword" << std::endl; }

            void enterBlockName(YWParser::BlockNameContext *context) override { _log << "entered block name" << std::endl; }
            void exitBlockName(YWParser::BlockNameContext *context) override { _log << "exited block name" << std::endl; }
            void enterPortName(YWParser::PortNameContext *context) override { _log << "entered port name" << std::endl; }
            void exitPortName(YWParser::PortNameContext *context) override { _log << "exited port name" << std::endl; }
            void enterDataName(YWParser::DataNameContext *context) override { _log << "entered data name" << std::endl; }
            void exitDataName(YWParser::DataNameContext *context) override { _log << "exited data name" << std::endl; }
            void enterDescription(YWParser::DescriptionContext *context) override { _log << "entered description" << std::endl; }
            void exitDescription(YWParser::DescriptionContext *context) override { _log << "exited description" << std::endl; }
        };
    }
}
