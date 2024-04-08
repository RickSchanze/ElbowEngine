#include "ReflectedEntityFinder.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <iostream>

using namespace clang::tooling;

static llvm::cl::OptionCategory gToolCategory{"Reflection Code Generator"};

static llvm::cl::list<std::string> gInclude(
    "inc", llvm::cl::desc("includ paths"), llvm::cl::value_desc("path"),
    llvm::cl::cat(gToolCategory)
);

static llvm::cl::opt<std::string> gOutputPath(
    "output-file-path", llvm::cl::desc("Output file path"), llvm::cl::value_desc("path"),
    llvm::cl::cat(gToolCategory), llvm::cl::Required
);

static llvm::cl::opt<std::string> gConfigPath(
    "config", llvm::cl::desc("Config file path"), llvm::cl::value_desc("path"),
    llvm::cl::cat(gToolCategory), llvm::cl::Required
);

bool VerifyConfigFile(const Json::Value& Value) {
    if (!Value.isMember("DefineMacros")) {
        std::cerr << "Config \"DefineMacros\" lacks";
        return false;
    }
    if (!Value.isMember("ExtraParams")) {
        std::cerr << "Config \"ExtraParams\" lacks";
        return false;
    }
    return true;
}

void AddIncludePaths(ClangTool& tool) {
    for (const auto& path: gInclude) {
        std::string path_command = std::string("-I") + path;
        tool.appendArgumentsAdjuster(getInsertArgumentAdjuster(path_command.c_str()));
    }
}

int main(int argc, const char** argv) {
    using namespace llvm::cl;
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace ast_matchers;
    auto OptionsParser = CommonOptionsParser::create(argc, argv, gToolCategory);
    ClangTool Tool(OptionsParser->getCompilations(), OptionsParser->getSourcePathList());

    Json::Value Config;
    try {
        std::ifstream ConfigStream;
        ConfigStream.open(gConfigPath, std::ios::in);
        ConfigStream >> Config;
        if (!VerifyConfigFile(Config)) {
            return -1;
        }
    } catch (const std::exception& e) {
        std::cout << e.what();
        return 0;
    }

    ReflectedEntityFinder ClassFinder{Config, gOutputPath};
    MatchFinder Finder;

    static DeclarationMatcher ClassMatcher =
        cxxRecordDecl(decl().bind("class"), hasAttr(attr::Annotate), isExpansionInMainFile());
    Finder.addMatcher(ClassMatcher, &ClassFinder);

    static DeclarationMatcher FieldMatcher =
        fieldDecl(decl().bind("field"), isExpansionInMainFile());
    Finder.addMatcher(FieldMatcher, &ClassFinder);

    static DeclarationMatcher EnumMatcher = enumDecl(decl().bind("enum"), isExpansionInMainFile());
    Finder.addMatcher(EnumMatcher, &ClassFinder);

    static DeclarationMatcher EnumContantMatcher =
        enumConstantDecl(decl().bind("enum_constant"), isExpansionInMainFile());
    Finder.addMatcher(EnumContantMatcher, &ClassFinder);

    // 增加Config的DefineMacros选项
    for (Json::Value Macros = Config["DefineMacros"]; const auto& Value: Macros) {
        std::string Macro    = Value.asString();
        std::string MacroStr = std::string("-D") + Macro;
        Tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(MacroStr.c_str(), ArgumentInsertPosition::BEGIN)
        );
    }

    // Config的OtherParams选项
    for (Json::Value ExtraParams = Config["ExtraParams"]; const auto& Value: ExtraParams) {
        std::string ExtraParam = Value.asString();
        Tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(ExtraParam.c_str(), ArgumentInsertPosition::BEGIN)
        );
    }

    // Config的Includes选项
    for (Json::Value Includes = Config["Includes"]; const auto& Value: Includes) {
        std::string Include    = Value.asString();
        std::string IncludeStr = std::string("-I") + Include;
        Tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(IncludeStr.c_str(), ArgumentInsertPosition::BEGIN)
        );
    }
    IgnoringDiagConsumer Consumer;
    Tool.setDiagnosticConsumer(&Consumer);
    AddIncludePaths(Tool);
    int rtn = Tool.run(newFrontendActionFactory(&Finder).get());
    std::cout << "Return code: " << rtn;
}


