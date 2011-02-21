#include <iostream>
#include <string>

#include "llvm/Support/Process.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/HeaderSearch.h"

using namespace clang;
using namespace llvm;
using namespace std;

int main()
{
    string str;
    raw_string_ostream ost(str);
    const DiagnosticOptions diagOptions;
    TextDiagnosticPrinter tdp(ost, diagOptions, true);
	LangOptions lang;

    const llvm::IntrusiveRefCntPtr<DiagnosticIDs> Diags;
	Diagnostic diag(Diags);
    FileSystemOptions fsOptions;
	FileManager fm(fsOptions);
	SourceManager sm(diag, fm);
	HeaderSearch headers(fm);
    TargetOptions tarOptions;
    tarOptions.Triple = LLVM_HOSTTRIPLE;
	TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, tarOptions);
	Preprocessor pp(diag, lang, *ti, sm, headers);
	return 0;
}
