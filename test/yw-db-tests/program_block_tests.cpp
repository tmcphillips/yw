#include "yw_db_tests.h"

using namespace yw;
using namespace yw::test;
using namespace yw::db;

using std::make_unique;
using std::string;
using Tag = yw::db::Annotation::Tag;

YW_TEST_FIXTURE(ProgramBlock)

    YesWorkflowDB ywdb { false };
    row_id user13;
    row_id extraction17;
    row_id source8;
    row_id line19, line119;
    row_id annotation11, annotation12;
    row_id model77;

    YW_TEST_SETUP(ProgramBlock)
    {
        ywdb.createUserTable();
        Expect::AreEqual(13, ywdb.insert(User{ (user13 = 13), "user1" }));

        ywdb.createExtractionTable();
        Expect::AreEqual(17, ywdb.insert(Extraction{ (extraction17 = 17), user13, "2017-06-22 10:52:00.000" }));

        ywdb.createFileTable();

        ywdb.createSourceTable();
        Expect::AreEqual(8, ywdb.insert(Source{ (source8 = 8), null_id, "C" }));

        ywdb.createLineTable();
        Expect::AreEqual(19, ywdb.insert(Line{ (line19 = 19), source8, 1, "@begin block" }));
        Expect::AreEqual(119, ywdb.insert(Line{ (line119 = 119), source8, 11, "@begin nested block" }));

        ywdb.createAnnotationTable();
        Expect::AreEqual(11, ywdb.insert(Annotation{ (annotation11 = 11), extraction17, Tag::BEGIN, null_id, line19, 1, 0, 11, "@begin", "block" }));
        Expect::AreEqual(12, ywdb.insert(Annotation{ (annotation12 = 12), extraction17, Tag::BEGIN, annotation11, line119, 1, 0, 18, "@begin", "nested block" }));

        ywdb.createModelTable();
        Expect::AreEqual(77, ywdb.insert(Model{ (model77 = 77), user13, extraction17, "2017-06-22 10:52:00.000" }));

        ywdb.createProgramBlockTable();
    }

YW_TEST_SET

    YW_TEST(ProgramBlock, InsertOneRow_GeneratedIdIs_1)
    {
        Assert::AreEqual(1, ywdb.insert(ProgramBlock{ auto_id, model77, null_id, annotation11, "block" }));
    }

    YW_TEST(ProgramBlock, InsertTwoRows_SecondGeneratedIdIs_2)
    {
        row_id block1;
        Expect::AreEqual(1, (block1 = ywdb.insert(ProgramBlock{ auto_id, model77, null_id, annotation11, "block" })));
        Assert::AreEqual(2, ywdb.insert(ProgramBlock{ auto_id, model77, block1, annotation12, "nested block" }));
    }

    YW_TEST(ProgramBlock, SelectById_RowExists) {

        row_id block1;
        Expect::AreEqual(1, (block1 = ywdb.insert(ProgramBlock{ auto_id, model77, null_id, annotation11, "block" })));
        Assert::AreEqual(2, ywdb.insert(ProgramBlock{ auto_id, model77, block1, annotation12, "nested block" }));

        auto block = ywdb.selectProgramBlockById(2);
        Assert::AreEqual(2L, block.id);
        Assert::AreEqual(77, block.modelId);
        Assert::AreEqual(1L, block.workflowId);
        Assert::AreEqual(12L, block.annotationId);
        Assert::AreEqual(std::string("nested block"), block.name);
    }
    
    YW_TEST(ProgramBlock, SelectById_RowDoesntExist) {
        try {
            auto user = ywdb.selectProgramBlockById(1L);
            Assert::Fail();
        }
        catch (std::runtime_error& e) {
            Assert::AreEqual("No program block with that id", e.what());
        }
    }

YW_TEST_END