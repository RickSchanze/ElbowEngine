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
    ClangTool tool(OptionsParser->getCompilations(), OptionsParser->getSourcePathList());

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

    ReflectedEntityFinder class_finder{Config, static_cast<std::string>(gOutputPath)};
    MatchFinder finder;

    static DeclarationMatcher class_matcher =
        cxxRecordDecl(decl().bind("class"), hasAttr(attr::Annotate), isExpansionInMainFile());
    finder.addMatcher(class_matcher, &class_finder);

    static DeclarationMatcher field_matcher =
        fieldDecl(decl().bind("field"), isExpansionInMainFile());
    finder.addMatcher(field_matcher, &class_finder);

    static DeclarationMatcher enum_matcher = enumDecl(decl().bind("enum"), isExpansionInMainFile());
    finder.addMatcher(enum_matcher, &class_finder);

    static DeclarationMatcher enum_contant_matcher =
        enumConstantDecl(decl().bind("enum_constant"), isExpansionInMainFile());
    finder.addMatcher(enum_contant_matcher, &class_finder);

    static DeclarationMatcher function_matcher =
        functionDecl(decl().bind("function"), isExpansionInMainFile());
    finder.addMatcher(function_matcher, &class_finder);

    // 增加Config的DefineMacros选项
    for (Json::Value macros = Config["DefineMacros"]; const auto& value: macros) {
        std::string macro    = value.asString();
        std::string macro_str = std::string("-D") + macro;
        tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(macro_str.c_str(), ArgumentInsertPosition::BEGIN)
        );
    }

    // Config的OtherParams选项
    for (Json::Value extra_params = Config["ExtraParams"]; const auto& value: extra_params) {
        std::string extra_param = value.asString();
        tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(extra_param.c_str(), ArgumentInsertPosition::BEGIN)
        );
    }

    // Config的Includes选项
    for (Json::Value includes = Config["Includes"]; const auto& value: includes) {
        std::string include    = value.asString();
        std::string include_str = std::string("-I") + include;
        tool.appendArgumentsAdjuster(
            getInsertArgumentAdjuster(include_str.c_str(), ArgumentInsertPosition::BEGIN)
        );
    }
    IgnoringDiagConsumer consumer;
    tool.setDiagnosticConsumer(&consumer);
    AddIncludePaths(tool);
    int rtn = tool.run(newFrontendActionFactory(&finder).get());
    std::cout << "Return code: " << rtn;
}
