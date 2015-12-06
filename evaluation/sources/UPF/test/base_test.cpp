#include "base_test.hpp"

const base_test::TestResult* base_test::BaseTest::TEST_OK = NULL;
size_t base_test::BaseTest::NUM_CHECKS = 0;

void base_test::BaseTest::run_tests(const Verbosity verbosity) {
  const size_t num_tests = tests_.size();
  start_time_ = time_util::Clock::now();
  print_suite_header(verbosity);
  for (size_t i = 0; i < num_tests; ++i) {
    base_test::BaseTest::NUM_CHECKS = 0;
    time_util::Clock::time_point test_start = time_util::Clock::now();
    // run the test
    const TestResult* result = tests_[i]();
    time_util::Clock::time_point test_end = time_util::Clock::now();
    test_results_.push_back(result);
    // record execution time
    const double runtime = time_util::duration(test_start, test_end);
    test_times_.push_back(runtime);
    print_test_result(i, base_test::BaseTest::NUM_CHECKS, runtime, verbosity);
  }
  end_time_ = time_util::Clock::now();
  if (verbosity == QUIET) {
    const size_t passed_tests = num_passed_tests();
    if (passed_tests < num_tests)
      std::cout << RED_BOLD;
    else
      std::cout << GREEN_BOLD;
    std::cout << " (" << passed_tests << "/" << num_tests << ")";
    std::cout << WHITE;
  }
  print_suite_footer(time_util::duration(start_time_, end_time_), verbosity);
}


void base_test::BaseTest::print_test_result(const size_t test_id,
    const size_t num_checks, const double runtime, const Verbosity verbosity) {

  const TestResult* result = test_results_[test_id];
  const bool failed = result != NULL;
  if (verbosity == QUIET) {
    std::cout << (failed ? "F" : ".");
    return;
  }
  std::cout << test_result_prefix(failed) <<  " " << test_names_[test_id]
      << ": " << test_result_msg(failed);
  if (verbosity == VERY_VERBOSE) {
    std::cout << std::endl << "  "
        << "(" << num_checks << " checks passed, " << runtime << " seconds)";
  }
  std::cout << std::endl;
  if (failed) {
    std::cout << "      ==> Check failed in file " << result->file()
        << ", " << RED_BOLD << "line " << result->line() << WHITE << std::endl
        << "      ==> Expression "<< RED_BOLD << "'" << result->expr()
        << "'" << WHITE << std::endl;
  }
}


std::string base_test::BaseTest::test_result_msg(const bool failed) {
  std::stringstream ss;
  if (failed)
    ss << RED_BOLD << "FAIL";
  else
    ss << GREEN_BOLD << "OK";
  ss << WHITE;
  return ss.str();
}


std::string base_test::BaseTest::test_result_prefix(const bool failed) {
  std::stringstream ss;
  if (failed)
    ss << RED_BOLD << "-";
  else
    ss << GREEN_BOLD << "+";
  ss << WHITE;
  return ss.str();
}


void base_test::BaseTest::print_suite_header(const Verbosity verbosity) {
  switch (verbosity) {
    case QUIET:
      std::cout << YELLOW_BOLD << suite_name_ << ": " << WHITE;
      break;
    case VERBOSE:
    case VERY_VERBOSE:
    default:
      std::cout << YELLOW_BOLD << "=======> " << "Suite: "
          << suite_name_ << WHITE << std::endl;
  }
}


void base_test::BaseTest::print_suite_footer(const double time,
    const Verbosity verbosity) {

  switch(verbosity) {
    case VERY_VERBOSE:
      std::cout << YELLOW_BOLD << "------" << WHITE << std::endl
          << num_passed_tests() << " of " << num_tests()
          << " tests passed" << std::endl
          << "Total test suite runtime: " << time << " seconds"
          << std::endl;
    case QUIET:
      std::cout << std::endl << std::endl;
      break;
    case VERBOSE:
      std::cout << std::endl;
      break;
    default:
      break;
  }
}


size_t base_test::BaseTest::num_passed_tests() const {
  size_t passed_tests = 0;
  const size_t num = num_tests();
  for (size_t i = 0; i < num; ++i)
    if (test_results_[i] == NULL)
      ++passed_tests;
  return passed_tests;
}


void base_test::BaseTest::print_suite_summary(
    const base_test::SuiteVector& suites) {

  std::cout << WHITE_BOLD << "TEST SUMMARY:" << std::endl
      << "=============" << WHITE << std::endl;
  const size_t num_suites = suites.size();

  double total_time = 0.0;
  size_t total_tests = 0;
  size_t total_passed_tests = 0;
  for (size_t i = 0; i < num_suites; ++i) {
    total_time += suites[i]->duration();
    total_tests += suites[i]->num_tests();
    total_passed_tests += suites[i]->num_passed_tests();
  }
  std::cout << "* " << total_passed_tests << " of " << total_tests
      << " tests passed"
      << std::endl << "* Total runtime: " << total_time << " seconds"
      << std::endl << std::endl;
}


void base_test::BaseTest::write_xml_output(
    const base_test::SuiteVector& suites, const std::string& xml_path) {

  const size_t num_suites = suites.size();
  for (size_t i = 0; i < num_suites; ++i) {
    base_test::BaseTest* suite = suites[i];
    const std::string& suite_name = suite->suite_name();
    std::stringstream namebuffer;
    namebuffer << xml_path << "/" << suite_name << ".xml";
    // try to open file
    std::ofstream out(namebuffer.str());
    if (!out.is_open()) {
      std::stringstream ss;
      ss << "xml output file '" << xml_path << "' cannot be opened";
      out.close();
      throw ss.str();
    }
    // write test results to file
    out << std::fixed << std::setprecision(6);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    const size_t num_tests = suite->num_tests();
    out << "<testsuite tests=\"" << num_tests << "\"";
    out << " time=\"" << suite->duration() << "\">" << std::endl;
    for (size_t j = 0; j < num_tests; ++j) {
      out << "  <testcase classname=\"" << suite_name << "\" ";
      out << "name=\"" << suite->test_name(j) << "\" ";
      out << "time=\"" << suite->test_time(j) << "\" ";
      const TestResult* result = suite->test_result(j);
      // check if the test succeeded
      if (result == BaseTest::TEST_OK) {
        out << "/>" << std::endl;
        continue;
      }
      // the test failed - give some additional information
      out << ">" << std::endl << "    <failure type=\"heisenbug\">";
      out << std::endl;
      out << "      File: " << result->file() << std::endl;
      out << "      Line: " << result->line() << std::endl;
      out << "      Error: " << result->expr() << std::endl;
      out << "    </failure>" << std::endl << "  </testcase>" << std::endl;
    }
    out << "</testsuite>" << std::endl;
    out.close();
  }
}
