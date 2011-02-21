#include <iostream>
#include <stdio.h>
#include <string>

//tut1
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

//tut4
#include "clang/Basic/IdentifierTable.h"
#include "clang/Parse/Parser.h"
//tut6
#include "clang/Parse/ParseAST.h"
#include "clang/AST/Decl.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Sema/CodeCompleteConsumer.h"

//tut5
#include "clang/Driver/Action.h"

#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/HeaderSearch.h"

//#define TUT_05
using namespace clang;
using namespace llvm;
using namespace std;



//---------------------------------------------------------------//
// For tutorial 5 - Find the global user declarator                                               
//---------------------------------------------------------------//

class MySemaAction : public Sema {
    const Preprocessor& pp; 
public:
    MySemaAction(Preprocessor &prep, ASTContext &ctxt, ASTConsumer &consumer,
        bool CompleteTranslationUnit = true,
        CodeCompleteConsumer *CompletionConsumer = 0)
        : Sema(prep, ctxt, consumer, CompleteTranslationUnit, CompletionConsumer),
          pp(prep) {}
    
    using Sema::ActOnDeclarator;
    Decl *ActOnDeclarator(Scope *S, Declarator &D) {

        // Print names of global variables. Differentiating between
        // global variables and global functions is Hard in C, so this
        // is only an approximation.
     
        const DeclSpec& DS = D.getDeclSpec();
        SourceLocation loc = D.getIdentifierLoc();
     
        if (
            // Only global declarations...
            D.getContext() == Declarator::FileContext
     
            // ...that aren't typedefs or `extern` declarations...
            && DS.getStorageClassSpec() != DeclSpec::SCS_extern
            && DS.getStorageClassSpec() != DeclSpec::SCS_typedef
     
            // ...and no functions...
            && !D.isFunctionDeclarator()
     
            // ...and in a user header
            && !pp.getSourceManager().isInSystemHeader(loc)
            ) { 
            IdentifierInfo *II = D.getIdentifier();
            
            std::cout << "Found global user declarator " << II->getName().str()
                      << std::endl;

            //raw_ostream& Out();
            //Out << "Found global user declarator " << II->getName();
        }

        return Sema::ActOnDeclarator(S, D); 
    }   
};

//---------------------------------------------------------------//
// For tutorial 6 - Read the top-level variable declaration                                               
//---------------------------------------------------------------//
class MyASTConsumer : public ASTConsumer {
public:
	virtual void HandleTopLevelDecl(DeclGroupRef D) {
		static int count = 0;
		DeclGroupRef::iterator it;
		for(it = D.begin(); it != D.end(); it++) {
			count++;
			VarDecl *VD = dyn_cast<VarDecl>(*it);
			if(!VD) continue;
			std::cout << VD << std::endl;
			if(VD->isFileVarDecl() &&
			   VD->getStorageClass() != SC_Extern) {
				std::cout << "Read top-level variable decl: '" 
                          << VD->getDeclName().getAsString() << "'\n";
			}
		}
	}
};

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
//---------------------------------------------------------------//
// For tutorial 3 - Include files                                               
//---------------------------------------------------------------//
    HeaderSearchOptions HSOpts;
    // #include <stddef.h>
    HSOpts.AddPath("/usr/include/linux/", frontend::System, false, false, true); 
    // #include <stdarg.h>
    HSOpts.AddPath("/usr/lib/gcc/i686-pc-linux-gnu/4.3.4/include/", 
                    frontend::System, false, false, true); 
    ApplyHeaderSearchOptions(headers, HSOpts, lang, ti->getTriple());
//---------------------------------------------------------------//

    IdentifierTable tab(lang);

	Preprocessor pp(diag, lang, *ti, sm, headers, tab.getExternalIdentifierLookup(), false);
	Preprocessor pp2(diag, lang, *ti, sm, headers); // why could this work?

//---------------------------------------------------------------//
// For tutorial 2 - Processing a file                                               
//---------------------------------------------------------------//
    const FileEntry *file = fm.getFile("input04.c");
    sm.createMainFileID(file);
#ifndef TUT_06
    pp.EnterMainSourceFile();
#endif

#ifdef TUT_02
    std::cout << "------------------------- The output of tutorial 2 ";
    std::cout << "-------------------------" << std::endl;
    Token Tok;
    do {
        pp.Lex(Tok);
        if(diag.hasErrorOccurred())
            break;
        pp.DumpToken(Tok);
        std::cerr << std::endl;
    } while(Tok.isNot(tok::eof));
#endif

//---------------------------------------------------------------//
// For tutorial 4 - Parsing the file
//---------------------------------------------------------------//

    SelectorTable sels;
    Builtin::Context builtins(*ti);
    unsigned size = 0;

    ASTContext ctxt(lang, sm, *ti, tab, sels, builtins, size);

    MyASTConsumer consumer;
    CodeCompleteConsumer *codeCompleter;
#if defined TUT_05 || defined TUT_06
    MySemaAction sema(pp, ctxt, consumer, false, codeCompleter);
#else
    Sema sema(pp, ctxt, consumer, false, codeCompleter);
#endif
#ifndef TUT_06
    Parser p(pp, sema);

    // The callback, ActOnDeclarator(...), defined in Tutorial05 will 
    // be called when parsing translation units.
    p.ParseTranslationUnit(); 
    std::cout << "------------------------- The output of tutorial 4 ";
    std::cout << "-------------------------" << std::endl;
    tab.PrintStats();
#endif

//---------------------------------------------------------------//
// For tutorial 6 - Read the top-level variable declaration                                               
//---------------------------------------------------------------//
#ifdef TUT_06
	ParseAST(pp, &consumer, ctxt, false, true, codeCompleter);
#endif

	return 0;
}
