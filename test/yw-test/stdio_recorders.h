#pragma once

#include <sstream>

namespace yw {
	namespace test {

		class StderrRecorder {

			std::stringstream recording;
			std::streambuf * previousBuffer;

		public:

			StderrRecorder() { previousBuffer = std::cerr.rdbuf(recording.rdbuf()); }

			~StderrRecorder() { std::cerr.rdbuf(previousBuffer); }

			std::string str() { return recording.str(); }
		};
	}
}