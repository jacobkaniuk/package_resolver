#include "loader.h"
#include "config.h"
#include "threadpool.h"

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
