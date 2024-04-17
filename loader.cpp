#include "loader.h"
#include "config.h"
#include "threadpool.h"
#include "parser.h"

#include <chrono>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <thread>
#include <unordered_map>


namespace fs = std::filesystem;

loader::loader(const fs::path& root_dir)
{
	_root_dir = root_dir;
}

loader::loader()
{
}

loader::~loader()
{
}

void loader::set_package_root(const fs::path& filepath)
{
	if (!fs::exists(filepath) or
		!fs::is_directory(filepath)) {
		std::cerr << "Couldn't set package root. Invalid filepath provided: " << filepath << std::endl;
		return;
	}
	_root_dir = filepath;
}

void loader::set_package_root(const std::string& filepath)
{
	auto fp = fs::path(filepath);
	if (!fs::exists(fp) or
		!fs::is_directory(fp))	{
		std::cerr << "Couldn't set package root. Invalid filepath provided: " << filepath << std::endl;
		return;
	}
	_root_dir = fp;
}

package* loader::get_package(const std::string& name, const std::string& version)
{
	if (_package_map.count(name) <= 0) {
		return nullptr;
	}
	return _package_map.find(name)->second.find(version)->second.get();
}

package* loader::get_package(const std::string& versioned_name)
{
	if (versioned_name.find('-') == std::string::npos) {
		return nullptr;
	}
	auto name = versioned_name.substr(0, versioned_name.find('-'));
	auto version = versioned_name.substr(versioned_name.find('-') + 1);
	return _package_map.find(name)->second.find(version)->second.get();
}

bool loader::package_exists(const std::string& package_name, const std::string& version)
{
	auto count = _package_map.count(package_name);
	if (count <= 0) {
		return false;
	}
	auto version_count = _package_map[package_name].count(version);
	if (version_count <= 0) {
		return false;
	}
	return true;
}

bool loader::package_exists(const std::string& versioned_name)
{
	auto info = parser::version::split_to_package_and_version(versioned_name);
	if (info.size() <= 1) {
		return false;
	}
	return package_exists(info[0], info[1]);
}

bool loader::package_index_exists(const std::string& package_name)
{
	return _package_map.count(package_name) > 0;
}

fs::path loader::get_package_root()
{
	return fs::path();
}

std::vector<package*> loader::get_all_packages()
{
	std::vector<package*> all_packages;

	for (auto& p : _package_map) {
		for (auto& v : p.second) {
			all_packages.push_back(v.second.get());
		}
	}
	return all_packages;
}

std::vector<package*> loader::get_all_package_versions(const std::string& package_name)
{
	std::vector<package*> packages_versions;
	if (!package_index_exists(package_name)) {
		return packages_versions;
	}

	for (auto& p: _package_map[package_name]) {
		packages_versions.push_back(p.second.get());
	}
	return packages_versions;
}

std::vector<package*> loader::get_version_range(const std::string& package_name, const std::string& version_expression)
{
	std::vector<package*> version_range;
	// check to make sure there's a valid version expression
	if (!parser::version::is_valid_version_expression(version_expression)) {
		return version_range;
	}

	auto version_expression_tokens = parser::version::split_version_expression(version_expression);

	// if there's no range, just get the package
	if (!parser::version::has_range) {
		auto package = get_package(package_name, version_expression);
		if (package) {
			version_range.push_back(package);
		}
		return version_range;
	}

	// convert the major restriction to a minimum version first
	else if (parser::version::has_major_restriction(version_expression)) {
		auto major_version = version_expression_tokens[0];
		package cmp = package(package_name, parser::version::eval_version_from_version_expression_token(version_expression), fs::path(""));

		for (auto& p : get_all_package_versions(package_name)) {
			if (*p > cmp && p->get_major() <= cmp.get_major()) {
				version_range.push_back(p);
			}
			return version_range;
		}
	}

	// convert the minor restrction to a minimum first
	else if (parser::version::has_minor_restriction(version_expression)){
		auto minor_version = version_expression_tokens[0];
		package cmp = package(package_name, parser::version::eval_version_from_version_expression_token(version_expression), fs::path(""));
		for (auto& p : get_all_package_versions(package_name)) {
			if (*p > cmp && p->get_major() <= cmp.get_major() && p->get_minor() <= cmp.get_minor()) {
				version_range.push_back(p);
			}
		}
		return version_range;
	}

	// check to make sure we have a valid version range expression
	if (!_is_valid_version_range_expression(version_expression)) {
		return version_range;
	}
	
	//return version_range;
	std::vector<std::string> version_range_expression_tokens = parser::version::split_version_expression(version_expression);
	std::string version_lower_bound = version_range_expression_tokens.front();
	
	auto all_package_versions = get_all_package_versions(package_name);
	auto expression_operator = version_expression_tokens[1];
	auto version_upper_bound = parser::version::eval_version_from_version_expression_token(version_expression_tokens[2]);
	package lower_bound_pkg(package_name, parser::version::eval_version_from_version_expression_token(version_lower_bound), "");
	package upper_bound_pkg(package_name, parser::version::eval_version_from_version_expression_token(version_upper_bound), "");

	if ( expression_operator == std::string{ LESS_THAN } ) {	
		for (auto& v : all_package_versions) {
			if (lower_bound_pkg <= *v && *v < upper_bound_pkg) {
				version_range.push_back(v);
			}
		}
		return version_range;
	}

	else if (expression_operator == LESS_THAN_EQ) {
		for (auto& v : all_package_versions) {
			if (lower_bound_pkg <= *v && *v <= upper_bound_pkg) {
				version_range.push_back(v);
			}
		}
		return version_range;
	}

	// for now, GREATER_THAN and GREATER_THAN_EQ shoudln't be used as the syntax is not clear/does not support it
	else if (expression_operator == std::string{ GREATER_THAN }) {
		std::cerr << "Invalid operator used: GREATER_THAN (<) Please make sure the order of your version expression elements " <<
			"are in the correct order. ie. { <VERSION_LOWER_BOUND> <LESS_THAN or LESS_THAN_EQ> <VERSION_UPPER_BOUND>" << std::endl;
		return version_range;
	}
	else if (expression_operator == GREATER_THAN_EQ) {
		std::cerr << "Invalid operator used: GREATER_THAN (<) Please make sure the order of your version expression elements " <<
			"are in the correct order. ie. { <VERSION_LOWER_BOUND> <LESS_THAN or LESS_THAN_EQ> <VERSION_UPPER_BOUND>" << std::endl;
		return version_range;
	}
	else
		return version_range;
	
}

std::vector<package*> loader::get_version_range(const std::string& package_name, const std::string& from_version, const std::string& to_version)
{
	return std::vector<package*>();
}

std::vector<std::string> loader::get_verion_range_expression_tokens(const std::string& package_name, const std::string& version_range_expression)
{
	if (_is_valid_version_range_expression(version_range_expression)) {
		return std::vector<std::string>();
	}
	auto tokens = parser::version::split_to_package_and_version(version_range_expression);

	return std::vector<std::string>();
}

bool loader::_is_valid_version_range_expression(const std::string& version_range_expression)
{
	auto tokens = parser::version::split_version_expression(version_range_expression);
	std::string previous_token = tokens[0];
	std::set<std::string > expression_operator_tokens = { std::string(1, LESS_THAN), LESS_THAN_EQ, std::string(1, GREATER_THAN), GREATER_THAN_EQ, LOGICAL_OR};
	
	auto in_expression_tokens = [&expression_operator_tokens](const std::string& token) {
		return std::count(expression_operator_tokens.begin(), expression_operator_tokens.end(), token) >= 1;
	};

	// check we're not starting on an expression operator
	if (in_expression_tokens(previous_token)) {
		std::cerr << "Invalid version range expression detected: " <<  "\"" << version_range_expression << "\""
			"\tVersion range expression CANNOT start on an expression operators (<, <=, >, >=, ||, etc.), it MUST start with either a MAJOR RESTRICTION operator (^), " <<
			" a MINOR RESTRICTION operator (~), or a valid VERSION STRING ie. (2, 2.0, 2.0.5)" << std::endl;
		return false;
	}

	// check we're not ending on an expression operator
	if (in_expression_tokens(tokens.back())) {
		std::cerr << "Invalid version range expression detected: " << "\"" << version_range_expression << "\""
			"\tVersion range expression CANNOT end on an expression operators (<, <=, >, >=, ||, etc.), it MUST end with a valid VERSION STRING ie. (2, 2.0, 2.0.5)" << std::endl;
		return false;
	}

	for (auto& t : tokens) {
		// if we're currently looking at an expression token
		if ( in_expression_tokens(t) ) {
			if ( in_expression_tokens(previous_token) )
			{
				std::cerr << "Invalid version range expression detected: " << version_range_expression <<
					"All expression operators (<, <=, >, >=, ||, etc.) must be followed by a version expression, NOT another expression operator." << std::endl;
				return false;
			}
			previous_token = t;
		}
	}
	return true;
}

void loader::load_all_packages()
{
	auto start = std::chrono::high_resolution_clock::now();
	thread_pool tp;

	int pkg_count = 0;
	auto size = std::distance(fs::directory_iterator(_root_dir), fs::directory_iterator{});

	for (auto fp: fs::directory_iterator(_root_dir)) {
		if (fp.is_directory()) {
			for (auto version_fp : fs::directory_iterator(fp)) {
				int count = 0;
				for (char c : version_fp.path().string()) {
					if (c == '.') ++count;
				}

				if (count >= 2) {
					tp.add_work_to_queue(
						[this, version_fp]() mutable {this->_load_package_from_dir(version_fp); });
				}
				else {
					auto lock = std::scoped_lock<std::mutex>(_output_mutex);
					std::cerr << "Invalid package directory: " << version_fp << std::endl;
				}

				++pkg_count;
			}
		}
	}

	tp.join();

	auto end = std::chrono::high_resolution_clock::now();

//#ifdef logging
	std::stringstream ss;
	ss << "Loaded " << pkg_count;
	ss << " packages in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
	std::cout << ss.str();
//#endif
}

void loader::_load_package_from_dir(const fs::path& filepath)
{
	auto config_path = filepath / CONFIG_NAME;
	if (!fs::exists(config_path))
	{
#ifdef logging
		auto lock = std::scoped_lock<std::mutex>(_output_mutex);
		std::cerr << "Could not find appropriate " << CONFIG_NAME << " for package. Skipping..." << std::endl;
#endif
		return;
	}
	
	std::string name, version;
	umap<std::string, std::string> dependencies;
	{
		auto lock = std::scoped_lock<std::mutex>(_config_yaml_mutex);
		YAML::Node config = YAML::LoadFile(config_path.string());
		name = config["name"].as<std::string>();
		version = config["version"].as<std::string>();
		dependencies = config["depends"].as<umap<std::string, std::string>>();
	}

#ifdef logging
	{
		auto lock = std::scoped_lock<std::mutex>(_output_mutex);
		std::cout << "Loaded package: " << name << "-" << version << " at: " << filepath << std::endl;
		std::cout << "Dependencies: \n";
		for (auto d : dependencies) {
			std::cout << d.first << "-" << d.second << std::endl;
		}
		std::cout << std::endl;
	}
#endif
	auto versioned_package_name = get_qualified_name(name, version);
	{
		std::scoped_lock<std::mutex> lock(_package_list_mutex);
		// if the package doesn't exist in the map, add it
		if (_package_map.count(name) <= 0) {
			_package_map.insert_or_assign(name, umap<std::string, std::shared_ptr<package>>());
		}
		// add the package version to the map
		if (_package_map[name].count(version) <= 0) {
			_package_map[name].insert_or_assign(
				version, std::make_shared<package>(package(name, version, filepath, std::move(dependencies))));
		}
	}
}
