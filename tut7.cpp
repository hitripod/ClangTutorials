#include <iostream>
#include <string>
#include "llvm/Support/CommandLine.h"


using namespace std;
using namespace llvm;

cl::opt<int> OutputString("s", cl::desc("Specify what you what to print"), cl::value_desc("OutString"));
int main(int argc, char **argv)
{
    cl::ParseCommandLineOptions(argc, argv);
    cout << OutputString << endl;

    return 0;
}
