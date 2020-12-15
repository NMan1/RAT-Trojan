#include <sstream>
#include <vector>

#include "requests.h"

#pragma warning disable 26444

static size_t write_call_back(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

namespace requests {
	std::string website = "https://YouWebsiteDirecotryPathHere/";

	std::string get_request(std::string url, std::string payload) {
		CURL* curl;
		CURLcode res{};
		std::string read_buffer;

		url = website + url;

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

	bool post_request(std::string url, std::string payload, std::string* read_buffer) {
		CURL* curl;
		CURLcode res{};

		curl_global_init(CURL_GLOBAL_ALL);

		url = website + url;

		payload += std::string("&version=") + VERSION;

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

			if (read_buffer) {
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, read_buffer);
			}

			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();

		if (res == CURLE_OK)
			return true;
		else
			return false;
	}

	bool post_request_file(std::string url, const std::string file_path, std::string type, std::string* read_buffer) {
		CURL* curl;
		CURLcode res{};

		curl_global_init(CURL_GLOBAL_ALL);

		url = website + url;

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			struct curl_httppost* beginPostList = NULL;
			struct curl_httppost* endPostList = NULL;

			curl_formadd(&beginPostList, &endPostList, CURLFORM_COPYNAME, type.c_str(), CURLFORM_FILE, file_path.c_str(), CURLFORM_END);

			curl_easy_setopt(curl, CURLOPT_POST, true);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, beginPostList);

			if (read_buffer) {
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, read_buffer);
			}

			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();

		if (res == CURLE_OK)
			return true;
		else
			return false;
	}

	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
		size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
		return written;
	}

	std::vector<std::pair<std::string, std::string>> sort_payload(std::string payload) {
		std::stringstream ss(payload);
		std::vector<std::string> result;
		std::vector<std::pair<std::string, std::string>> final;

		while (ss.good())
		{
			std::string substr;
			std::getline(ss, substr, '&');
			result.push_back(substr);
		}

		for (auto str : result) {
			auto index = str.find("=");
			auto key = str.substr(0, index);
			auto value = str.substr(index + 1);
			final.push_back({ key, value });
		}

		return final;
	}

	std::string post(std::string url, std::string payload, const std::string file_path, std::string type) {
		/*
			Mix of being able to upload a file and send a payload at same time
			problay a bad idea but in hindsight it was convient 
		*/

		CURL* curl;
		CURLcode res{};
		std::string read_buffer = "";

		curl_global_init(CURL_GLOBAL_ALL);

		url = website + url;

		curl = curl_easy_init();
		if (curl) {
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "cache-control: no-cache");
			headers = curl_slist_append(headers, "Content-Disposition: form-data");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			if (file_path.empty()) {
				payload += std::string("&version=") + VERSION;
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
			}

			if (!file_path.empty()) {
				struct curl_httppost* form_begin = NULL;
				struct curl_httppost* form_end = NULL;

				curl_formadd(&form_begin, &form_end, CURLFORM_COPYNAME, type.c_str(), CURLFORM_FILE, file_path.c_str(), CURLFORM_END);

				payload += std::string("&version=") + VERSION;
				for (auto pair : sort_payload(payload)) {
					curl_formadd(&form_begin, &form_end, CURLFORM_COPYNAME, pair.first.c_str(), CURLFORM_COPYCONTENTS, pair.second.c_str(), CURLFORM_END);
				}

				curl_easy_setopt(curl, CURLOPT_POST, true);
				curl_easy_setopt(curl, CURLOPT_HTTPPOST, form_begin);
			}

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();

		return read_buffer;
	}

	bool download_file(std::string url, const std::string file_save_path) {
		CURL* curl;
		CURLcode res{};
		FILE* page_file;

		curl_global_init(CURL_GLOBAL_ALL);

		url = website + url;

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

			/* open the file */
			page_file = fopen(file_save_path.c_str(), "wb");
			if (page_file) {

				curl_easy_setopt(curl, CURLOPT_WRITEDATA, page_file);
				res = curl_easy_perform(curl);
				fclose(page_file);
			}
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();

		if (res == CURLE_OK)
			return true;
		else
			return false;
	}

}