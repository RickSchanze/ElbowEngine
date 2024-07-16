/**
 * @file ReflectedEntityFinder.cpp
 * @author Echo 
 * @Date 24-4-5
 * @brief 
 */

#include "ReflectedEntityFinder.h"
#include <filesystem>
#include <iostream>
using namespace std::string_literals;

// 去掉字符串两端的空格
std::string Trim(const std::string& Str, const std::string& TrimStr = " ") {
    if (Str.empty()) {
        return Str;
    }
    std::string S = Str;
    if (S.find_first_not_of(TrimStr) != std::string::npos) {
        S.erase(0, S.find_first_not_of(TrimStr));
    }
    if (S.find_last_not_of(TrimStr) != std::string::npos) {
        S.erase(S.find_last_not_of(TrimStr) + 1);
    }
    return S;
}

bool ReplaceOne(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

bool IsSpace(std::string Str) {
    return std::ranges::all_of(Str, [](char c) { return std::isspace(c); });
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();   // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void ReflectedEntityFinder::run(const clang::ast_matchers::MatchFinder::MatchResult& Result) {
    using namespace clang;
    context_        = Result.Context;
    source_manager_ = Result.SourceManager;

    if (const auto record = Result.Nodes.getNodeAs<CXXRecordDecl>("class"))
        return FoundRecord(record);
    if (const auto field = Result.Nodes.getNodeAs<FieldDecl>("field")) return FoundField(field);
    if (const auto Enum = Result.Nodes.getNodeAs<EnumDecl>("enum")) return FoundEnum(Enum);
    if (const auto enum_constant = Result.Nodes.getNodeAs<EnumConstantDecl>("enum_constant"))
        return FoundEnumConstant(enum_constant);
    if (const auto function = Result.Nodes.getNodeAs<FunctionDecl>("function"))
        return FoundFunction(function);
}

void ReflectedEntityFinder::FoundField(const clang::FieldDecl* decl) {
    if (record_entities_.empty()) return;
    record_entities_.back().AddField(decl);
}

void ReflectedEntityFinder::FoundEnum(const clang::EnumDecl* decl) {
    original_filename_ = source_manager_->getFilename(decl->getLocation()).str();
    // 获取文件名字
    if (file_id_.empty()) {
        const std::filesystem::path FilePath = original_filename_;
        std::string FileID                   = FilePath.filename().generic_string();
        ReplaceOne(FileID, ".", "_");
        file_id_ = FileID;
    }
    // 看看Enum有没有Reflected属性
    for (auto& Attr: decl->attrs()) {
        if (Attr->getKind() == clang::attr::Annotate) {
            if (const auto* AnnotateAttr = static_cast<clang::AnnotateAttr*>(Attr);
                AnnotateAttr->getAnnotation().starts_with("Reflected")) {
                enum_entites_.emplace_back(decl, file_id_);
                break;
            }
        }
    }
}

void ReflectedEntityFinder::FoundEnumConstant(const clang::EnumConstantDecl* decl) {
    if (enum_entites_.empty()) return;
    enum_entites_.back().AddConstant(decl);
}

void ReflectedEntityFinder::FoundFunction(const clang::FunctionDecl* decl) {
    if (record_entities_.empty()) return;
    record_entities_.back().AddFunction(decl);
}

void ReflectedEntityFinder::FoundRecord(const clang::CXXRecordDecl* decl) {
    original_filename_ = source_manager_->getFilename(decl->getLocation()).str();
    // 获取文件名字
    if (file_id_.empty()) {
        const std::filesystem::path FilePath = original_filename_;
        std::string FileID                   = FilePath.filename().generic_string();
        ReplaceOne(FileID, ".", "_");
        file_id_ = FileID;
    }
    // 看看这个类有没有Reflected属性
    for (auto& Attr: decl->attrs()) {
        if (Attr->getKind() == clang::attr::Annotate) {
            if (const auto* AnnotateAttr = static_cast<clang::AnnotateAttr*>(Attr);
                AnnotateAttr->getAnnotation().starts_with("Reflected")) {
                record_entities_.emplace_back(decl, config_, file_id_);
                break;
            }
        }
    }
    if (original_filename_.ends_with(".generated.h")) {
        llvm::outs() << "Skip generated file: " << original_filename_ << "\n";
        exit(0);
    }
    if (!original_filename_.empty() && !generated_filename_.empty()) return;
    generated_filename_ = original_filename_;
    generated_filename_.replace(generated_filename_.find(".h"), 2, ".generated.h");
}

void ReflectedEntityFinder::onEndOfTranslationUnit() {
    MatchCallback::onEndOfTranslationUnit();
    if (enum_entites_.empty() && record_entities_.empty()) return;
    std::error_code ec;
    std::filesystem::path Path = generated_filename_;
    Path                       = std::filesystem::path(output_path_) / Path.filename();
    generated_filename_        = Path.string();
    llvm::raw_fd_ostream os(generated_filename_, ec);
    assert(!ec && "error opening file");
    std::cout << "Generated file: " << generated_filename_ << "\n";
    os << "// This file generated by generator, do not edit it.\n";
    std::vector<std::string> MacroStrs;
    for (auto& Ref: record_entities_) {
        MacroStrs.push_back(Ref.Generate(context_, os));
    }
    for (auto& Ref: enum_entites_) {
        MacroStrs.push_back(Ref.Generate(context_, os));
    }
    os << "#undef GENERATED_SOURCE_" << file_id_ << "\n";
    os << "#define GENERATED_SOURCE_" << file_id_ << " \\\n";
    for (const auto& MacroStr: MacroStrs) {
        os << MacroStr << " \\\n";
    }
    os << ";\n";
    os << "#undef CURRENT_FILE_ID\n";
    os << "#define CURRENT_FILE_ID " << file_id_ << "\n";
    os.close();
}

std::string ClassEntity::GetBaseClassName() const {
    if (!record_->bases().empty()) {
        const clang::CXXBaseSpecifier* BaseSpecifier = record_->bases_begin();
        const CXXRecordDecl* BaseRecordDecl = BaseSpecifier->getType()->getAsCXXRecordDecl();
        const std::string base_class_name   = BaseRecordDecl->getQualifiedNameAsString();
        return base_class_name;
    }
    return "";
}

std::string ClassEntity::GetClassQualifiedName() const {
    if (record_) {
        return record_->getQualifiedNameAsString();
    }
    return "";
}
std::string ClassEntity::GetClassName() const {
    if (record_) {
        return record_->getNameAsString();
    }
    return "";
}

void ClassEntity::GenerateField(
    const clang::FieldDecl* Decl, const std::map<std::string, std::string>& attr_map,
    llvm::raw_fd_ostream& os
) const {
    if (!attr_map.contains("Reflected") || attr_map.at("Reflected") != "True") return;
    std::string FieldDeclName       = Decl->getNameAsString();
    std::string FieldName           = FieldDeclName;
    auto QualifiedClassName         = GetClassQualifiedName();
    const std::string FieldTypeName = Decl->getType().getAsString();
    if (attr_map.contains("Name")) {
        FieldName = attr_map.at("Name");
    }
    std::string getter_name;
    if (attr_map.contains("Getter")) {
        getter_name = attr_map.at("Getter");
    }
    std::string setter_name;
    if (attr_map.contains("Setter")) {
        setter_name = attr_map.at("Setter");
    }
    std::string register_str;
    if (getter_name.empty() && setter_name.empty()) {
        register_str = std::vformat(
            R"(.property("{}", &{}::{}))",
            std::make_format_args(FieldName, QualifiedClassName, FieldDeclName)
        );
    } else if (!getter_name.empty() && !setter_name.empty()) {
        register_str = std::vformat(
            R"(.property("{}", &{}::{}, &{}::{}, &{}::{}))",
            std::make_format_args(
                FieldName,
                QualifiedClassName,
                FieldDeclName,
                QualifiedClassName,
                getter_name,
                QualifiedClassName,
                setter_name
            )
        );
    } else {
        register_str = std::vformat(
            R"(.property("{}", &{}::{}))",
            std::make_format_args(FieldName, QualifiedClassName, FieldDeclName)
        );
        std::cout << "getter and settr for property " << QualifiedClassName << "::" << FieldDeclName
                  << " must exist at the same time." << std::endl;
    }
    for (const auto& [key, value]: attr_map) {
        if (key == "Name") continue;
        if (key == "Reflected") continue;
        if (key == "Getter") continue;
        if (key == "Setter") continue;
        register_str +=
            std::vformat(R"((rttr::metadata("{}", "{}")))", std::make_format_args(key, value));
    }
    if (config_.isMember("RefWrapperType")) {
        for (auto& Value: config_["RefWrapperType"]) {
            auto WarpperStr = Value.asString();
            if (FieldTypeName.starts_with(WarpperStr)) {
                register_str += std::string("(rttr::policy::prop::as_reference_wrapper)");
                break;
            }
        }
    }
    register_str += " \\\n";
    os << register_str;
}

void ClassEntity::GenerateFunction(
    const clang::FunctionDecl* decl, const std::map<std::string, std::string>& attr_map,
    llvm::raw_fd_ostream& os
) const {
    if (!attr_map.contains("Reflected") || attr_map.at("Reflected") != "True") return;
    std::string field_decl_name = decl->getNameAsString();
    std::string field_name      = field_decl_name;
    auto qualified_class_name   = GetClassQualifiedName();
    if (attr_map.contains("Name")) {
        field_name = attr_map.at("Name");
    }
    std::string register_str = std::vformat(
        R"(.method("{}", &{}::{}))",
        std::make_format_args(field_name, qualified_class_name, field_decl_name)
    );
    for (const auto& [key, value]: attr_map) {
        if (key == "Name") continue;
        if (key == "Reflected") continue;
        register_str +=
            std::vformat(R"((rttr::metadata("{}", "{}")))", std::make_format_args(key, value));
    }
    register_str += " \\\n";
    os << register_str;
}

std::string EnumEntity::Generate(const clang::ASTContext* Context, llvm::raw_fd_ostream& OS) const {
    std::string CodeBody;
    CodeBody += "(";
    auto UnderlineEnumName = GetEnumUnderlineName();
    auto QualifiedEnumName = GetQualifiedEnumName();
    auto EnumName          = mEnumDecl->getNameAsString();
    for (const auto* ConstantDecl: mConstants) {
        auto ConstantDeclName = ConstantDecl->getNameAsString();
        CodeBody += std::vformat(
            R"(rttr::value("{}", {}::{}), )",
            std::make_format_args(ConstantDeclName, QualifiedEnumName, ConstantDeclName)
        );
    }
    // 删除最后一个","
    CodeBody.erase(CodeBody.find_last_of(','));
    CodeBody += ")";
    auto EnumMacroString = std::vformat(
        "GENERATED_ENUM_CODE_{}_{}", std::make_format_args(mCurrentFileID, UnderlineEnumName)
    );
    const std::string EnumBody = std::vformat(
        R"(
#undef {}
#define {} \
    rttr::registration::enumeration<{}>("{}") \
    {};
)",
        std::make_format_args(
            EnumMacroString, EnumMacroString, QualifiedEnumName, QualifiedEnumName, CodeBody
        )
    );
    OS << EnumBody;
    return EnumMacroString;
}

std::string EnumEntity::GetQualifiedEnumName() const {
    return mEnumDecl->getQualifiedNameAsString();
}

std::string EnumEntity::GetEnumUnderlineName() const {
    auto Name = GetQualifiedEnumName();
    ReplaceAll(Name, "::", "_");
    return Name;
}

std::string
ClassEntity::Generate(const clang::ASTContext* context, llvm::raw_fd_ostream& os) const {
    auto ClassName = GetClassName();
    auto CodeMacroStr =
        std::vformat(R"(GENERATED_CODE_{}{})", std::make_format_args(current_file_id_, ClassName));
    const std::string CodeDefineMacroStr =
        std::vformat(R"(#define {} \)", std::make_format_args(CodeMacroStr));
    auto ClassQualifiedName = GetClassQualifiedName();
    os << "#undef " << CodeMacroStr << "\n";
    os << CodeDefineMacroStr << "\n";

    const std::string RegisterClassCode = std::vformat(
        R"(rttr::registration::class_<{}>("{}").constructor<>()(rttr::policy::ctor::as_raw_ptr) \)",
        std::make_format_args(ClassQualifiedName, ClassQualifiedName)
    );
    os << RegisterClassCode << "\n";
    for (const auto& field: fields_) {
        // 寻找annoation attr
        std::string AttrStr;
        for (const auto& attr: field->attrs()) {
            if (attr->getKind() == clang::attr::Annotate) {
                const auto* AnnotateAttr = static_cast<clang::AnnotateAttr*>(attr);
                AttrStr                  = AnnotateAttr->getAnnotation();
                break;
            }
        }
        if (!AttrStr.empty()) {
            auto AttrMap = GetFieldAttributes(AttrStr);
            GenerateField(field, AttrMap, os);
        }
    }
    for (const auto& function: functions_) {
        // 寻找annoation attr
        std::string AttrStr;
        for (const auto& attr: function->attrs()) {
            if (attr->getKind() == clang::attr::Annotate) {
                const auto* AnnotateAttr = static_cast<clang::AnnotateAttr*>(attr);
                AttrStr                  = AnnotateAttr->getAnnotation();
                break;
            }
        }
        if (!AttrStr.empty()) {
            auto AttrMap = GetFieldAttributes(AttrStr);
            GenerateFunction(function, AttrMap, os);
        }
    }
    os << ";\n";
    std::string ThisClassName         = GetClassQualifiedName();
    const std::string ParentClassName = GetBaseClassName();
    std::string ParentClassNameTypedef =
        ParentClassName.empty() ? "" : "typedef "s + ParentClassName + " Super;";
    std::string RTTREnable = ParentClassName.empty() ? "" : "RTTR_ENABLE("s + ParentClassName + ")";
    // clang-format off
    const std::string RegisterGeneratedBody = std::vformat(R"(
#undef GENERATED_BODY_{}_{}
#define GENERATED_BODY_{}_{} \
public: \
    typedef {} ThisClass; \
    {} \
    {} \
    RTTR_REGISTRATION_FRIEND \
private:
)",std::make_format_args(current_file_id_, ClassName, current_file_id_, ClassName, ThisClassName, ParentClassNameTypedef , RTTREnable));
    // clang-format on
    os << RegisterGeneratedBody << "\n";
    return CodeMacroStr;
}

std::map<std::string, std::string> ClassEntity::GetFieldAttributes(const std::string& AttrStr
) const {
    std::map<std::string, std::string> AttrMap;
    std::string AttrStrCopy = AttrStr;
    // 按","分割字符串
    std::vector<std::string> Attrs;
    while (!AttrStrCopy.empty()) {
        auto Pos = AttrStrCopy.find(',');
        if (Pos == std::string::npos) {
            Attrs.push_back(AttrStrCopy);
            break;
        }
        Attrs.push_back(AttrStrCopy.substr(0, Pos));
        AttrStrCopy = AttrStrCopy.substr(Pos + 1);
    }
    for (const auto& Attr: Attrs) {
        auto Pos = Attr.find('=');
        if (Pos == std::string::npos) {
            // 去除Attr周围的空格
            std::string TrimmedAttrStr = Trim(Attr);
            if (IsSpace(TrimmedAttrStr)) continue;
            AttrMap[TrimmedAttrStr] = "True";
        } else {
            std::string Key        = Attr.substr(0, Pos);
            std::string TrimmedKey = Trim(Trim(Trim(Key, " "), "\""), "\"");
            if (IsSpace(TrimmedKey)) continue;

            std::string Value        = Attr.substr(Pos + 1);
            std::string TrimmedValue = Trim(Trim(Trim(Value, " "), "\""), "\"");
            if (IsSpace(TrimmedValue)) continue;

            AttrMap[TrimmedKey] = TrimmedValue;
        }
    }
    return AttrMap;
}
