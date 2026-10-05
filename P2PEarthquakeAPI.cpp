#include "P2PEarthquakeAPI.h"
#include <curl/curl.h>
#include <string>

size_t P2PEarthquakeAPI::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string P2PEarthquakeAPI::GetEarthquake() {
    CURL* curl = nullptr;
    CURLcode res = {};
    std::string readBuffer = "";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.p2pquake.net/v2/history?codes=551");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return readBuffer;
    }
    return "";
}
