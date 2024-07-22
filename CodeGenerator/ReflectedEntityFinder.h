/**
 * @file ReflectedEntityFinder.h
 * @author Echo 
 * @Date 24-4-5
 * @brief 
 */

#ifndef CODEGENERATOR_REFLECTEDENTITYFINDER_H
#define CODEGENERATOR_REFLECTEDENTITYFINDER_H

#include <utility>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "json/json.h"

/**
 * 需要反射的枚举类的信息
 */
class EnumEntity {
public:
    explicit EnumEntity(const clang::EnumDecl* EnumDecl, std::string CurrentFileID) :
        mEnumDecl(EnumDecl), mCurrentFileID(std::move(CurrentFileID)) {}
    void AddConstant(const clang::EnumConstantDecl* Constant) { mConstants.push_back(Constant); }
    std::string Generate(const clang::ASTContext* Context, llvm::raw_fd_ostream& OS) const;
    [[nodiscard]] std::string GetQualifiedEnumName() const;
    std::string GetEnumUnderlineName() const;

private:
    const clang::EnumDecl* mEnumDecl;
    std::vector<const clang::EnumConstantDecl*> mConstants;
    std::string mCurrentFileID;
};

/**
 * 需要反射的类的信息
 */
class ClassEntity {
    using CXXRecordDecl = clang::CXXRecordDecl;
    using FieldDecl     = clang::FieldDecl;
    using FunctionDecl  = clang::FunctionDecl;

public:
    ClassEntity() = delete;
    explicit ClassEntity(
        const clang::CXXRecordDecl* RecordDecl, Json::Value& Config, std::string CurrentFileID
    ) : record_(RecordDecl), current_file_id_(std::move(CurrentFileID)), config_(Config) {}

    void AddField(const FieldDecl* field) {
        fields_.insert(field);
    }
    void AddFunction(const FunctionDecl* function) { functions_.insert(function); }

    std::string Generate(const clang::ASTContext* context, llvm::raw_fd_ostream& os) const;

    void GenerateField(
        const clang::FieldDecl* Decl, const std::map<std::string, std::string>& attr_map,
        llvm::raw_fd_ostream& os
    ) const;

    void GenerateFunction(
        const clang::FunctionDecl* decl, const std::map<std::string, std::string>& attr_map,
        llvm::raw_fd_ostream& os
    ) const;

    [[nodiscard]] std::map<std::string, std::string> GetFieldAttributes(const std::string& AttrStr
    ) const;

    [[nodiscard]] std::vector<std::string> GetBaseClassNames() const;

    [[nodiscard]] std::string GetClassQualifiedName() const;

    [[nodiscard]] std::string GetClassName() const;

private:
    const clang::CXXRecordDecl* record_ = nullptr;
    std::set<const FieldDecl*> fields_;
    std::set<const FunctionDecl*> functions_;
    std::string current_file_id_;   // 当前文件ID
    Json::Value& config_;
};

/**
 * 寻找所有需要反射的实体
 */
class ReflectedEntityFinder final : public clang::ast_matchers::MatchFinder::MatchCallback {
    using SourceManager = clang::SourceManager;
    using ASTContext    = clang::ASTContext;
    using MatchFinder   = clang::ast_matchers::MatchFinder;

public:
    explicit ReflectedEntityFinder(Json::Value& Config, std::string OutputPath) :
        config_(Config), output_path_(std::move(OutputPath)) {}

    void run(const MatchFinder::MatchResult& Result) override;
    void onEndOfTranslationUnit() override;

protected:
    void FoundRecord(const clang::CXXRecordDecl* decl);
    void FoundField(const clang::FieldDecl* decl);
    void FoundEnum(const clang::EnumDecl* decl);
    void FoundEnumConstant(const clang::EnumConstantDecl* decl);
    void FoundFunction(const clang::FunctionDecl* decl);

private:
    ASTContext* context_           = nullptr;
    SourceManager* source_manager_ = nullptr;
    std::string original_filename_;
    std::string generated_filename_;
    std::vector<ClassEntity> record_entities_;
    std::vector<EnumEntity> enum_entites_;
    Json::Value& config_;
    std::string file_id_;
    std::string output_path_;
};



#endif   //CODEGENERATOR_REFLECTEDENTITYFINDER_H
