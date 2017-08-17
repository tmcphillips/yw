#include "yw_db_tests.h"

using namespace yw;
using namespace yw::test;
using namespace yw::db;

using std::make_unique;
using std::string;

using Tag = yw::db::Annotation::Tag;
using Direction = yw::db::Transceiver::Direction;

YW_TEST_FIXTURE(Transceiver)

    YesWorkflowDB ywdb { false };
    row_id user13;
    row_id extraction17;
    row_id source8;
    row_id line19, line25, line119, line125;
    row_id annotation11, annotation16, annotation22, annotation36;
    row_id model77;
    row_id block12, block13;
    row_id port28, port88;
    row_id data5, data92;

    YW_TEST_SETUP(Transceiver)
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
        Expect::AreEqual(25, ywdb.insert(Line{ (line25 = 25), source8, 4, "@in port" }));
        Expect::AreEqual(119, ywdb.insert(Line{ (line119 = 119), source8, 11, "@begin nested block" }));
        Expect::AreEqual(125, ywdb.insert(Line{ (line125 = 125), source8, 14, "@out second port" }));

        ywdb.createAnnotationTable();
        Expect::AreEqual(11, ywdb.insert(Annotation{ (annotation11 = 11), extraction17, Tag::BEGIN, null_id, line19, 1, 0, 11, "@begin", "block" }));
        Assert::AreEqual(16, ywdb.insert(Annotation{ (annotation16 = 16), extraction17, Tag::IN, annotation11, line25, 1, 0, 7, "@in", "port" }));
        Expect::AreEqual(22, ywdb.insert(Annotation{ (annotation22 = 22), extraction17, Tag::BEGIN, annotation11, line119, 1, 0, 18, "@begin", "nested block" }));
        Assert::AreEqual(36, ywdb.insert(Annotation{ (annotation36 = 36), extraction17, Tag::IN, annotation22, line125, 1, 0, 16, "@out", "second port" }));

        ywdb.createModelTable();
        Expect::AreEqual(77, ywdb.insert(Model{ (model77 = 77), user13, extraction17, "2017-06-22 10:52:00.000" }));

        ywdb.createBlockTable();
        Expect::AreEqual(12, (ywdb.insert(Block{ (block12 = 12), model77, null_id, annotation11, "block" })));
        Expect::AreEqual(13, (ywdb.insert(Block{ (block13 = 13), model77, block12, annotation22, "nested block" })));

        ywdb.createPortTable();
        Expect::AreEqual(28, ywdb.insert(Port{ (port28 = 28), block12, annotation16, "port" }));
        Expect::AreEqual(88, ywdb.insert(Port{ (port88 = 88), block13, annotation36, "second port" }));

        ywdb.createDataTable();
        Expect::AreEqual(5, ywdb.insert(Data{ (data5 = 5), null_id, "d" }));
        Expect::AreEqual(92, ywdb.insert(Data{ (data92 = 92), 5, "e" }));

        ywdb.createTransceiverTable();
    }

YW_TEST_SET

    YW_TEST(Transceiver, InsertOneRow_GeneratedIdIs_1)
    {
        Assert::AreEqual(1, ywdb.insert(Transceiver{ auto_id, port28, data5, 
            Direction::IN, nullable_long(1), nullable_long(1) }));
    }

    YW_TEST(Transceiver, InsertTwoRows_SecondGeneratedIdIs_2)
    {
        Expect::AreEqual(1, ywdb.insert(Transceiver{ auto_id, port28, data5,
            Direction::IN, nullable_long(1), nullable_long(1) }));
        Assert::AreEqual(2, ywdb.insert(Transceiver{ auto_id, port88, data92,
            Direction::OUT, nullable_long(1), nullable_long(1) }));
    }

    YW_TEST(Transceiver, SelectById_RowExists) {

        Expect::AreEqual(1, ywdb.insert(Transceiver{ auto_id, port28, data5,
            Direction::IN, nullable_long(1), nullable_long(2) }));
        Expect::AreEqual(2, ywdb.insert(Transceiver{ auto_id, port88, data92,
            Direction::OUT, nullable_long(3), nullable_long(4) }));

        auto transceiver = ywdb.selectTransceiverById(2L);
        Assert::AreEqual(2, transceiver.id.getValue());
        Assert::AreEqual(88, transceiver.portId);
        Assert::AreEqual(92, transceiver.dataId);
        Assert::AreEqual(Transceiver::Direction::OUT, transceiver.direction);
        Assert::AreEqual(3, transceiver.minRate.getValue());
        Assert::AreEqual(4, transceiver.maxRate.getValue());
    }
    
    YW_TEST(Transceiver, SelectById_RowDoesntExist) {
        try {
            auto user = ywdb.selectTransceiverById(1L);
            Assert::Fail();
        }
        catch (std::runtime_error& e) {
            Assert::AreEqual("No transceiver row with that id", e.what());
        }
    }

YW_TEST_END