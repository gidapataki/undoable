#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <set>


#ifdef TEST
#undef TEST
#endif

using TestFunc = std::function<void()>;

class TestRunner {
public:
	static TestRunner& Get() {
		static TestRunner instance;
		return instance;
	}

	void Add(const std::string& name, TestFunc fn) {
		tests_.push_back({name, std::move(fn)});
	}

	void RunAll() {
		for (auto& t : tests_) {
			std::cerr << "Running " << t.first << " ..." << std::endl;
			current_ = t.first;
			t.second();
			current_.clear();
		}

		auto success = tests_.size() - errors_.size();
		auto total = tests_.size();

		std::cerr << "--" << std::endl;
		std::cerr << "[" << success << "/" << total<< "]";
		std::cerr << " " << (success == total ?  "Success" : "Failed");
		std::cerr << std::endl;
	}

	void AddError() {
		if (!current_.empty()) {
			errors_.insert(current_);
		}
	}

private:
	TestRunner() = default;

	std::string current_;
	std::vector<std::pair<std::string, TestFunc>> tests_;
	std::set<std::string> errors_;
};


class TestRegistrar {
public:
	TestRegistrar(const std::string& name, TestFunc fn) {
		TestRunner::Get().Add(name, std::move(fn));
	}
};

#define TEST(testname, testcase) \
	void testname ## __ ## testcase (); \
	TestRegistrar reg_ ## testname ## __ ## testcase( \
		#testname "." #testcase, \
		testname ## __ ## testcase); \
	void testname ## __ ## testcase ()


#define EXPECT_EQ(u, v) expect_eq(__LINE__, u, v)
#define EXPECT_TRUE(v) expect_eq(__LINE__, true, (bool) v)
#define EXPECT_FALSE(v) expect_eq(__LINE__, false, (bool) v)


template<typename T>
void ToStream(std::ostream& stream, const T& value) {
	stream << value;
}

template<typename T>
void ToStream(std::ostream& stream, const std::vector<T>& container) {
	stream << "[";
	auto it = container.begin(), it_end = container.end();
	if (it != it_end) {
		ToStream(stream, *it);
		++it;
	}
	for (; it != it_end; ++it) {
		stream << ", ";
		ToStream(stream, *it);
	}
	stream << "]";
}

template<typename U, typename V>
void expect_eq(int line, const U& expected, const V& actual) {
	if (!(expected == actual)) {
		TestRunner::Get().AddError();
		std::cerr << "Error in line " << line << std::endl;
		std::cerr << "  expected: ";
		ToStream(std::cerr, expected);
		std::cerr << std::endl;
		std::cerr << "    actual: ";
		ToStream(std::cerr, actual);
		std::cerr << std::endl;
	}
}
