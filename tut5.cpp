#include <iostream>
#include <stdio.h>
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

//tut4
#include "clang/Basic/IdentifierTable.h"
#include "clang/Parse/Parser.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Sema/CodeCompleteConsumer.h"

//tut5
#include "clang/Driver/Action.h"

#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/HeaderSearch.h"



using namespace clang;
using namespace llvm;
using namespace std;



//tut5---------------------------------------------------------------
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

std::cout << "------------------------------------------------\n";
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
            
            std::cout << "Found global user declarator " << II->getName().str() << std::endl;

            //raw_ostream& Out();
            //Out << "Found global user declarator " << II->getName();
        } else {
         
            //std::cout << ((D.getContext() == Declarator::FileContext) ? ("D.getContext() == Declarator::FileContext"):("D.getContext() != Declarator::FileContext"))  << std::endl;
            //std::cout << ((DS.getStorageClassSpec() != DeclSpec::SCS_extern) ? ("DS.getStorageClassSpec() != DeclSpec::SCS_extern"):("DS.getStorageClassSpec() == DeclSpec::SCS_extern"))  << std::endl;
            //std::cout << ((DS.getStorageClassSpec() != DeclSpec::SCS_typedef) ? ("DS.getStorageClassSpec() != DeclSpec::SCS_typedef"):("DS.getStorageClassSpec() == DeclSpec::SCS_typedef"))  << std::endl;
            //std::cout << ((!D.isFunctionDeclarator()) ? ("!D.isFunctionDeclarator()"):("D.isFunctionDeclarator()"))  << std::endl;
            //std::cout << ((!pp.getSourceManager().isInSystemHeader(loc)) ? ("!pp.getSourceManager().isInSystemHeader(loc)"):("pp.getSourceManager().isInSystemHeader(loc)"))  << std::endl;
        }   
     
        return Sema::ActOnDeclarator(S, D); 
    }   
};
//end of tut5---------------------------------------------------------------

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
    const FileEntry *file = fm.getFile("input01.c");
    sm.createMainFileID(file);
    pp.EnterMainSourceFile();

//end of tut2---------------------------------------------------------------

//tut4---------------------------------------------------------------
    IdentifierTable tab(lang);

    SelectorTable sels;
    Builtin::Context builtins(*ti);
    unsigned size = 0;

    ASTContext ctxt(lang, sm, *ti, tab, sels, builtins, size);
    ASTConsumer consumer;
    CodeCompleteConsumer *codeCompleter;
    MySemaAction sema(pp, ctxt, consumer, false, codeCompleter);
    cout << "\nMySemaAction: \n" << &sema << endl;

    Parser p(pp, sema);
    p.ParseTranslationUnit();
    tab.PrintStats();
//end of tut4---------------------------------------------------------------

    //DeclSpec DS;
    //Declarator DeclaratorInfo(DS, Declarator::TypeNameContext);
    //sema.ActOnDeclarator(p.getCurScope(), DeclaratorInfo);

	return 0;
}
