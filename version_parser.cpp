#include "version_parser.h"
#include <iostream>

bool has_minor_restriction(std::string& version)
{
	return version[0] == MINOR_RESTRICTION;
}

bool has_major_restriction(std::string& version)
{
	return version[0] == MAJOR_RESTRICTION;
}

bool is_valid_version_string(std::string& version)
{
	std::vector<std::string> version_expressions;
	std::string expression = "";
	char prev_char = ';';
	for (auto c : version) {
		if (c == ' ' and prev_char != ' ') {
			version_expressions.push_back(expression);
			expression = "";
		}
		prev_char = c;
		if (c != ' ') {
			expression += c;
		}
	}

	if (expression != "") {
		version_expressions.push_back(expression);
	}
	
	// check to make sure or statements is not head expression (ie. || 1.5.22 )
	if (version_expressions.front() == OR) {
		std::cerr << 
			"OR ( || ) expression in wrong place (1st item in expression). " << version <<
			". Please make sure it is found between 2 version expressions and try again." 
			<< std::endl;
		return false;
	}
	
	// check to make sure or statements is not tail expression (ie. 1.5.22 || )
	if (version_expressions.back() == OR) {
		std::cerr << 
			"OR ( || ) expression in wrong place (last item in expression). " << version <<
			". Please make sure it is found between 2 version expressions and try again." 
			<< std::endl;
		return false;
	}


	// check to make sure we don't have double or statements back to back
	std::string prev_exp = "";
	char prev_exp_char = ';';
	for (auto expression : version_expressions) {
		for (auto ch : expression) {
			if (ch == prev_exp_char){
				if (ch == '.' or ch == '^' or ch == '~') {
					std::cerr <<
						"Detected duplicate token: " << ch <<
						" in expression: " << expression <<
						" for version string: " << version
						<< std::endl;

					return false;
				}
			}
			prev_exp_char = ch;
		}

		// check for back to back OR ops
		if (expression == OR and prev_exp == OR) {
			std::cerr << 
				"Detected double OR ( || ) expression in version string. Please "
				"make sure to only use OR expressions between 2 version statements and try again. " 
				<< std::endl;
			return false;
		}

		// check for multiple major restrictions in expression
		if (std::count(expression.begin(), expression.end(), MAJOR_RESTRICTION) > 1) {
			std::cerr <<
				"Detected multiple major restriction characters ( ^ ) in expression: " <<
				expression << " in version string: " << version 
				<< std::endl;
			return false;
		}

		// check for multiple minor restrictions in expression
		if (std::count(expression.begin(), expression.end(), MINOR_RESTRICTION) > 1) {
			std::cerr <<
				"Detected multiple minor restriction characters ( ~ ) in expression: " <<
				expression << " in version string: " << version
				<< std::endl;
			return false;
		}

		// check for multiple minor restrictions in expression
		if (std::count(expression.begin(), expression.end(), LESS_THAN) > 1) {
			std::cerr <<
				"Detected multiple less than restriction characters ( < ) in expression: " <<
				expression << " in version string: " << version
				<< std::endl;
			return false;
		}

		// check for multiple minor restrictions in expression
		if (std::count(expression.begin(), expression.end(), GREATER_THAN) > 1) {
			std::cerr <<
				"Detected multiple minor restriction characters ( > ) in expression: " <<
				expression << " in version string: " << version
				<< std::endl;
			return false;
		}

		// check that our or op only contains 2 slahes
		if (std::count(expression.begin(), expression.end(), '|') > 2) {
			std::cerr <<
				"Detected multiple or op characters ( | ) in expression: " <<
				expression << " in version string: " << version
				<< std::endl;
			return false;
		}

		// check that our equal ops only contains 1 equal
		if (std::count(expression.begin(), expression.end(), EQUAL) > 1) {
			std::cerr <<
				"Detected multiple equal op characters ( = ) in expression: " <<
				expression << " in version string: " << version
				<< std::endl;
			return false;
		}
	}

	return true;
}

std::vector<std::string> get_version_range(std::string& version)
{
	std::vector<std::string> range;
	int major;
	int minor; 
	int patch;
	// check if we're using a ~ for minor restriction
	if (has_minor_restriction(version)) {
		
	}

	return range;
}
