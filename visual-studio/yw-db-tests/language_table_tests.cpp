#include "stdafx.h"

using namespace yw::test;
using namespace yw::db;

using std::make_unique;

namespace yw {
    namespace db {

        TEST_CLASS(LanguageTableTests)
        {
        public:

            TEST_METHOD(TestInsertLanguage_OneRow_GeneratedIdIs_1)
            {
                YesWorkflowDB ywdb;
                auto rowId = ywdb.insertLanguage("C");
                Assert::AreEqual(1, rowId);
            }

            TEST_METHOD(TestInsertLanguage_TwoRows_SecondGeneratedIdIs_2)
            {
                YesWorkflowDB ywdb;
                ywdb.insertLanguage("C");
                auto secondRowId = ywdb.insertLanguage("Java");
                Assert::AreEqual(2, secondRowId);
            }

            TEST_METHOD(TestSelectLanguageById_RowExists) {
                YesWorkflowDB ywdb;
                ywdb.insertLanguage("C");
                auto language = ywdb.selectLanguageById(1L);
                Assert::AreEqual(1, language.id);
                Assert::AreEqual(std::string("C"), language.name);
            }

            TEST_METHOD(TestSelectLanguageById_RowDoesntExist) {
                YesWorkflowDB ywdb;
                try {
                    auto language = ywdb.selectLanguageById(1L);
                    Assert::Fail();
                }
                catch (std::runtime_error& e) {
                    Assert::AreEqual("No row with that id", e.what());
                }
            }
        };
    }
}