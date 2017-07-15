#pragma once

#include <stdexcept>

namespace yw {
	namespace sqlite {

		template<typename T>
		class nullable {

			const bool isSet;
			const T value;

		public:

			nullable() : isSet(false), value() {}
			nullable(const T& value) : isSet(true), value(value) {}

			bool hasValue() const { return isSet; }

			const T& getValue() const {
				if (!isSet) { throw std::runtime_error("No value assigned"); }
				return value;
			}
		};

		class nullable_string : public nullable<std::string> {
		public:
			nullable_string() : nullable() {}
			nullable_string(const std::string& value) : nullable(value) {}
			nullable_string(const char * c_ptr) : nullable(
				c_ptr == nullptr ? nullable{} : nullable{ std::string(c_ptr) }) {}
		};

		using nullable_long = nullable<long>;
		using nullable_id = nullable<long>;
		
		static const auto null_id = nullable_long{};
		static const auto null_long = nullable_long{};
		static const auto null_string = nullable_string{};
	}
}