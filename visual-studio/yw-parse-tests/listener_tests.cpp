#include "stdafx.h"
#include "CppUnitTest.h"

#include "YWLexer.h"
#include "YWListener.h"
#include "YWParser.h"
#include "YWBaseListener.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using std::stringstream;
using std::endl;

namespace yw_parse_tests
{
	class YWListenerForTests : public YWBaseListener {
	private:
		stringstream _log;
	public:

		inline std::string log() { return _log.str(); }

		void enterScript(YWParser::ScriptContext *context) override { _log << "entered script" << endl;  }
		void exitScript(YWParser::ScriptContext *context) override { _log << "exited script" << endl; }
		void enterBlock(YWParser::BlockContext *context) override { _log << "entered block" << endl; }
		void exitBlock(YWParser::BlockContext *context) override { _log << "exited block" << endl; }
		void enterBlockName(YWParser::BlockNameContext *context) override { _log << "entered block name" << endl; }
		void exitBlockName(YWParser::BlockNameContext *context) override { _log << "exited block name" << endl; }
		void enterBegin(YWParser::BeginContext *context) override { _log << "entered begin" << endl; }
		void exitBegin(YWParser::BeginContext *context) override { _log << "exited begin" << endl; }
		void enterBeginKeyword(YWParser::BeginKeywordContext *context) override { _log << "entered begin keyword" << endl; }
		void exitBeginKeyword(YWParser::BeginKeywordContext *context) override { _log << "exited begin keyword" << endl; }
		void enterEnd(YWParser::EndContext *context) override { _log << "entered end" << endl; }
		void exitEnd(YWParser::EndContext *context) override { _log << "exited end" << endl; }
		void enterEndKeyword(YWParser::EndKeywordContext *context) override { _log << "entered end keyword" << endl; }
		void exitEndKeyword(YWParser::EndKeywordContext *context) override { _log << "exited end keyword" << endl; }
		void enterBlockAttribute(YWParser::BlockAttributeContext *context) override { _log << "entered block qualifier" << endl; }
		void exitBlockAttribute(YWParser::BlockAttributeContext *context) override { _log << "exited block qualifier" << endl; }
		void enterPort(YWParser::PortContext *context) override { _log << "entered port" << endl; }
		void exitPort(YWParser::PortContext *context) override { _log << "exited port" << endl; }
		void enterInputPort(YWParser::InputPortContext *context) override { _log << "entered input port" << endl; }
		void exitInputPort(YWParser::InputPortContext *context) override { _log << "exited input port" << endl; }
		void enterOutputPort(YWParser::OutputPortContext *context) override { _log << "entered output port" << endl; }
		void exitOutputPort(YWParser::OutputPortContext *context) override { _log << "exited output port" << endl; }
		void enterIn(YWParser::InContext *context) override { _log << "entered in" << endl; }
		void exitIn(YWParser::InContext *context) override { _log << "exited in" << endl; }
		void enterOut(YWParser::OutContext *context) override { _log << "entered out" << endl; }
		void exitOut(YWParser::OutContext *context) override { _log << "exited out" << endl; }
	};

	TEST_CLASS(ListenerTests)
	{
	private:
		
		YWListenerForTests listener;

		void parse(const char * text) {
			stringstream textStream(text);
			antlr4::ANTLRInputStream input(textStream);
			YWLexer lexer(&input);
			antlr4::CommonTokenStream tokens(&lexer);
			YWParser parser(&tokens);
			antlr4::tree::ParseTree *tree = parser.script();
			antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
		}

	public:

		TEST_METHOD(TestListenerEventSequence_Begin_End)
		{
			parse("@begin b @end b");

			Assert::AreEqual(std::string(
				"entered script"		"\n"
				"entered block"			"\n"
				"entered begin"			"\n"
				"entered begin keyword"	"\n"
				"exited begin keyword"	"\n"
				"entered block name"	"\n"
				"exited block name"		"\n"
				"exited begin"			"\n"
				"entered end"			"\n"
				"entered end keyword"	"\n"
				"exited end keyword"	"\n"
				"entered block name"	"\n"
				"exited block name"		"\n"
				"exited end"			"\n"
				"exited block"			"\n"
				"exited script"			"\n"
			), listener.log());
		}

		TEST_METHOD(TestListenerEventSequence_Begin_In_End)
		{
			parse("@begin b @in p @end b");

			Assert::AreEqual(std::string(
				"entered script"			"\n"
				"entered block"				"\n"
				"entered begin"				"\n"
				"entered begin keyword"		"\n"
				"exited begin keyword"		"\n"
				"entered block name"		"\n"
				"exited block name"			"\n"
				"exited begin"				"\n"
				"entered block qualifier"	"\n"
				"entered port"				"\n"
				"entered input port"		"\n"
				"entered in"				"\n"
				"exited in"					"\n"
				"exited input port"			"\n"
				"exited port"				"\n"
				"exited block qualifier"	"\n"
				"entered end"				"\n"
				"entered end keyword"		"\n"
				"exited end keyword"		"\n"
				"entered block name"		"\n"
				"exited block name"			"\n"
				"exited end"				"\n"
				"exited block"				"\n"
				"exited script"				"\n"
			), listener.log());
		}

		TEST_METHOD(TestListenerEventSequence_Begin_Out_End)
		{
			parse("@begin b @out p @end b");

			Assert::AreEqual(std::string(
				"entered script"			"\n"
				"entered block"				"\n"
				"entered begin"				"\n"
				"entered begin keyword"		"\n"
				"exited begin keyword"		"\n"
				"entered block name"		"\n"
				"exited block name"			"\n"
				"exited begin"				"\n"
				"entered block qualifier"	"\n"
				"entered port"				"\n"
				"entered output port"		"\n"
				"entered out"				"\n"
				"exited out"				"\n"
				"exited output port"		"\n"
				"exited port"				"\n"
				"exited block qualifier"	"\n"
				"entered end"				"\n"
				"entered end keyword"		"\n"
				"exited end keyword"		"\n"
				"entered block name"		"\n"
				"exited block name"			"\n"
				"exited end"				"\n"
				"exited block"				"\n"
				"exited script"				"\n"
			), listener.log());
		}


	};
}