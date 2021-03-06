#include "ywdb.h"
#include "insert_statement.h"
#include "select_statement.h"

using std::string;
using namespace yw::sqlite;

namespace yw {
    namespace db {

        void YesWorkflowDB::createAdjacentDataBlocksView() {
            SQLiteDB::createTable(db, std::string(R"(

                CREATE VIEW adjacent_data_blocks_view AS
                SELECT  workflow_block.id AS workflow_block_id,
                        input_data_block.id AS input_data_block_id,
                        input_data_block.name AS input_data_block_name,
                        input_port.id,
                        input_port.name,
                        program_block.id,
                        program_block.name AS program_block_name,
                        output_port.id,
                        output_port.name,
                        output_data_block.id AS output_data_block_id,
                        output_data_block.name AS output_data_block_name
                FROM program_block AS workflow_block
                JOIN program_block ON program_block.workflow = workflow_block.id
                JOIN port AS input_port ON input_port.program_block = program_block.id
                JOIN flow AS input_flow ON input_flow.port = input_port.id
                JOIN data_block AS input_data_block ON input_flow.data_block = input_data_block.id
                JOIN port AS output_port ON output_port.program_block = program_block.id
                JOIN flow AS output_flow ON output_flow.port = output_port.id
                JOIN data_block AS output_data_block ON output_flow.data_block = output_data_block.id
                WHERE input_flow.direction=1 AND output_flow.direction=2

            )"));
        }

        AdjacentDataBlockPair getAdjacentDataBlockPairFromSelectStatementFields(SelectStatement& statement) {
            
            auto workflowId = statement.getNullableIdField(0);
            auto inputDataBlockId = statement.getIdField(1);
            auto inputDataBlockName = statement.getTextField(2);
            auto inputPortId = statement.getIdField(3);
            auto inputPortName = statement.getTextField(4);
            auto programBlockId = statement.getIdField(5);
            auto programBlockName = statement.getTextField(6);
            auto outputPortId = statement.getIdField(7);
            auto outputPortName = statement.getTextField(8);
            auto outputDataBlockId = statement.getIdField(9);
            auto outputDataBlockName = statement.getTextField(10);

            return AdjacentDataBlockPair(workflowId, inputDataBlockId, inputDataBlockName, inputPortId, inputPortName,
                                  programBlockId, programBlockName, outputPortId, outputPortName,
                                  outputDataBlockId, outputDataBlockName);
        }

        std::vector<AdjacentDataBlockPair> YesWorkflowDB::selectAdjacentDataBlockPairsByWorkflowId(const row_id& workflowId) {
            auto sql = std::string(R"(
                SELECT * 
                FROM adjacent_data_blocks_view
                WHERE workflow_block_id = ?
                ORDER BY input_data_block_name, output_data_block_name, input_data_block_id, output_data_block_id
            )");
            SelectStatement statement(db, sql);
            statement.bindNullableId(1, workflowId);
            auto programChannels = std::vector<AdjacentDataBlockPair>{};
            while (statement.step() == SQLITE_ROW) {
                programChannels.push_back(getAdjacentDataBlockPairFromSelectStatementFields(statement));
            }
            return programChannels;
        }

        std::vector<DataDataEdge> YesWorkflowDB::selectDataDataEdges(const row_id& workflowId) {
            auto sql = std::string(R"(
                SELECT DISTINCT input_data_block_name, output_data_block_name 
                FROM adjacent_data_blocks_view
                WHERE workflow_block_id = ?
                ORDER BY input_data_block_name, output_data_block_name
            )");
            SelectStatement statement(db, sql);
            statement.bindNullableId(1, workflowId);
            auto dataDataEdges = std::vector<DataDataEdge>{};
            while (statement.step() == SQLITE_ROW) {
                dataDataEdges.push_back(DataDataEdge{
                    statement.getTextField(0),
                    statement.getTextField(1)
                });
            }
            return dataDataEdges;
        }

        std::vector<DataProgramDataEdge> YesWorkflowDB::selectDataProgramDataEdges(const row_id& workflowId) {
            auto sql = std::string(R"(
                SELECT DISTINCT input_data_block_name, program_block_name, output_data_block_name 
                FROM adjacent_data_blocks_view
                WHERE workflow_block_id = ?
                ORDER BY input_data_block_name, output_data_block_name, program_block_name
            )");
            SelectStatement statement(db, sql);
            statement.bindNullableId(1, workflowId);
            auto dataProgramDataEdges = std::vector<DataProgramDataEdge>{};
            while (statement.step() == SQLITE_ROW) {
                dataProgramDataEdges.push_back( DataProgramDataEdge{
                    statement.getTextField(0),
                    statement.getTextField(1),
                    statement.getTextField(2)
                });
            }
            return dataProgramDataEdges;
        }
    }
}