#include "ReflectedEntityFinder.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <iostream>

using namespace clang::tooling;

static llvm::cl::OptionCategory gToolCategory{"Reflection Code Generator"};

static llvm::cl::list<std::string> gInclude("inc", llvm::cl::desc("includ paths"), llvm::cl::value_desc("path"),
                                            llvm::cl::cat(gToolCategory));

bool VerifyConfigFile(Json::Value& Value) {
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

void AddIncludePaths(ClangTool &tool)
{
    for (const auto &path : gInclude)
    {
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
    auto      OptionsParser = CommonOptionsParser::create(argc, argv, gToolCategory);
    ClangTool Tool(OptionsParser->getCompilations(), OptionsParser->getSourcePathList());

    Json::Value   Config;
    std::ifstream ConfigStream("Config.json");
    ConfigStream >> Config;
    if (!VerifyConfigFile(Config)) {
        return -1;
    }

    ReflectedEntityFinder ClassFinder{Config};
    MatchFinder           Finder;

    static DeclarationMatcher ClassMatcher =
        cxxRecordDecl(decl().bind("class"), hasAttr(attr::Annotate), isExpansionInMainFile());
    Finder.addMatcher(ClassMatcher, &ClassFinder);

    static DeclarationMatcher FieldMatcher =
        fieldDecl(decl().bind("field"), isExpansionInMainFile());
    Finder.addMatcher(FieldMatcher, &ClassFinder);

    // 增加Config的DefineMacros选项
    Json::Value Macros = Config["DefineMacros"];
    for (const auto& Value: Macros) {
        std::string Macro    = Value.asString();
        std::string MacroStr = std::string("-D") + Macro;
        Tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(MacroStr.c_str(), ArgumentInsertPosition::BEGIN));
    }

    // Config的OtherParams选项
    Json::Value ExtraParams = Config["ExtraParams"];
    for (const auto& Value: ExtraParams) {
        std::string ExtraParam = Value.asString();
        Tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(ExtraParam.c_str(), ArgumentInsertPosition::BEGIN));
    }

    // Config的Includes选项
    Json::Value Includes = Config["Includes"];
    for (const auto& Value: Includes) {
        std::string Include    = Value.asString();
        std::string IncludeStr = std::string("-I") + Include;
        Tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(IncludeStr.c_str(), ArgumentInsertPosition::BEGIN));
    }
    IgnoringDiagConsumer Consumer;
    Tool.setDiagnosticConsumer(&Consumer);
    AddIncludePaths(Tool);
    return Tool.run(newFrontendActionFactory(&Finder).get());
}
