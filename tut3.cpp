#include <iostream>
#include <string>

#include "llvm/Support/Process.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

//tut3
#include "clang/Frontend/PreprocessorOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/HeaderSearchOptions.h" 
#include "clang/Frontend/Utils.h"


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

    //tut3
    HeaderSearchOptions HSOpts;
    HSOpts.AddPath("/usr/include/linux/", frontend::System, false, false, true); // for stddef.h
    HSOpts.AddPath("/usr/lib/gcc/i686-pc-linux-gnu/4.3.4/include/", frontend::System, false, false, true); // for stdarg.h
	HeaderSearch headers(fm);

    TargetOptions tarOptions;
    tarOptions.Triple = LLVM_HOSTTRIPLE;
	TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, tarOptions);

    //tut3
    ApplyHeaderSearchOptions( headers, HSOpts, lang, ti->getTriple());

	Preprocessor pp(diag, lang, *ti, sm, headers);



//tut2---------------------------------------------------------------
    const FileEntry *file = fm.getFile("input03.c");
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
//end of tut2---------------------------------------------------------------


	return 0;
}
