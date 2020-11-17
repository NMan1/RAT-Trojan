#pragma once
#include <string>
#define CURL_STATICLIB
#include <curl/curl.h>

enum class requests { SEND_MSG, CREATE_CHANNEL };

static size_t write_call_back(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string get_request(std::string url, std::string payload = "") {
	CURL* curl;
	CURLcode res;
	std::string read_buffer;

	if (!payload.empty())
		url += "?" + payload;

	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	} 
	return read_buffer;
}

bool post_request(std::string url, std::string payload) {
	CURL* curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	if (res == CURLE_OK)
		return true;
	else
		return false;
}

bool post_request_file(std::string url, std::string file_path) {
	CURL* curl;
	CURLcode res;

	//curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		struct curl_httppost* beginPostList = NULL;
		struct curl_httppost* endPostList = NULL;

		std::string concat1 = file_path;

		curl_formadd(&beginPostList, &endPostList, CURLFORM_COPYNAME, "image", CURLFORM_FILE, &concat1, CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_POST, true);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, beginPostList);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	if (res == CURLE_OK)
		return true;
	else
		return false;
}