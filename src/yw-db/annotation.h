#pragma once

#include "ywdb.h"
#include <string>
#include <sstream>
#include <ostream>
#include <map>

namespace yw {
    namespace db {

        struct Annotation : sqlite::TableRow {

            enum class Tag {
                NONE = 0,
                BEGIN = 1,
                END = 2,
                DESC = 3,
                IN = 4,
                OUT = 5,
                PARAM = 6,
                RETURN = 7,
                AS = 8, 
                FILE = 9,
                URI = 10
            };

            nullable_row_id id;

            const row_id extractionId;
            const Tag tag;
            const nullable_row_id qualifiesId;
            const row_id lineId;
            const long rankOnLine;
            const long start;
            const long end;
            const std::string keyword;
            const nullable_string value;

            Annotation(
                const nullable_row_id& id,
                const row_id& extractionId,
                Tag tag,
                const nullable_row_id& qualifiesId,
                const row_id& lineId,
                long rankOnLine,
                long start,
                long end,
                const std::string& keyword,
                const nullable_string& value
            ) : id(id), extractionId(extractionId), tag(tag), qualifiesId(qualifiesId), lineId(lineId),
                rankOnLine(rankOnLine), start(start), end(end), keyword(keyword), value(value)
            {}

            std::string fieldValues() const override {
                std::stringstream ss;
                ss  <<        id.str()
                    << "|" << extractionId
                    << "|" << to_string(tag)
                    << "|" << qualifiesId.str()
                    << "|" << lineId
                    << "|" << rankOnLine
                    << "|" << start
                    << "|" << end
                    << "|" << keyword
                    << "|" << value.str();
                return ss.str();
            }

            static std::string to_string(Tag tag) {
                static const std::vector<std::string> names{
                    "NONE", "BEGIN", "END", "DESC", "IN", "OUT", "PARAM", "RETURN", "AS", "FILE", "URI"
                };
                return names[static_cast<int>(tag)];
            }
        };
    }
}