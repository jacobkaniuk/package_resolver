#pragma once

#include "package.h"

#include <memory>
#include <filesystem>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <set>

#define umap std::unordered_map

class package;

class loader {
public:
	loader(const std::filesystem::path& root_dir);
	loader();
	~loader();

	void set_package_root(const std::filesystem::path& filepath);
	void set_package_root(const std::string& filepath);
	package* get_package(const std::string& name, const std::string& version);
	package* get_package(const std::string& versioned_name);
	bool package_exists(const std::string& package_name, const std::string& version);
	bool package_exists(const std::string& versioned_name);
	bool package_index_exists(const std::string& package_name);
	std::filesystem::path get_package_root();
	std::vector<package*> get_all_packages();
	std::vector<package*> get_all_package_versions(const std::string& package_name);
	void load_all_packages();
	static std::string get_qualified_name(const std::string& name, const std::string& version) { return name + "-" + version; };
	static std::string get_qualified_name(std::string& name, std::string& version) { return name + "-" + version; };
	std::vector<package*> get_version_range(const std::string& package_name, const std::string& version_expression);
	std::vector<package*> get_version_range(const std::string& package_name, const std::string& from_version, const std::string& to_version);
	std::vector<std::string> get_verion_range_expression_tokens(const std::string& package_name, const std::string& version_range_expression);
	std::vector<std::string> get_verion_range_expression_tokens(const std::string& version_range_expression);
	bool _is_valid_version_range_expression(const std::string& version_range_expression);
private:
	std::mutex _output_mutex;
	std::mutex _package_list_mutex;
	std::mutex _config_yaml_mutex;
	std::filesystem::path _root_dir;
	umap<std::string, umap<std::string, std::shared_ptr<package>>> _package_map;
	void _load_package_from_dir(const std::filesystem::path& filepath);
};
