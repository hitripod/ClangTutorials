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
    const DiagnosticOptions diagOptions;
    TextDiagnosticPrinter *tdp = 
        new TextDiagnosticPrinter(outs(), diagOptions, true);
	const LangOptions lang;

    DiagnosticIDs *diagID;
    const llvm::IntrusiveRefCntPtr< DiagnosticIDs > Diags;
	Diagnostic diag(Diags, tdp, false);
    FileSystemOptions fsOptions;
	FileManager fm(fsOptions);
	SourceManager sm(diag, fm);

    TargetOptions tarOptions;
    tarOptions.Triple = LLVM_HOSTTRIPLE;
	const TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, tarOptions);

	HeaderSearch headers(fm);
	Preprocessor pp(diag, lang, *ti, sm, headers);
	return 0;
}
