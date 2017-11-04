#include "yw_model_tests.h"

using namespace yw;
using namespace yw::model;
using namespace yw::extract;
using namespace yw::parse;
using namespace yw::test;
using namespace yw::db;

using Tag = yw::db::Annotation::Tag;
using Direction = yw::db::Flow::Direction;

YW_TEST_FIXTURE(ModelEntityListener)

    YesWorkflowDB ywdb;
    row_id modelId, extractionId, sourceId;
    std::shared_ptr<SourceLoader> sourceLoader;
    ModelEntityListener* listener;
    StderrRecorder stderrRecorder;

    void storeAndParse(const std::string& code) {
        sourceLoader->insertSourceLinesFromString(sourceId, code);
        YWParserBuilder parser_builder(code);
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(listener, parser_builder.parse()->script());
    }

    YW_TEST_SETUP(ModelEntityListener) {

        row_id userId;

        Expect::AreEqual(1, (userId = ywdb.insert(User{ auto_id, "user1" })));
        Expect::AreEqual(1, (extractionId = ywdb.insert(Extraction{ auto_id, userId, "2017-06-22 10:52:00.000" })));
        Expect::AreEqual(1, (sourceId = ywdb.insert(Source{ auto_id, null_id, "C" })));
        Expect::AreEqual(1, (modelId = ywdb.insert(Model{ auto_id, userId, extractionId, "2017-06-22 10:52:00.000" })));

        sourceLoader = std::make_shared<SourceLoader>(ywdb);
        listener = new ModelEntityListener{ ywdb, modelId, extractionId, sourceId };
    }

YW_TEST_SET

    YW_TEST(ModelEntityListener, WhenBeginAnnotationAtStartOfOnlyLineInsertOneBlockWithNullWorkflowId)
    {
        this->storeAndParse(R"(

            @begin b @end b

        )");

        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        auto programBlock = ywdb.selectProgramBlockById(1);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, programBlock);
        Assert::IsTrue(! programBlock.workflowId.hasValue());
    }

    YW_TEST(ModelEntityListener, WhenBeginAndEndAtStartOfOnlyLineInsertOneBlockWithNullWorkflowId)
    {
        this->storeAndParse(R"(
        
            @begin b @end b
        
        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        auto programBlock = ywdb.selectProgramBlockById(1);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, programBlock);
        Assert::IsFalse(programBlock.workflowId.hasValue());
    }

    YW_TEST(ModelEntityListener, WhenTwoBeginEndPairsAtTopLevelSecondOfTwoBlocksHasNullWorkflowId)
    {
        this->storeAndParse(R"(
            
            @begin b
            @end b

            @begin c
            @end c

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(4, ywdb.getRowCount("annotation"));
        Expect::AreEqual(2, ywdb.getRowCount("program_block"));

        //Expect::AreEqual(Annotation{ auto_id, 17, Tag::END, null_id, 19, 1, 0, 11, "@end", null_string }, ywdb.get

        auto programBlock2 = ywdb.selectProgramBlockById(2);
        Assert::AreEqual(ProgramBlock{ 2, modelId, null_id, 3, "c", null_string }, programBlock2);
        Assert::IsFalse(programBlock2.workflowId.hasValue());
    }

    YW_TEST(ModelEntityListener, WhenTwoBeginEndPairsAreNestedAtTopLevelWorkflowIdOfNestedBlockIsIdOfTopBlock)
    {
        this->storeAndParse(R"(

            @begin b
                @begin c
                @end c
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(2, ywdb.getRowCount("program_block"));

        auto topProgramBlock = ywdb.selectProgramBlockById(1);
        auto nestedProgramBlock = ywdb.selectProgramBlockById(2);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, topProgramBlock);
        Assert::AreEqual(ProgramBlock{ 2, modelId, 1, 2, "c", null_string }, nestedProgramBlock);
        Assert::AreEqual(topProgramBlock.id, nestedProgramBlock.workflowId);
    }

    YW_TEST(ModelEntityListener, WhenTwoBlocksAreNestedInsideTopLevelWorkflowIdOfSecondNestedBlockIsIdOfTopBlock)
    {
        this->storeAndParse(R"(

            @begin b
            
                @begin c
                @end c
                
                @begin d
                @end d
            
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(3, ywdb.getRowCount("program_block"));

        auto topProgramBlock = ywdb.selectProgramBlockById(1);
        auto nestedProgramBlock1 = ywdb.selectProgramBlockById(2);
        auto nestedProgramBlock2 = ywdb.selectProgramBlockById(3);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, topProgramBlock);
        Assert::AreEqual(ProgramBlock{ 2, modelId, 1, 2, "c", null_string }, nestedProgramBlock1);
        Assert::AreEqual(ProgramBlock{ 3, modelId, 1, 4, "d", null_string }, nestedProgramBlock2);
        Assert::AreEqual(topProgramBlock.id, nestedProgramBlock1.workflowId);
        Assert::AreEqual(topProgramBlock.id, nestedProgramBlock2.workflowId);
    }

    YW_TEST(ModelEntityListener, WhenBlockIsDoublyNestedItsWorkflowIdIsIdOfImmediateParentBlock)
    {
        this->storeAndParse(R"(
            
            @begin b
                @begin c
                    @begin d
                    @end d
                @end c
            @end b
        
        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(3, ywdb.getRowCount("program_block"));

        auto topProgramBlock = ywdb.selectProgramBlockById(1);
        auto nestedProgramBlock = ywdb.selectProgramBlockById(2);
        auto doublyNestedProgramBlock = ywdb.selectProgramBlockById(3);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, topProgramBlock);
        Assert::AreEqual(ProgramBlock{ 2, modelId, 1, 2, "c", null_string }, nestedProgramBlock);
        Assert::AreEqual(ProgramBlock{ 3, modelId, 2, 3, "d", null_string }, doublyNestedProgramBlock);
        Assert::AreEqual(topProgramBlock.id, nestedProgramBlock.workflowId);
        Assert::AreEqual(nestedProgramBlock.id, doublyNestedProgramBlock.workflowId);
    }

    YW_TEST(ModelEntityListener, WhenNestedBlockFollowsDoublyNestedBlockItsWorkflowIdIsIdOfTopBlock)
    {
        this->storeAndParse(R"(
            
            @begin b
                
                @begin c
                    @begin d
                    @end d
                @end c
                
                @begin e
                @end e
            
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(4, ywdb.getRowCount("program_block"));

        auto topProgramBlock = ywdb.selectProgramBlockById(1);
        auto nestedProgramBlock1 = ywdb.selectProgramBlockById(2);
        auto doublyNestedProgramBlock = ywdb.selectProgramBlockById(3);
        auto nestedProgramBlock2 = ywdb.selectProgramBlockById(4);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, topProgramBlock);
        Assert::AreEqual(ProgramBlock{ 2, modelId, 1, 2, "c", null_string }, nestedProgramBlock1);
        Assert::AreEqual(ProgramBlock{ 3, modelId, 2, 3, "d", null_string }, doublyNestedProgramBlock);
        Assert::AreEqual(ProgramBlock{ 4, modelId, 1, 6, "e", null_string }, nestedProgramBlock2);
        Assert::AreEqual(topProgramBlock.id, nestedProgramBlock1.workflowId);
        Assert::AreEqual(nestedProgramBlock1.id, doublyNestedProgramBlock.workflowId);
        Assert::AreEqual(topProgramBlock.id, nestedProgramBlock2.workflowId);
    }

    YW_TEST(ModelEntityListener, WhenSecondTopBlockFollowsDoublyNestedBlocksItsWorkflowIdIsNull)
    {
        this->storeAndParse(R"(

            @begin b
                @begin c
                    @begin d
                    @end d
                @end c
            @end b

            @begin e
            @end e

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(4, ywdb.getRowCount("program_block"));

        auto topProgramBlock1 = ywdb.selectProgramBlockById(1);
        auto nestedProgramBlock = ywdb.selectProgramBlockById(2);
        auto doublyNestedProgramBlock = ywdb.selectProgramBlockById(3);
        auto topProgramBlock2 = ywdb.selectProgramBlockById(4);
        Assert::AreEqual(ProgramBlock{ 1, modelId, null_id, 1, "b", null_string }, topProgramBlock1);
        Assert::AreEqual(ProgramBlock{ 2, modelId, 1, 2, "c", null_string }, nestedProgramBlock);
        Assert::AreEqual(ProgramBlock{ 3, modelId, 2, 3, "d", null_string }, doublyNestedProgramBlock);
        Assert::AreEqual(ProgramBlock{ 4, modelId, null_id, 7, "e", null_string }, topProgramBlock2);
        Assert::AreEqual(topProgramBlock1.id, nestedProgramBlock.workflowId);
        Assert::AreEqual(nestedProgramBlock.id, doublyNestedProgramBlock.workflowId);
        Assert::IsFalse(topProgramBlock2.workflowId.hasValue());
    }

    YW_TEST(ModelEntityListener, WhenInPortHasOneNameInsertOnePortOneFlowAndOneDataBlock)
    {
        this->storeAndParse(R"(

            @begin b
            @in p
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(1, ywdb.getRowCount("port"));
        Assert::AreEqual(1, ywdb.getRowCount("flow"));
        Assert::AreEqual(1, ywdb.getRowCount("data_block"));
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, ywdb.selectPortById(1));
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "p" }, ywdb.selectDataBlockById(1));
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::IN, 1, 1 }, ywdb.selectFlowById(1));
    }

    YW_TEST(ModelEntityListener, WhenInPortHasTwoNamesInsertTwoPorts)
    {
        this->storeAndParse(R"(

            @begin b
            @in p q
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(2, ywdb.getRowCount("port"));
        Assert::AreEqual(2, ywdb.getRowCount("flow"));
        Assert::AreEqual(2, ywdb.getRowCount("data_block"));
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, ywdb.selectPortById(1));
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "p" }, ywdb.selectDataBlockById(1));
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::IN, 1, 1 }, ywdb.selectFlowById(1));
        Assert::AreEqual(Port{ 2, 1, 3, "q" }, ywdb.selectPortById(2));
        Assert::AreEqual(DataBlock{ 2, modelId, null_id, null_id, "q" }, ywdb.selectDataBlockById(2));
        Assert::AreEqual(Flow{ 2, 2, 2, Direction::IN, 1, 1 }, ywdb.selectFlowById(2));
    }

    YW_TEST(ModelEntityListener, WhenTwoInPortsInsertTwoPorts)
    {
        this->storeAndParse(R"(

            @begin b
            @in p
            @in q
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(2, ywdb.getRowCount("port"));
        Assert::AreEqual(2, ywdb.getRowCount("flow"));
        Assert::AreEqual(2, ywdb.getRowCount("data_block"));
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, ywdb.selectPortById(1));
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "p" }, ywdb.selectDataBlockById(1));
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::IN, 1, 1 }, ywdb.selectFlowById(1));
        Assert::AreEqual(Port{ 2, 1, 3, "q" }, ywdb.selectPortById(2));
        Assert::AreEqual(DataBlock{ 2, modelId, null_id, null_id, "q" }, ywdb.selectDataBlockById(2));
        Assert::AreEqual(Flow{ 2, 2, 2, Direction::IN, 1, 1 }, ywdb.selectFlowById(2));
    }

    YW_TEST(ModelEntityListener, WhenTwoOutPortsInsertTwoPorts)
    {
        this->storeAndParse(R"(

            @begin b
            @out p
            @out q
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(2, ywdb.getRowCount("port"));
        Assert::AreEqual(2, ywdb.getRowCount("flow"));
        Assert::AreEqual(2, ywdb.getRowCount("data_block"));
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, ywdb.selectPortById(1));
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "p" }, ywdb.selectDataBlockById(1));
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, ywdb.selectFlowById(1));
        Assert::AreEqual(Port{ 2, 1, 3, "q" }, ywdb.selectPortById(2));
        Assert::AreEqual(DataBlock{ 2, modelId, null_id, null_id, "q" }, ywdb.selectDataBlockById(2));
        Assert::AreEqual(Flow{ 2, 2, 2, Direction::OUT, 1, 1 }, ywdb.selectFlowById(2));
    }

    YW_TEST(ModelEntityListener, WhenInPortHasAliasPortTakesNameAndDataBlockTakesAlias)
    {
        this->storeAndParse(R"(

            @begin b
            @in p @as d
            @end b

       )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(1, ywdb.getRowCount("port"));
        Assert::AreEqual(1, ywdb.getRowCount("flow"));
        Assert::AreEqual(1, ywdb.getRowCount("data_block"));
        auto port = ywdb.selectPortById(1);
        auto dataBlock = ywdb.selectDataBlockById(1);
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, port);
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "d" }, dataBlock);
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::IN, 1, 1 }, ywdb.selectFlowById(1));
        Assert::AreEqual("p", port.name);
        Assert::AreEqual("d", dataBlock.name);
    }

    YW_TEST(ModelEntityListener, WhenInPortWithTwoPortNamesHasAliasDataForFirstPortTakesNameAndDataForSecondPortTakesAlias)
    {
        this->storeAndParse(R"(

            @begin b
            @in p q @as d
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(2, ywdb.getRowCount("port"));
        Assert::AreEqual(2, ywdb.getRowCount("flow"));
        Assert::AreEqual(2, ywdb.getRowCount("data_block"));
        auto port1 = ywdb.selectPortById(1);
        auto dataBlock1 = ywdb.selectDataBlockById(1);
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, port1);
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "p" }, dataBlock1);
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::IN, 1, 1 }, ywdb.selectFlowById(1));
        Assert::AreEqual("p", port1.name);
        Assert::AreEqual("p", dataBlock1.name);
        auto port2 = ywdb.selectPortById(2);
        auto dataBlock2 = ywdb.selectDataBlockById(2);
        Assert::AreEqual(Port{ 2, 1, 3, "q" }, port2);
        Assert::AreEqual(DataBlock{ 2, modelId, null_id, null_id, "d" }, dataBlock2);
        Assert::AreEqual(Flow{ 2, 2, 2, Direction::IN, 1, 1 }, ywdb.selectFlowById(2));
        Assert::AreEqual("q", port2.name);
        Assert::AreEqual("d", dataBlock2.name);
    }

    YW_TEST(ModelEntityListener, WhenFirstInPortHasAliasDataBlockForFirstPortTakesAlias)
    {
        this->storeAndParse(R"(

            @begin b
            @in p @as d
            @in q
            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(1, ywdb.getRowCount("program_block"));

        Assert::AreEqual(2, ywdb.getRowCount("port"));
        Assert::AreEqual(2, ywdb.getRowCount("flow"));
        Assert::AreEqual(2, ywdb.getRowCount("data_block"));
        auto port1 = ywdb.selectPortById(1);
        auto dataBlock1 = ywdb.selectDataBlockById(1);
        Assert::AreEqual(Port{ 1, 1, 2, "p" }, port1);
        Assert::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "d" }, dataBlock1);
        Assert::AreEqual(Flow{ 1, 1, 1, Direction::IN, 1, 1 }, ywdb.selectFlowById(1));
        Assert::AreEqual("p", port1.name);
        Assert::AreEqual("d", dataBlock1.name);
        auto port2 = ywdb.selectPortById(2);
        auto dataBlock2 = ywdb.selectDataBlockById(2);
        Assert::AreEqual(Port{ 2, 1, 4, "q" }, port2);
        Assert::AreEqual(DataBlock{ 2, modelId, null_id, null_id, "q" }, dataBlock2);
        Assert::AreEqual(Flow{ 2, 2, 2, Direction::IN, 1, 1 }, ywdb.selectFlowById(2));
        Assert::AreEqual("q", port2.name);
        Assert::AreEqual("q", dataBlock2.name);
    }

    YW_TEST(ModelEntityListener, WhenOutPortNameMatchesInPortNameOnDifferentBlockThePortShareOneDataBlock)
    {
        this->storeAndParse(R"(

            @begin b
            @out d
            @end b

            @begin c
            @in d
            @end c

        )");
        Expect::EmptyString(stderrRecorder.str());
        
        Expect::AreEqual(2, ywdb.getRowCount("program_block"));
        Expect::AreEqual(2, ywdb.getRowCount("port"));
        Expect::AreEqual(2, ywdb.getRowCount("flow"));
        Expect::AreEqual(1, ywdb.getRowCount("data_block"));

        auto dataBlock = ywdb.selectDataBlockById(1);
        auto port1 = ywdb.selectPortById(1);
        auto flow1 = ywdb.selectFlowById(1);
        auto port2 = ywdb.selectPortById(2);
        auto flow2 = ywdb.selectFlowById(2);

        Expect::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "d" }, dataBlock);
        Expect::AreEqual(Port{ 1, 1, 2, "d" }, port1);
        Expect::AreEqual(Port{ 2, 2, 5, "d" }, port2);
        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow1);
        Expect::AreEqual(Flow{ 2, 2, 1, Direction::IN, 1, 1 }, flow2);

        Assert::AreEqual(port1.id, flow1.portId);
        Assert::AreEqual(dataBlock.id, flow1.dataBlockId);
        Assert::AreEqual(port2.id, flow2.portId);
        Assert::AreEqual(dataBlock.id, flow2.dataBlockId);
    }

    YW_TEST(ModelEntityListener, WhenTwoInPortNamesMatchTwoOutPortNamesEachMatchingPortPairSharesDataBlock)
    {
        this->storeAndParse(R"(

            @begin b
            @out d1 d2
            @end b

            @begin c
            @in d1 d2
            @end c

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(2, ywdb.getRowCount("program_block"));
        Expect::AreEqual(4, ywdb.getRowCount("port"));
        Expect::AreEqual(4, ywdb.getRowCount("flow"));
        Expect::AreEqual(2, ywdb.getRowCount("data_block"));

        auto dataBlock1 = ywdb.selectDataBlockById(1);
        auto dataBlock2 = ywdb.selectDataBlockById(2);
        auto port1 = ywdb.selectPortById(1);
        auto port2 = ywdb.selectPortById(2);
        auto port3 = ywdb.selectPortById(3);
        auto port4 = ywdb.selectPortById(4);
        auto flow1 = ywdb.selectFlowById(1);
        auto flow2 = ywdb.selectFlowById(2);
        auto flow3 = ywdb.selectFlowById(3);
        auto flow4 = ywdb.selectFlowById(4);

        Expect::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "d1" }, dataBlock1);
        Expect::AreEqual(DataBlock{ 2, modelId, null_id, null_id, "d2" }, dataBlock2);
        Expect::AreEqual(Port{ 1, 1, 2, "d1" }, port1);
        Expect::AreEqual(Port{ 2, 1, 3, "d2" }, port2);
        Expect::AreEqual(Port{ 3, 2, 6, "d1" }, port3);
        Expect::AreEqual(Port{ 4, 2, 7, "d2" }, port4);
        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow1);
        Expect::AreEqual(Flow{ 2, 2, 2, Direction::OUT, 1, 1 }, flow2);
        Expect::AreEqual(Flow{ 3, 3, 1, Direction::IN,  1, 1 }, flow3);
        Expect::AreEqual(Flow{ 4, 4, 2, Direction::IN,  1, 1 }, flow4);

        Assert::AreEqual(port1.id, flow1.portId);
        Assert::AreEqual(dataBlock1.id, flow1.dataBlockId);
        Assert::AreEqual(port2.id, flow2.portId);
        Assert::AreEqual(dataBlock2.id, flow2.dataBlockId);
        Assert::AreEqual(port3.id, flow3.portId);
        Assert::AreEqual(dataBlock1.id, flow3.dataBlockId);
        Assert::AreEqual(port4.id, flow4.portId);
        Assert::AreEqual(dataBlock2.id, flow4.dataBlockId);
    }


    YW_TEST(ModelEntityListener, WhenTwoInPortNamesOnNestedBlocksMatchTwoOutPortNamesEachMatchingPortPairSharesDataBlock)
    {
        this->storeAndParse(R"(

            @begin b

                @begin c
                @out d1 d2
                @end c

                @begin d
                @in d1 d2
                @end d

            @end b

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(3, ywdb.getRowCount("program_block"));
        Expect::AreEqual(4, ywdb.getRowCount("port"));
        Expect::AreEqual(4, ywdb.getRowCount("flow"));
        Expect::AreEqual(2, ywdb.getRowCount("data_block"));

        auto dataBlock1 = ywdb.selectDataBlockById(1);
        auto dataBlock2 = ywdb.selectDataBlockById(2);
        auto port1 = ywdb.selectPortById(1);
        auto port2 = ywdb.selectPortById(2);
        auto port3 = ywdb.selectPortById(3);
        auto port4 = ywdb.selectPortById(4);
        auto flow1 = ywdb.selectFlowById(1);
        auto flow2 = ywdb.selectFlowById(2);
        auto flow3 = ywdb.selectFlowById(3);
        auto flow4 = ywdb.selectFlowById(4);

        Expect::AreEqual(DataBlock{ 1, modelId, 1, null_id, "d1" }, dataBlock1);
        Expect::AreEqual(DataBlock{ 2, modelId, 1, null_id, "d2" }, dataBlock2);
        Expect::AreEqual(Port{ 1, 2, 3, "d1" }, port1);
        Expect::AreEqual(Port{ 2, 2, 4, "d2" }, port2);
        Expect::AreEqual(Port{ 3, 3, 7, "d1" }, port3);
        Expect::AreEqual(Port{ 4, 3, 8, "d2" }, port4);
        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow1);
        Expect::AreEqual(Flow{ 2, 2, 2, Direction::OUT, 1, 1 }, flow2);
        Expect::AreEqual(Flow{ 3, 3, 1, Direction::IN,  1, 1 }, flow3);
        Expect::AreEqual(Flow{ 4, 4, 2, Direction::IN,  1, 1 }, flow4);

        Assert::AreEqual(port1.id, flow1.portId);
        Assert::AreEqual(dataBlock1.id, flow1.dataBlockId);
        Assert::AreEqual(port2.id, flow2.portId);
        Assert::AreEqual(dataBlock2.id, flow2.dataBlockId);
        Assert::AreEqual(port3.id, flow3.portId);
        Assert::AreEqual(dataBlock1.id, flow3.dataBlockId);
        Assert::AreEqual(port4.id, flow4.portId);
        Assert::AreEqual(dataBlock2.id, flow4.dataBlockId);
    }

    YW_TEST(ModelEntityListener, WhenPairedPortsAtDifferentLevelsHaveDifferentNamesEachMatchingPortPairSharesDataBlock)
    {
        this->storeAndParse(R"(

            @begin b
            @out d1

                @begin c
                @out d2
                @end c

                @begin d
                @in d2
                @end d

            @end b

            @begin e
            @in d1
            @end e

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(4, ywdb.getRowCount("program_block"));
        Expect::AreEqual(4, ywdb.getRowCount("port"));
        Expect::AreEqual(4, ywdb.getRowCount("flow"));
        Expect::AreEqual(2, ywdb.getRowCount("data_block"));

        auto dataBlock1 = ywdb.selectDataBlockById(1);
        auto dataBlock2 = ywdb.selectDataBlockById(2);
        auto port1 = ywdb.selectPortById(1);
        auto port2 = ywdb.selectPortById(2);
        auto port3 = ywdb.selectPortById(3);
        auto port4 = ywdb.selectPortById(4);
        auto flow1 = ywdb.selectFlowById(1);
        auto flow2 = ywdb.selectFlowById(2);
        auto flow3 = ywdb.selectFlowById(3);
        auto flow4 = ywdb.selectFlowById(4);

        Expect::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "d1" }, dataBlock1);
        Expect::AreEqual(DataBlock{ 2, modelId, 1, null_id, "d2" }, dataBlock2);
        Expect::AreEqual(Port{ 1, 1, 2, "d1" }, port1);
        Expect::AreEqual(Port{ 2, 2, 4, "d2" }, port2);
        Expect::AreEqual(Port{ 3, 3, 7, "d2" }, port3);
        Expect::AreEqual(Port{ 4, 4, 11, "d1" }, port4);
        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow1);
        Expect::AreEqual(Flow{ 2, 2, 2, Direction::OUT, 1, 1 }, flow2);
        Expect::AreEqual(Flow{ 3, 3, 2, Direction::IN,  1, 1 }, flow3);
        Expect::AreEqual(Flow{ 4, 4, 1, Direction::IN,  1, 1 }, flow4);

        Assert::AreEqual(port1.id, flow1.portId);
        Assert::AreEqual(dataBlock1.id, flow1.dataBlockId);
        Assert::AreEqual(port2.id, flow2.portId);
        Assert::AreEqual(dataBlock2.id, flow2.dataBlockId);
        Assert::AreEqual(port3.id, flow3.portId);
        Assert::AreEqual(dataBlock2.id, flow3.dataBlockId);
        Assert::AreEqual(port4.id, flow4.portId);
        Assert::AreEqual(dataBlock1.id, flow4.dataBlockId);
    }

    YW_TEST(ModelEntityListener, WhenPairedPortsAtDifferentLeveHaveSameNamesEachMatchingPairsShareDifferentDataBlocks)
    {
        this->storeAndParse(R"(

            @begin b
            @out d

                @begin c
                @out d
                @end c

                @begin d
                @in d
                @end d

            @end b

            @begin e
            @in d
            @end e

        )");

        Expect::EmptyString(stderrRecorder.str());
        Expect::AreEqual(4, ywdb.getRowCount("program_block"));
        Expect::AreEqual(4, ywdb.getRowCount("port"));
        Expect::AreEqual(4, ywdb.getRowCount("flow"));
        Expect::AreEqual(2, ywdb.getRowCount("data_block"));

        auto dataBlock1 = ywdb.selectDataBlockById(1);
        auto dataBlock2 = ywdb.selectDataBlockById(2);
        auto port1 = ywdb.selectPortById(1);
        auto port2 = ywdb.selectPortById(2);
        auto port3 = ywdb.selectPortById(3);
        auto port4 = ywdb.selectPortById(4);
        auto flow1 = ywdb.selectFlowById(1);
        auto flow2 = ywdb.selectFlowById(2);
        auto flow3 = ywdb.selectFlowById(3);
        auto flow4 = ywdb.selectFlowById(4);

        Expect::AreEqual(DataBlock{ 1, modelId, null_id, null_id, "d" }, dataBlock1);
        Expect::AreEqual(DataBlock{ 2, modelId, 1, null_id, "d" }, dataBlock2);
        Expect::AreEqual(Port{ 1, 1, 2, "d" }, port1);
        Expect::AreEqual(Port{ 2, 2, 4, "d" }, port2);
        Expect::AreEqual(Port{ 3, 3, 7, "d" }, port3);
        Expect::AreEqual(Port{ 4, 4, 11, "d" }, port4);
        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow1);
        Expect::AreEqual(Flow{ 2, 2, 2, Direction::OUT, 1, 1 }, flow2);
        Expect::AreEqual(Flow{ 3, 3, 2, Direction::IN,  1, 1 }, flow3);
        Expect::AreEqual(Flow{ 4, 4, 1, Direction::IN,  1, 1 }, flow4);

        Assert::AreEqual(port1.id, flow1.portId);
        Assert::AreEqual(dataBlock1.id, flow1.dataBlockId);
        Assert::AreEqual(port2.id, flow2.portId);
        Assert::AreEqual(dataBlock2.id, flow2.dataBlockId);
        Assert::AreEqual(port3.id, flow3.portId);
        Assert::AreEqual(dataBlock2.id, flow3.dataBlockId);
        Assert::AreEqual(port4.id, flow4.portId);
        Assert::AreEqual(dataBlock1.id, flow4.dataBlockId);
    }

    YW_TEST(ModelEntityListener, WhenOutPortHasFileAnnotationWithSimpleFileNameArgumentFlowTemplateAnnotationContainsFileNameAsPath)
    {
        this->storeAndParse(R"(

            @begin b
            @out d @file foo.txt
            @end b

            @begin c
            @in d
            @end c

        )");
        Expect::EmptyString(stderrRecorder.str());

        Expect::AreEqual(2, ywdb.getRowCount("flow"));
        Expect::AreEqual(1, ywdb.getRowCount("flow_template"));

        auto flow = ywdb.selectFlowById(1);
        auto flowTemplate = ywdb.selectFlowTemplateById(1);

        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow);
        Expect::AreEqual(FlowTemplate{ 1, 1, null_string, "foo.txt" }, flowTemplate);

        Assert::AreEqual(flow.id, flowTemplate.flow);
    }

    YW_TEST(ModelEntityListener, WhenOutPortHasUriAnnotationWithSimpleFileNameArgumentFlowTemplateAnnotationContainsFileNameAsPath)
    {
        this->storeAndParse(R"(

            @begin b
            @out d @uri foo.txt
            @end b

            @begin c
            @in d
            @end c

        )");
        Expect::EmptyString(stderrRecorder.str());

        Expect::AreEqual(2, ywdb.getRowCount("flow"));
        Expect::AreEqual(1, ywdb.getRowCount("flow_template"));

        auto flow = ywdb.selectFlowById(1);
        auto flowTemplate = ywdb.selectFlowTemplateById(1);

        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow);
        Expect::AreEqual(FlowTemplate{ 1, 1, null_string, "foo.txt" }, flowTemplate);

        Assert::AreEqual(flow.id, flowTemplate.flow);
    }

    YW_TEST(ModelEntityListener, WhenOutPortHasUriAnnotationWithSchemAndSimpleFileNameArgumentFlowTemplateAnnotationContainsSchemeAndFileNameAsPath)
    {
        this->storeAndParse(R"(

            @begin b
            @out d @uri file:foo.txt
            @end b

            @begin c
            @in d
            @end c

        )");
        Expect::EmptyString(stderrRecorder.str());

        Expect::AreEqual(2, ywdb.getRowCount("flow"));
        Expect::AreEqual(1, ywdb.getRowCount("flow_template"));

        auto flow = ywdb.selectFlowById(1);
        auto flowTemplate = ywdb.selectFlowTemplateById(1);

        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow);
        Expect::AreEqual(FlowTemplate{ 1, 1, "file", "foo.txt" }, flowTemplate);

        Assert::AreEqual(flow.id, flowTemplate.flow);
    }

    YW_TEST(ModelEntityListener, WhenOutPortHasUriAnnotationWithGeneralUrlArgumentFlowTemplateAnnotationContainsSchemeAndPath)
    {
        this->storeAndParse(R"(

            @begin b
            @out d @uri http://foo.org/a.dir/general.dir/url/bar.txt
            @end b

            @begin c
            @in d
            @end c

        )");
        Expect::EmptyString(stderrRecorder.str());

        Expect::AreEqual(2, ywdb.getRowCount("flow"));
        Expect::AreEqual(1, ywdb.getRowCount("flow_template"));

        auto flow = ywdb.selectFlowById(1);
        auto flowTemplate = ywdb.selectFlowTemplateById(1);

        Expect::AreEqual(Flow{ 1, 1, 1, Direction::OUT, 1, 1 }, flow);
        Expect::AreEqual(FlowTemplate{ 1, 1, "http", "//foo.org/a.dir/general.dir/url/bar.txt" }, flowTemplate);

        Assert::AreEqual(flow.id, flowTemplate.flow);
    }


YW_TEST_END
