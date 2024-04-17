#include <vector>
#include <algorithm>
#include <queue>

#include "package.h"


package::package(
	const std::string& name, 
	const std::string& version, 
	const std::filesystem::path& path, 
	std::unordered_map<std::string, std::string>&& dependencies) {
	_name = name;
	_version = version;
	_path = path;
	_dependencies = dependencies;
	_ver_major = -1;
	_ver_minor = -1;
	_ver_patch = -1;

	_extract_version();
}

package::package(const package& other)
{
	_dependencies = other._dependencies;
	_name = other._name;
	_version = other._version;
	_path = other._path;
	_ver_major = other._ver_major;
	_ver_minor = other._ver_minor;
	_ver_patch = other._ver_patch;
}

package::package(package&& other) noexcept { 
	_dependencies = std::move(other._dependencies);
	_name = other._name;
	_version = other._version;
	_path = other._path;
	_ver_major = other._ver_major;
	_ver_minor = other._ver_minor;
	_ver_patch = other._ver_patch;
}

package& package::operator=(package&& other) noexcept {
	_dependencies = std::move(other._dependencies);
	_name = other._name;
	_version = other._version;
	_path = other._path;
	_ver_major = other._ver_major;
	_ver_minor = other._ver_minor;
	_ver_patch = other._ver_patch;
	return *this;
}

package::~package()
{
}

int package::get_major()
{
	return _ver_major;
}

int package::get_minor()
{
	return _ver_minor;
}

int package::get_patch()
{
	return _ver_patch;
}

const std::string& package::get_name()
{
	return _name;
}

const std::string& package::get_version_string()
{
	return _version;
}

std::vector<package>* package::get_child_dependencies()
{
	return &_dependency_packages;
}

std::vector<package> package::get_all_dependencies()
{
	std::vector<package> all_package_dependencies;
	std::queue<package> dependency_queue;
	for (auto& p : _dependency_packages) {
		dependency_queue.push(p);
	}

	while (!dependency_queue.empty()) {
		auto child_package = dependency_queue.front();
		for (auto& descendent : child_package._dependency_packages) {
			dependency_queue.push(descendent);
		}
		all_package_dependencies.push_back(child_package);
		dependency_queue.pop();
	}

	return all_package_dependencies;
}

inline void package::_extract_version()
{
	std::istringstream versionStream(_version);
	char version_separator;

	if (!(versionStream >> _ver_major >> version_separator >> _ver_minor >> version_separator >> _ver_patch)) {
		std::cerr << "Error parsing version string: " << _version << std::endl;
	}
}

bool package::depends_on(package& package)
{
	return std::find(
		package.get_child_dependencies()->begin(), 
		package.get_child_dependencies()->end(),
		*this) != package.get_child_dependencies()->end();
}

bool package::has_dependency(package& package)
{
	return std::find(
		_dependency_packages.begin(), 
		_dependency_packages.end(), 
		package) != _dependency_packages.end();
}

void package::add_dependency(package& package)
{
	if (!has_dependency(package)) {
		_dependency_packages.push_back(package);
	}
}

void package::remove_dependency(package& package)
{
	if (has_dependency(package)) {
		auto idx = std::find(
			_dependency_packages.begin(), 
			_dependency_packages.end(), package);
		
		_dependency_packages.erase(idx);
	}
}

bool package::operator==(const package& other)
{
	return (other._name == _name and
			other._version == _version and
			other._path == _path
		);
}

bool package::operator!=(const package& other)
{
	return !(*this == other);
}

bool package::operator>(const package& other)
{
	if (_ver_major > other._ver_major) return true;
	else if (_ver_major == other._ver_major) {
		if (_ver_minor > other._ver_minor) return true;
		else if (_ver_minor == other._ver_minor) {
			if (_ver_patch > other._ver_patch) return true;
		}
	}
	return false;
}

bool package::operator<(const package& other)
{
	if (_ver_major < other._ver_major) return true;
	else if (_ver_major == other._ver_major) {
		if (_ver_minor < other._ver_minor) return true;
		else if (_ver_minor == other._ver_minor) {
			if (_ver_patch < other._ver_patch) return true;
		}
	}
	return false;
}

bool package::operator>=(const package& other)
{
	return (*this > other) || (*this == other);
}

bool package::operator<=(const package& other)
{
	return (*this < other) || (*this == other);
}