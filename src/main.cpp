#include <algorithm>

#include "analyzer.h"
#include "parser.h"
#include "util.h"

// https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
auto getCmdOption(char ** begin, char ** end, const std::string & option) -> char* {
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return nullptr;
}

auto cmdOptionExists(char** begin, char** end, const std::string& option) -> bool {
  return std::find(begin, end, option) != end;
}

void printUsage() {
  std::cout
    << "Usage: slippc -i <infile> [-j <jsonfile>] [-a <analysisfile>] [-f] [-d] [-h]:" << std::endl
    << "  -i     Parse and analyze <infile> (not very useful on its own)" << std::endl
    << "  -j     Output <infile> in .json format to <jsonfile>" << std::endl
    << "  -a     Output an analysis of <infile> in .json format to <analysisfile> (use \"-\" for stdout)" << std::endl
    << "  -f     When used with -j <jsonfile>, write full frame info (instead of just frame deltas)" << std::endl
    << "  -d     Run in debug mode (show debug output)" << std::endl
    << "  -D     Run in verbose debug mode (show more debug output)" << std::endl
    << "  -h     Show this help message" << std::endl
    ;
}

auto main(int argc, char** argv) -> int {
  if (cmdOptionExists(argv, argv+argc, "-h")) {
    printUsage();
    return 0;
  }
  int debug = 0;
  if (cmdOptionExists(argv, argv+argc, "-D")) {
    debug = 2;
  } else if (cmdOptionExists(argv, argv+argc, "-d")) {
    debug = 1;
  }
  char * infile = getCmdOption(argv, argv + argc, "-i");
  if (infile == nullptr) {
    printUsage();
    return -1;
  }
  bool delta = not cmdOptionExists(argv, argv+argc, "-f");

  auto *p = new slip::Parser(debug);
  if (not p->load(infile)) {
    delete p;
    return 2;
  }

  char * outfile = getCmdOption(argv, argv + argc, "-j");
  if (outfile != nullptr) {
    p->save(outfile,delta);
  }

  char * analysisfile = getCmdOption(argv, argv + argc, "-a");
  if (analysisfile != nullptr) {
    slip::Analysis *a = p->analyze();
    if (analysisfile[0] == '-' and analysisfile[1] == '\0') {
      std::cout << a->asJson() << std::endl;
    } else {
      a->save(analysisfile);
    }
    delete a;
  }

  delete p;
  return 0;
}
