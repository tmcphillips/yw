
#include "yw_graphviz_tests.h"

using namespace yw;
using namespace yw::graphviz;
using namespace yw::test;

YW_TEST_FIXTURE(GraphvizRenderer)

    StdoutRecorder stdoutRecorder;
    StderrRecorder stderrRecorder;

    YW_TEST_SETUP(GraphvizRenderer) {

    }

YW_TEST_SET

    YW_TEST(GraphvizRenderer, EmptyGraphReturnsMinimalSVGImageWithGivenTitle)
    {
        GraphvizRenderer renderer{ R"(
                digraph workflow {
                }
        )" };

        std::string svg = renderer.str();
        Expect::EmptyString(stdoutRecorder.str());
        Expect::EmptyString(stderrRecorder.str());

        Assert::AreEqual(trimmargins(R"rawxml(

                <?xml version="1.0" encoding="UTF-8" standalone="no"?>
                <!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
                 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
                <!-- Generated by graphviz version 2.39.20160612.1140 (20090106.0545)
                 -->
                <!-- Title: workflow Pages: 1 -->
                <svg width="8pt" height="8pt"
                 viewBox="0.00 0.00 8.00 8.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
                <g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 4)">
                <title>workflow</title>
                <polygon fill="white" stroke="transparent" points="-4,4 -4,-4 4,-4 4,4 -4,4"/>
                </g>
                </svg>

        )rawxml"), svg);
    }

    YW_TEST(GraphvizRenderer, GraphWithOneNodeAReturnsSVGImageWithSingleCirculareNodeLabeledWithNodeName)
    {
        GraphvizRenderer renderer{ R"(
                    digraph workflow {
                    b
                    }
            )" };

        std::string svg = renderer.str();
        Expect::EmptyString(stdoutRecorder.str());
        Expect::EmptyString(stderrRecorder.str());

        Assert::AreEqual(trimmargins(R"rawxml(

                <?xml version="1.0" encoding="UTF-8" standalone="no"?>
                <!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
                 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
                <!-- Generated by graphviz version 2.39.20160612.1140 (20090106.0545)
                 -->
                <!-- Title: workflow Pages: 1 -->
                <svg width="62pt" height="44pt"
                 viewBox="0.00 0.00 62.00 44.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
                <g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 40)">
                <title>workflow</title>
                <polygon fill="white" stroke="transparent" points="-4,4 -4,-40 58,-40 58,4 -4,4"/>
                <!-- b -->
                <g id="node1" class="node">
                <title>b</title>
                <ellipse fill="none" stroke="black" cx="27" cy="-18" rx="27" ry="18"/>
                <text text-anchor="middle" x="27" y="-14.3" font-family="Times New Roman,serif" font-size="14.00">b</text>
                </g>
                </g>
                </svg>

                )rawxml"), renderer.str());

        Expect::EmptyString(stdoutRecorder.str());
        Expect::EmptyString(stderrRecorder.str());
    }

YW_TEST_END
