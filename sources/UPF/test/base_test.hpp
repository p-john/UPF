#ifndef HARDBIT_TEST_BASE_TEST
#define HARDBIT_TEST_BASE_TEST

#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include "time_util.hpp"

#define REGISTER_TEST(testname) \
{ \
  tests_.push_back(std::function<const base_test::TestResult*()>(&testname)); \
  test_names_.push_back(#testname); \
}

/* the do-while loop is needed for the parser to use this macro in ifs without
 * braces */
#define CHECK(expr) \
do { \
  if (!(expr)) { \
    return new base_test::TestResult(__LINE__, __FILE__, #expr); \
  } else { \
    ++base_test::BaseTest::NUM_CHECKS; \
  } \
} while(0)

namespace base_test {

enum Verbosity {QUIET, VERBOSE, VERY_VERBOSE};

class BaseTest;
typedef std::vector<BaseTest*> SuiteVector;

//#ifdef NO_COLORS
static const std::string GREEN_BOLD("");
static const std::string RED_BOLD("");
static const std::string WHITE("");
static const std::string WHITE_BOLD("");
static const std::string YELLOW_BOLD("");
//#else
//static const std::string GREEN_BOLD("\033[01;32m");
//static const std::string RED_BOLD("\033[01;31m");
//static const std::string WHITE("\x1b[0m");
//static const std::string WHITE_BOLD("\033[01;37m");
//static const std::string YELLOW_BOLD("\033[01;33m");
//#endif

class TestResult {
public:
  TestResult(const size_t line, const std::string& file,
      const std::string& expr) : line_(line), file_(file), expr_(expr) {}

  inline size_t line() const {return line_;}
  inline const std::string& file() const {return file_;}
  inline const std::string& expr() const {return expr_;}

private:
  const size_t line_;
  const std::string file_;
  const std::string expr_;
};


class BaseTest {
public:

  BaseTest(const std::string& suite_name) : suite_name_(suite_name) {}

  ~BaseTest() {
    const size_t num_tests = this->num_tests();
    for (size_t i = 0; i < num_tests; ++i)
      delete test_results_[i];
  }


  /*
   * Executes all registered test cases and logs test execution times.
   */
  void run_tests(const Verbosity verbosity);

  /*
   * Computes the total execution time for the test suite.
   */
  inline double duration() {
    return time_util::duration(start_time_, end_time_);
  }

  /*
   * Returns the total number of passed tests in this suite.
   */
  size_t num_passed_tests() const;

  // getters
  inline size_t num_tests() const {return tests_.size();}
  inline const std::string& suite_name() const {return suite_name_;}
  inline std::string test_name(const size_t i) const {return test_names_[i];}
  inline const TestResult* test_result(const size_t i) const {
      return test_results_[i];
  }
  inline double test_time(const size_t i) const {return test_times_[i];}

  static size_t NUM_CHECKS;
  static const TestResult* TEST_OK;

  /*
   * Prints a summary of all given test suites.
   */
  static void print_suite_summary(const SuiteVector& suites);

  /*
   * Creates JUnit Ant compliant test output XML files in the specified
   * directory.
   * Throws an std::string if an output file cannot be opened.
   */
   static void write_xml_output(const SuiteVector& suites,
      const std::string& xml_path);

protected:
  std::vector<std::function<const TestResult*()>> tests_;
  std::vector<std::string> test_names_;
  std::string testfile_;

private:
  const std::string suite_name_;
  time_util::Clock::time_point start_time_;
  time_util::Clock::time_point end_time_;
  std::vector<double> test_times_;
  std::vector<const TestResult*> test_results_;

  /*
   * Prints the result of the most recent executed test case.
   */
  void print_test_result(const size_t test_id, const size_t num_checks,
      const double runtime, const Verbosity verbosity);

  /*
   *  Returns a pretty-print of the test result.
   */
  std::string test_result_msg(const bool failed);

  /*
   *  Returns a colored prefix of the test result.
   */
  std::string test_result_prefix(const bool failed);

  /*
   * Prints the header of a test suite.
   */
  void print_suite_header(const Verbosity verbosity);

  /*
   * Prints the footer of a test suite.
   */
  void print_suite_footer(const double time, const Verbosity verbosity);
};

} // namespace base_test

#endif // HARDBIT_TEST_BASE_TEST
