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
    ) : mRecord(RecordDecl), mCurrentFileID(std::move(CurrentFileID)), mConig(Config) {}

    void AddField(const FieldDecl* field) { mFields.push_back(field); }

    std::string Generate(const clang::ASTContext* context, llvm::raw_fd_ostream& os) const;

    void GenerateField(
        const clang::FieldDecl* Decl, const std::map<std::string, std::string>& AttrMap,
        llvm::raw_fd_ostream& os
    ) const;

    [[nodiscard]] std::map<std::string, std::string> GetFieldAttributes(const std::string& AttrStr
    ) const;

    [[nodiscard]] std::string GetBaseClassName() const;

    [[nodiscard]] std::string GetClassQualifiedName() const;

    [[nodiscard]] std::string GetClassName() const;

private:
    const clang::CXXRecordDecl* mRecord = nullptr;
    std::vector<const FieldDecl*> mFields;
    std::vector<const FunctionDecl*> mFunctions;
    std::string mCurrentFileID;   // 当前文件ID
    Json::Value& mConig;
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
        mConfig(Config), mOutputPath(std::move(OutputPath)) {}

    virtual void run(const MatchFinder::MatchResult& Result) override;
    virtual void onEndOfTranslationUnit() override;

protected:
    void FoundRecord(const clang::CXXRecordDecl* Decl);
    void FoundField(const clang::FieldDecl* Decl);
    void FoundEnum(const clang::EnumDecl* Decl);
    void FoundEnumConstant(const clang::EnumConstantDecl* Decl);

private:
    ASTContext* mContext          = nullptr;
    SourceManager* mSourceManager = nullptr;
    std::string mOriginalFilename;
    std::string mGeneratedFilename;
    std::vector<ClassEntity> mRecordEntities;
    std::vector<EnumEntity> mEnumEntites;
    Json::Value& mConfig;
    std::string mFileID;
    std::string mOutputPath;
};



#endif   //CODEGENERATOR_REFLECTEDENTITYFINDER_H
