#pragma once
#include <string>
#include <curl/curl.h>

#include "../client/client.h"

#define CURL_STATICLIB

namespace requests {
	extern std::string website;

	std::string get_request(std::string url, std::string payload = "");

	bool post_request(std::string url, std::string payload, std::string* read_buffer = nullptr);

	bool post_request_file(std::string url, const std::string file_path, std::string type = "image", std::string* read_buffer = nullptr);

	std::string post(std::string url, std::string payload = "", const std::string file_path = "", std::string type = "");

	bool download_file(std::string url, const std::string file_save_path);
}