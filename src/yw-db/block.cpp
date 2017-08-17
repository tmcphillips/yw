#include "ywdb.h"
#include "insert_statement.h"
#include "select_statement.h"

using std::string;
using namespace yw::sqlite;

namespace yw {
    namespace db {

        void YesWorkflowDB::createBlockTable() {
            SQLiteDB::createTable(db, std::string(R"(

                CREATE TABLE block(
                    id              INTEGER         NOT NULL        PRIMARY KEY,
                    model           INTEGER         NOT NULL        REFERENCES model(id),
                    workflow        INTEGER         NULL            REFERENCES block(id),
                    annotation      INTEGER         NULL            REFERENCES annotation(id),
                    name            TEXT            NOT NULL
                );

            )"));
        }

        row_id YesWorkflowDB::insert(const Block& block) {
            string sql = "INSERT INTO block(id, model, workflow, annotation, name) VALUES (?,?,?,?,?);";
            InsertStatement statement(db, sql);
            statement.bindNullableId(1, block.id);
            statement.bindId(2, block.modelId);
            statement.bindNullableId(3, block.workflowId);
            statement.bindNullableId(4, block.annotationId);
            statement.bindText(5, block.name);
            statement.execute();
            return statement.getGeneratedId();
        }

        Block YesWorkflowDB::selectBlockById(const row_id& requested_id) {
            string sql = "SELECT id, model, workflow, annotation, name FROM block WHERE id = ?";
            SelectStatement statement(db, sql);
            statement.bindId(1, requested_id);
            if (statement.step() != SQLITE_ROW) throw std::runtime_error("No block row with that id");
            auto id = statement.getNullableIdField(0);
            auto modelId = statement.getIdField(1);
            auto workflowId = statement.getNullableIdField(2);
            auto annotationId = statement.getNullableIdField(3);
            auto name = statement.getTextField(4);
            return Block(id, modelId, workflowId, annotationId, name);
        }
    }
}