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
    //string a;
    //raw_string_ostream ost(a);
    const DiagnosticOptions diagOptions;
    TextDiagnosticPrinter *tdp = 
        new TextDiagnosticPrinter(outs(), diagOptions, true);//, dops);
	LangOptions lang;

    DiagnosticIDs *diagID;
    const llvm::IntrusiveRefCntPtr< DiagnosticIDs > Diags;
	Diagnostic diag(Diags, tdp, false);
    FileSystemOptions fsOptions;
	FileManager fm(fsOptions);
	SourceManager sm(diag, fm);
	HeaderSearch headers(fm);
    TargetOptions tarOptions;
    tarOptions.Triple = LLVM_HOSTTRIPLE;
	TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, tarOptions);
	Preprocessor pp(diag, lang, *ti, sm, headers);

//tut2---------------------------------------------------------------
    const FileEntry *file = fm.getFile("input01.c");
    sm.createMainFileID(file);
    pp.EnterMainSourceFile();

    Token Tok;

    do {
        pp.Lex(Tok);
        if(diag.hasErrorOccurred())
            break;
        pp.DumpToken(Tok);
        std::cerr << std::endl;
    } while(Tok.isNot(tok::eof));

//tut3---------------------------------------------------------------

	return 0;
}
