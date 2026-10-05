#pragma once
#include <string>

namespace P2PEarthquakeAPI {

	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
	std::string GetEarthquake();
}