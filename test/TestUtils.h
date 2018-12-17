#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>
#include <functional>


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
			t.second();
		}
	}

private:
	TestRunner() = default;

	std::vector<std::pair<std::string, TestFunc>> tests_;
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
#define EXPECT_NE(u, v) expect_ne(__LINE__, u, v)
#define EXPECT_TRUE(v) expect_eq(__LINE__, true, v)
#define EXPECT_FALSE(v) expect_eq(__LINE__, false, v)


template<typename Container>
std::ostream& OutputToStream(std::ostream& stream, const Container& container) {
	stream << "[";
	auto it = container.begin(), it_end = container.end();
	if (it != it_end) {
		stream << *it;
		++it;
	}
	for (; it != it_end; ++it) {
		stream << " " << *it;
	}
	stream << "]";
	return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec) {
	return OutputToStream(stream, vec);
}

template<typename Type>
std::vector<Type> MakeVector(std::initializer_list<Type> ls) {
	std::vector<Type> vec;
	for (auto& item : ls) {
		vec.push_back(item);
	}
	return vec;
}

template<typename U, typename V>
void expect_eq(int line, const U& expected, const V& actual) {
	if (!(expected == actual)) {
		std::cerr << "Error in line " << line << std::endl;
		std::cerr << "  expected: " << expected << std::endl;
		std::cerr << "    actual: " << actual << std::endl;
	}
}

template<typename U, typename V>
void expect_ne(int line, const U& expected, const V& actual) {
	if (!(expected != actual)) {
		std::cerr << "Error in line " << line << std::endl;
		std::cerr << "  expected: " << expected << std::endl;
		std::cerr << "    actual: " << actual << std::endl;
	}
}

