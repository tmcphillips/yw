#include "yw_db_tests.h"

using namespace yw::test;
using namespace yw::db;

using std::make_unique;

YW_TEST_FIXTURE(UserTable)

	YesWorkflowDB ywdb { false };

	YW_TEST_SETUP(UserTable) 
	{
		ywdb.createUserTable();
	}

YW_TEST_SET

    YW_TEST(UserTable, InsertUser_OneRow_GeneratedIdIs_1)
    {
		Assert::AreEqual(1, ywdb.insert(UserRow{ "user1" }));
    }

    YW_TEST(UserTable, InsertUser_TwoRows_SecondGeneratedIdIs_2)
    {
		ywdb.insert(UserRow{ "user1" });
		Assert::AreEqual(2, ywdb.insert(UserRow{ "user2" }));
    }

    YW_TEST(UserTable, SelectUserById_RowExists) {
		ywdb.insert(UserRow{ "user1" });
        auto user = ywdb.selectUserById(1L);
        Assert::AreEqual(1, user.id);
        Assert::AreEqual(std::string("user1"), user.name);
    }

    YW_TEST(UserTable, SelectUserById_RowDoesntExist) {
        try {
            auto user = ywdb.selectUserById(1L);
            Assert::Fail();
        }
        catch (std::runtime_error& e) {
            Assert::AreEqual("No row with that id", e.what());
        }
    }

YW_TEST_END