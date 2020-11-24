#include "requests.h"

static size_t write_call_back(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

namespace requests {

	std::string get_request(std::string url, std::string payload) {
		CURL* curl;
		CURLcode res{};
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

	bool post_request(const std::string& url, std::string payload, std::string* read_buffer) {
		CURL* curl;
		CURLcode res{};

		curl_global_init(CURL_GLOBAL_ALL);

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

	bool post_request_file(const std::string& url, const std::string& file_path) {
		CURL* curl;
		CURLcode res{};

		curl_global_init(CURL_GLOBAL_ALL);

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			struct curl_httppost* beginPostList = NULL;
			struct curl_httppost* endPostList = NULL;

			curl_formadd(&beginPostList, &endPostList, CURLFORM_COPYNAME, "image", CURLFORM_FILE, file_path.c_str(), CURLFORM_END);

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

	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
		return written;
	}

	bool download_file(const std::string& url, const std::string& file_save_path)
	{
		CURL* curl;
		CURLcode res{};
		FILE* page_file;

		curl_global_init(CURL_GLOBAL_ALL);

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