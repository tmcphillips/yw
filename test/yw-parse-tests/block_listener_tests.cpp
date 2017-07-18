#include "yw_parse_tests.h"

using namespace yw;
using namespace yw::parse;
using namespace yw::test;

YW_TEST_FIXTURE(BlockListener)

	StubYWListener listener;
	StderrRecorder stderrRecorder;

YW_TEST_SET

    YW_TEST(BlockListener, Begin_In_End)
    {
        YWParserBuilder parser_builder("@begin b @in p @end");
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, parser_builder.parse()->block());
		Expect::EmptyString(stderrRecorder.str());

        Assert::AreEqual(
            "entered block"                 "\n"
            "entered begin tag"             "\n"
            "entered block name"            "\n"
            "exited block name"             "\n"
            "exited begin tag"              "\n"
            "entered block attribute"       "\n"
            "entered port tag"              "\n"
            "entered input port keyword"    "\n"
            "exited input port keyword"     "\n"
            "entered port name"             "\n"
            "exited port name"              "\n"
            "exited port tag"               "\n"
            "exited block attribute"        "\n"
            "entered end tag"               "\n"
            "exited end tag"                "\n"
            "exited block"                  "\n"
        , listener.log());
    }

    YW_TEST(BlockListener, Begin_Out_End)
    {
        YWParserBuilder parser_builder("@begin b @out p @end");
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, parser_builder.parse()->block());
		Expect::EmptyString(stderrRecorder.str());

        Assert::AreEqual(
            "entered block"                 "\n"
            "entered begin tag"             "\n"
            "entered block name"            "\n"
            "exited block name"             "\n"
            "exited begin tag"              "\n"
            "entered block attribute"       "\n"
            "entered port tag"              "\n"
            "entered output port keyword"   "\n"
            "exited output port keyword"    "\n"
            "entered port name"             "\n"
            "exited port name"              "\n"
            "exited port tag"               "\n"
            "exited block attribute"        "\n"
            "entered end tag"               "\n"
            "exited end tag"                "\n"
            "exited block"                  "\n"
        , listener.log());
    }

YW_TEST_END