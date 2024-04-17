#include "parser.h"
#include <iostream>
#include <sstream>

namespace parser::version {

bool has_range(const std::string& version_expression) {
	if (is_valid_version_expression(version_expression)) {
		auto version_expression_tokens = split_version_expression(version_expression);
		return version_expression_tokens.size() > 1 && (
			std::count(version_expression_tokens.begin(), version_expression_tokens.end(), std::string{LESS_THAN}) >= 1 ||
			std::count(version_expression_tokens.begin(), version_expression_tokens.end(), std::string{ GREATER_THAN }) >= 1 ||
			std::count(version_expression_tokens.begin(), version_expression_tokens.end(), LESS_THAN_EQ) >= 1 ||
			std::count(version_expression_tokens.begin(), version_expression_tokens.end(), GREATER_THAN_EQ) >= 1 ||
			std::count(version_expression_tokens.begin(), version_expression_tokens.end(), LOGICAL_OR) >= 1
			) 
			&& !has_major_restriction(version_expression) && !has_minor_restriction(version_expression);
	}
}

bool has_minor_restriction(const std::string& version_expression)
{
	return version_expression[0] == MINOR_RESTRICTION;
}

bool has_major_restriction(const std::string& version_expression)
{
	return version_expression[0] == MAJOR_RESTRICTION;
}

std::string eval_version_from_version_expression_token(const std::string& version_expression_token) {
	if (has_major_restriction) {
		auto major = std::to_string(get_major_version(version_expression_token));
		auto minor = std::to_string(get_minor_version(version_expression_token));
		auto patch = std::to_string(get_patch_version(version_expression_token));
		std::ostringstream ss;
		ss << major << '.' << minor << '.' << patch;
		return ss.str();
	}
}

std::vector<std::string> split_version(const std::string& version) {
	std::vector<std::string> version_tokens;
	std::string token = "";
	for (const auto& c : version) {
		if (c == MAJOR_RESTRICTION || c == MINOR_RESTRICTION) {
			continue;
		}
		if (c == '.') {
			version_tokens.push_back(std::string(token));
			token = "";
		}
		else {
			token += c;
		}
	}
	version_tokens.push_back(std::string(token));
	if (version_tokens.size() == 1) {
		version_tokens.push_back("0");
		version_tokens.push_back("0");
	}
	else if (version_tokens.size() == 2) {
		version_tokens.push_back("0");
	}
	return version_tokens;
}

int get_major_version(const std::string& version)
{
	std::string _version = version;
	if (version[0] == MAJOR_RESTRICTION || version[0] == MINOR_RESTRICTION) {
		_version = version.substr(1);
	}

	int sep_count = std::count(_version.begin(), _version.end(), '.');
	if (sep_count == 0) {
		return std::stoi(_version);
	}
	
	return std::stoi(_version.substr(0, _version.find('.')));
}

int get_minor_version(const std::string& version)
{
	int sep_count = std::count(version.begin(), version.end(), '.');
	if (sep_count <= 0) {
		return 0;
	}
	else {
		return std::stoi(version.substr(version.find('.') + 1));
	}
}

int get_patch_version(const std::string& version)
{
	int sep_count = std::count(version.begin(), version.end(), '.');
	if (sep_count <= 0) {
		return 0;
	}
	else {
		return std::stoi(version.substr(version.find('.') + 1));
	}
}

std::vector<std::string> split_version_expression(const std::string& version_expression)
{
	std::vector<std::string> version_expression_tokens;
	std::string expression = "";
	char prev_char = ';';
	for (const auto c : version_expression) {
		if (c == ' ' and prev_char != ' ') {
			version_expression_tokens.push_back(expression);
			expression = "";
		}
		prev_char = c;
		if (c != ' ') {
			expression += c;
		}
	}

	if (expression != "") {
		version_expression_tokens.push_back(expression);
	}
	return version_expression_tokens;
}

std::vector<std::string> split_to_package_and_version(const std::string& package_version)
{
	std::vector<std::string> tokens;
	std::string package_name = package_version.substr(0, package_version.find('-'));
	std::string version = package_version.substr(package_version.find('-') + 1);
	tokens.push_back(package_name);
	return tokens;
}

bool is_valid_version_expression(const std::string& version_expression)
{
	auto version_expression_tokens = split_version_expression(version_expression);
	
	// check to make sure or statements is not head expression (ie. || 1.5.22 )
	if (version_expression_tokens.front() == LOGICAL_OR) {
		std::cerr << 
			"LOGICAL_OR ( || ) expression in wrong place (1st item in expression). " << version_expression <<
			". Please make sure it is found between 2 version expressions and try again." 
			<< std::endl;
		return false;
	}
	
	// check to make sure or statements is not tail expression (ie. 1.5.22 || )
	if (version_expression_tokens.back() == LOGICAL_OR) {
		std::cerr << 
			"LOGICAL_OR ( || ) expression in wrong place (last item in expression). " << version_expression <<
			". Please make sure it is found between 2 version expressions and try again." 
			<< std::endl;
		return false;
	}


	// check to make sure we don't have double or statements back to back
	std::string prev_exp = "";
	char prev_exp_char = ';';
	for (auto expression : version_expression_tokens) {
		for (auto ch : expression) {
			if (ch == prev_exp_char){
				if (ch == '.' or ch == '^' or ch == '~') {
					std::cerr <<
						"Detected duplicate token: " << ch <<
						" in expression: " << expression <<
						" for version expression: " << version_expression
						<< std::endl;

					return false;
				}
			}
			prev_exp_char = ch;
		}

		// check for back to back LOGICAL_OR ops
		if (expression == LOGICAL_OR and prev_exp == LOGICAL_OR) {
			std::cerr << 
				"Detected double LOGICAL_OR ( || ) expression in version expression. Please "
				"make sure to only use LOGICAL_OR expressions between 2 version statements and try again. " 
				<< std::endl;
			return false;
		}

		// check for multiple major restrictions in expression
		if (std::count(expression.begin(), expression.end(), MAJOR_RESTRICTION) > 1) {
			std::cerr <<
				"Detected multiple major restriction characters ( ^ ) in expression: " <<
				expression << " in version expression: " << version_expression
				<< std::endl;
			return false;
		}

		// check for multiple minor restrictions in expression
		if (std::count(expression.begin(), expression.end(), MINOR_RESTRICTION) > 1) {
			std::cerr <<
				"Detected multiple minor restriction characters ( ~ ) in expression: " <<
				expression << " in version expression: " << version_expression
				<< std::endl;
			return false;
		}

		// check for multiple minor restrictions in expression
		if (std::count(expression.begin(), expression.end(), LESS_THAN) > 1) {
			std::cerr <<
				"Detected multiple less than restriction characters ( < ) in expression: " <<
				expression << " in version expression: " << version_expression
				<< std::endl;
			return false;
		}

		// check for multiple minor restrictions in expression
		if (std::count(expression.begin(), expression.end(), GREATER_THAN) > 1) {
			std::cerr <<
				"Detected multiple minor restriction characters ( > ) in expression: " <<
				expression << " in version expression: " << version_expression
				<< std::endl;
			return false;
		}

		// check that our or op only contains 2 slahes
		if (std::count(expression.begin(), expression.end(), '|') > 2) {
			std::cerr <<
				"Detected multiple or op characters ( | ) in expression: " <<
				expression << " in version expression: " << version_expression
				<< std::endl;
			return false;
		}

		// check that our equal ops only contains 1 equal
		if (std::count(expression.begin(), expression.end(), EQUAL) > 1) {
			std::cerr <<
				"Detected multiple equal op characters ( = ) in expression: " <<
				expression << " in version expression: " << version_expression
				<< std::endl;
			return false;
		}
	}

	return true;
}


} // namespace parser::version