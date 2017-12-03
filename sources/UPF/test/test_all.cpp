#ifndef TEST_ALL
#define TEST_ALL
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <iomanip>
#include "stdlib.h"

#include "base_test.hpp"
#include "test_parser.cpp"
#include "test_tokenizer.cpp"
#include "test_rulegenerator.cpp"
#include "test_field.cpp"
#include "test_upf.cpp"
#include "test_utility.cpp"
#include "test_upf_manager.cpp"
#include "test_dec_tree.cpp"
#include "test_graph.cpp"
#include "test_fdd.cpp"
#include "test_fw_compressor.cpp"

typedef std::chrono::high_resolution_clock Clock;

/*
 * Runs all tests.
 */
void run_all_tests(const base_test::Verbosity verbosity,
    base_test::SuiteVector& test_suites) {

  // add test suites here
  test_suites.push_back(new test_parser::TestParser());
  test_suites.push_back(new test_tokenizer::TestTokenizer());
  test_suites.push_back(new test_rulegenerator::TestRulegenerator());
  test_suites.push_back(new test_field::TestField());
  test_suites.push_back(new test_upf::TestUPF());
  test_suites.push_back(new test_utility::TestUtility());
////  test_suites.push_back(new test_upf_manager::TestCBSets_single());
////  test_suites.push_back(new test_upf_manager::TestCBSets_multithreaded());
  test_suites.push_back(new test_dec_tree::TestDecTree());
  test_suites.push_back(new test_graph::TestGraph());
  test_suites.push_back(new test_fdd::TestFDD());
  test_suites.push_back(new test_fw_compressor::TestFWCompressor());

  // execute test suites
  std::cout << std::endl;
  const size_t num_suites = test_suites.size();
  for (size_t i = 0; i < num_suites; ++i)
    test_suites[i]->run_tests(verbosity);
}


/*
 * Sets the format for floating-point output.
 */
void format_output() {
  std::cout << std::fixed << std::setprecision(4);
}


/*
 * Wrapper for arguments.
 */
class Arguments {
public:
  Arguments() : verbosity_(base_test::VERBOSE), xml_path_("") {}

  inline base_test::Verbosity verbosity() const {return verbosity_;}
  inline const std::string& xml_path() const {return xml_path_;}

  inline void verbosity(const base_test::Verbosity verbosity) {
    verbosity_ = verbosity;
  }
  inline void xml_path(const char* xml_path) {xml_path_ = xml_path;}

  /*
   * States whether XML output files with JUnit Ant compliant test results
   * should be created.
   */
  inline bool xml_specified() const{return xml_path_.size() > 0;}

private:
  base_test::Verbosity verbosity_;
  std::string xml_path_;
};


/*
 * Utility function for command-line parsing.
 */
void parse_args(const size_t argc, char* argv[], Arguments& args) {
  bool verbosity_set = false;
  bool xml_set = false;
  for (size_t i = 1;;) {
    if (i >= argc)
      return;
    const char* arg = argv[i];
    // dispatch
    if ((strcmp(arg, "-v") == 0) || (strcmp(arg, "--verbose") == 0)) {
      if (verbosity_set)
        throw std::string("verbosity parameter specified more than once");
      args.verbosity(base_test::VERY_VERBOSE);
      verbosity_set = true;
      ++i;

    } else if ((strcmp(arg, "-q") == 0) || (strcmp(arg, "--quiet") == 0)) {
      if (verbosity_set)
        throw std::string("verbosity parameter specified more than once");
      args.verbosity(base_test::QUIET);
      verbosity_set = true;
      ++i;

    } else if (strcmp(arg, "--xml") == 0) {
      if (xml_set)
        throw std::string("--xml parameter specified more than once");
      if (i == argc - 1)
        throw std::string("--xml parameter needs a path parameter");
      ++i;
      arg = argv[i];
      args.xml_path(arg);
      xml_set = true;
      ++i;

    } else if ((strcmp(arg, "-h") == 0) || (strcmp(arg, "--help") == 0)
            || (strcmp(arg, "--usage") == 0)) {

      throw std::string("usage");

    } else {
      std::stringstream ss;
      ss << "unknown argument: '" << arg << "'";
      throw ss.str();
    }
  }
}


/*
 * Prints test utility usage and terminates with code 0.
 */
void print_usage_and_exit(const char* prog) {
  std::cout << base_test::YELLOW_BOLD << std::endl
      << "Usage: " << prog << std::endl
      << "        [(-v|--verbose)|(-q|--quiet)]" << std::endl
      << "        [--xml <PATH TO DIRECTORY WITH RESULT XML FILES>]"
      << std::endl
      << "        [-h|--help|--usage]"
      << std::endl << std::endl << base_test::WHITE;
  exit(0);
}


/*
 * Prints an error message and terminates with code 1.
 */
void print_error_and_exit(const std::string& error) {
  std::cout << base_test::RED_BOLD << std::endl
      << "ERROR: " << error << "!"
      << std::endl << std::endl << base_test::WHITE;
  exit(1);
}


/*
 * Test entry point.
 */
int main(int argc, char* argv[]) {
   Arguments args;
  try {
    parse_args(argc, argv, args);
  } catch (const std::string& msg) {
    if (msg == "usage")
      print_usage_and_exit(argv[0]);
    else
      print_error_and_exit(msg);
  }
  format_output();
  base_test::SuiteVector suites;
  run_all_tests(args.verbosity(), suites);
  base_test::BaseTest::print_suite_summary(suites);

  // write XML output, if demanded
  std::string xml_error("");
  if (args.xml_specified()) {
    try {
      base_test::BaseTest::write_xml_output(suites, args.xml_path());
    } catch (const std::string& msg) {
      xml_error = msg;
    }
  }

  // delete test suites
  for (size_t i = 0; i < suites.size(); ++i)
    delete suites[i];

  if (xml_error.size() > 0)
    print_error_and_exit(xml_error);
  return 0;
}

#endif // TEST_ALL
