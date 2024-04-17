#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace parser::version {

#define MAJOR_RESTRICTION '^'
#define MINOR_RESTRICTION '~'
#define EQUAL '='
#define LESS_THAN '<'
#define GREATER_THAN '<'
#define LESS_THAN_EQ "<="
#define GREATER_THAN_EQ ">="
#define LOGICAL_AND "&&"
#define LOGICAL_EQUAL "=="
#define LOGICAL_OR "||"
#define LOGICAL_NOT "!"
#define LOGICAL_NOT_EQUAL "!="

std::vector<std::string> split_version_expression(const std::string& version_expression);
std::vector<std::string> split_to_package_and_version(const std::string& package_version);
bool has_range(const std::string& version_expression);
bool is_valid_version_expression(const std::string& version_expression);
bool has_minor_restriction(const std::string& version_expression);
bool has_major_restriction(const std::string& version_expression);
int get_major_version(const std::string& version);
int get_minor_version(const std::string& version);
int get_patch_version(const std::string& version);
std::string eval_version_from_version_expression_token(const std::string& version_expression_token);
std::vector<std::string> split_version(const std::string& version);

} // namespace parser::version