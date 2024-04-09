#include <iostream>
#include <unordered_map>
#include <thread>

#include "loader.h"
#include "package.h"
#include "version_parser.h"
#include "threadpool.h"




int main(int argc, char* argv[])
{
	std::string answer;
	//std::cin >> answer;
	/*std::string root_dir = "";
	if (argc >= 2) {
		root_dir = argv[1];
	}
	std::cout << argv[1] << std::endl;*/

	//thread_pool pool;
	//pool.add_work_to_pool([]() {std::cout << "yooo" << std::endl; });
		
	//auto id1 = std::thread([] {std::cout << std::this_thread::get_id() << std::endl; });
	//auto id2 = std::thread([] {std::cout << std::this_thread::get_id() << std::endl; });
	//auto id3 = std::thread([] {std::cout << std::this_thread::get_id() << std::endl; });
	//auto id4 = std::thread([] {std::cout << std::this_thread::get_id() << std::endl; });
	//auto id5 = std::thread([] {std::cout << std::this_thread::get_id() << std::endl; });
	//
	//id1.join();
	//id2.join();
	//id3.join();
	//id4.join();
	//id5.join();


	//thread_pool tp;
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!1" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!2" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!3" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!4" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!5" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!6" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!7" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!8" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!9" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!10" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!11" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!12" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!13" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!14" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!15" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!16" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!17" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!18" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!19" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!20" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!21" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!22" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!23" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!24" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!25" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!26" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!27" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!28" << std::endl; });
	//tp.add_work_to_queue([] {std::cout << "Yoo this is a test!!29" << std::endl; });
	//
	//tp.join();
	//std::cout << "SDFGIOSDG";

	/*std::vector<package> all_packages;
	all_packages.reserve(100);

	all_packages.emplace_back(package("openexr", "1.2.0", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("openexr", "1.2.1", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("openexr", "1.2.2", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("openexr", "1.2.3", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("openexr", "1.3.0", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("openexr", "1.4.0", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("openexr", "1.4.1", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("mlc", "1.2.0", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("mlc", "2.1.4", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("mlc", "2.2.2", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("mlc", "4.4.1", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("libtesting", "1.1.4", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("libtesting", "2.2.0", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("libtesting", "2.3.0", std::filesystem::path("/some/path/now")));
	all_packages.emplace_back(package("libtesting", "2.4.0", std::filesystem::path("/some/path/now")));

	typedef package p;

	package p1("openexr", "4.4.2", std::filesystem::path("/some/path/now"));
	package p2("openexr", "4.5.1", std::filesystem::path("/some/path/now"));

	all_packages[0].add_dependency(all_packages[7]);
	all_packages[0].add_dependency(all_packages[9]);
	all_packages[0].add_dependency(all_packages[13]);
	all_packages[14].add_dependency(all_packages[0]);
	p1.add_dependency(all_packages[5]);
	p1.add_dependency(all_packages[10]);
	p1.add_dependency(all_packages[14]);


	std::cout << (p1 > p2) << "\n";
	std::cout << (p1 >= p2) << "\n";

	std::cout << (p1 < p2) << "\n";
	std::cout << (p1 <= p2) << "\n";


	auto deps = p1.get_all_dependencies();

	std::unordered_map<std::string, package*> package_map;
	package_map[p1.get_name() + "-" + p1.get_version_string()] = &p1;
	package_map[p2.get_name() + "-" + p2.get_version_string()] = &p2;
	
	std::string v1 = "1.4.4";
	std::string v2 = "^1.4.5";
	std::string v3 = "2.11.4";
	std::string v4 = "~2.11.4";

	auto a1 = has_major_restriction(v1);
	auto a2 = has_major_restriction(v2);
	auto a3 = has_minor_restriction(v3);
	auto a4 = has_minor_restriction(v4);

	std::string v5 = "1.2.44";
	std::string v6 = "^^1.2.44";
	std::string v7 = "^1.2.44 ||";
	std::string v8 = "^1.2.44 ||  1.4.5";

	auto b1 = is_valid_version_string(v5);
	auto b2 = is_valid_version_string(v6);
	auto b3 = is_valid_version_string(v7);
	auto b4 = is_valid_version_string(v8);*/


//#ifdef windows
	//auto root_dir2 = "C:\\dev\\tmp\\packages";
	auto root_dir2 = "C:\\dev\\tmp_no_copies\\packages";
#ifdef __linux__
	auto root_dir2 = "/tmp/packages";
#else
	//auto root_dir2 = "C:\\dev\\tmp_heavy\\packages";
#endif
//#else
	//auto root_dir2 = "/tmp/packages";
//#endif
	
	loader l(root_dir2);
	l.load_all_packages();
	//auto pags = l.get_all_packages();
	//auto pags2 = l.get_all_packages();
	//auto pags3 = l.get_all_packages();
	//auto pags4 = l.get_all_packages();
	//auto pags5 = l.get_all_packages();
	//auto asteroid_1_70_92 = l.get_package("asteroid", "1.70.92");

	//auto pkgs = l.get_all_packages();
	//auto pkgs2 = l.get_all_packages();
	//auto pkgs3 = l.get_all_packages();
	//auto pkgs4 = l.get_all_packages();
	//auto pkgs5 = l.get_all_packages();

	//std::string name = (*pkgs[10])._name;
	//auto version = pkgs[10]->get_version_string();
	//auto deps = pkgs[10]->get_all_dependencies();
	auto size = sizeof(package);
	std::cout << "Size of package: " << size << std::endl;
	std::cin >> answer;
	return 0;

}
