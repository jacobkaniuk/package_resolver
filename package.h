// package_resolve.hpp : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <unordered_map>


class package {
public:
	explicit package(const std::string& name, const std::string& version, const std::filesystem::path& path, 
		std::unordered_map<std::string, std::string>&& dependencies=std::unordered_map<std::string, std::string>());
	package(const package& other);
	package(package&& other) noexcept;
	
	package& operator=(package&& other) noexcept; 
	~package();
	
	int get_major();
	int get_minor();
	int get_patch();
	
	const std::string& get_name();
	const std::string& get_version_string();
	
	std::vector<package>* get_child_dependencies();
	std::vector<package> get_all_dependencies();
	bool depends_on(package& package);
	bool has_dependency(package& package);

	void add_dependency(package& package);
	void remove_dependency(package& package);

	bool operator == (const package& other);
	bool operator != (const package& other);
	bool operator > (const package& other);
	bool operator < (const package& other);
	bool operator >= (const package& other);
	bool operator <= (const package& other);
	std::string _name;
private:
	std::string _version;
	std::filesystem::path _path;
	std::vector<package> _dependency_packages;
	std::unordered_map<std::string, std::string> _dependencies;
	int _ver_major;
	int _ver_minor;
	int _ver_patch;
	void _extract_version();
};
