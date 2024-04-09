#pragma once

#include <string>
#include <vector>

#define MAJOR_RESTRICTION '^'
#define MINOR_RESTRICTION '~'
#define OR "||"
#define EQUAL '='
#define LOGICAL_EQUAL "=="
#define LESS_THAN '<'
#define GREATER_THAN '<'
#define LESS_THAN_EQ "<="
#define GREATER_THAN_EQ ">="

bool is_valid_version_string(std::string& version);
bool has_minor_restriction(std::string& version);
bool has_major_restriction(std::string& version);
std::vector<std::string> get_version_range(std::string& version);
