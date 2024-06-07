// this file is mostly just copied from https://github.com/darknight1050/SongDownloader/blob/master/src/Utils/WebUtils.cpp,
// all credit for it goes to darknight1050 unless other credit is given
#include "WebUtils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

#include "libcurl/shared/curl.h"
#include "libcurl/shared/easy.h"
#include "logging.hpp"

#include <thread>
#define TIMEOUT 10
#define USER_AGENT useragent


namespace WebUtils
{   
	//https://stackoverflow.com/a/55660581
	std::string query_encode(const std::string& s)
	{
		std::string ret;

		#define IS_BETWEEN(ch, low, high) (ch >= low && ch <= high)
		#define IS_ALPHA(ch) (IS_BETWEEN(ch, 'A', 'Z') || IS_BETWEEN(ch, 'a', 'z'))
		#define IS_DIGIT(ch) IS_BETWEEN(ch, '0', '9')
		#define IS_HEXDIG(ch) (IS_DIGIT(ch) || IS_BETWEEN(ch, 'A', 'F') || IS_BETWEEN(ch, 'a', 'f'))

		for(size_t i = 0; i < s.size();)
		{
			char ch = s[i++];

			if (IS_ALPHA(ch) || IS_DIGIT(ch))
			{
				ret += ch;
			}
			else if ((ch == '%') && IS_HEXDIG(s[i+0]) && IS_HEXDIG(s[i+1]))
			{
				ret += s.substr(i-1, 3);
				i += 2;
			}
			else
			{
				switch (ch)
				{
					case '-':
					case '.':
					case '_':
					case '~':
					case '!':
					case '$':
					case '&':
					case '\'':
					case '(':
					case ')':
					case '*':
					case '+':
					case ',':
					case ';':
					case '=':
					case ':':
					case '@':
					case '/':
					case '?':
					case '[':
					case ']':
						ret += ch;
						break;

					default:
					{
						static const char hex[] = "0123456789ABCDEF";
						char pct[] = "%  ";
						pct[1] = hex[(ch >> 4) & 0xF];
						pct[2] = hex[ch & 0xF];
						ret.append(pct, 3);
						break;
					}
				}
			}
		}

		return ret;
	}

	std::size_t CurlWrite_CallbackFunc_StdString(void *contents, std::size_t size, std::size_t nmemb, std::string *s)
	{
		std::size_t newLength = size * nmemb;
		try {
			s->append((char*)contents, newLength);
		} catch(std::bad_alloc &e) {
			//handle memory problem
			DEBUG("Failed to allocate string of size: {}", newLength);
			return 0;
		}
		return newLength;
	}
	
	std::shared_ptr<std::vector<uint8_t>> val;

	std::size_t CurlWrite_CallbackFunc_Bytes(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		auto castPtr = reinterpret_cast<uint8_t*>(ptr);
		val.get()->insert(val->end(), castPtr, castPtr + size * nmemb);
		return size * nmemb;
	}

	void GetAsync(std::string url, std::function<void(long, std::string)> finished, std::function<void(float)> progressUpdate) {
		GetAsync(url, TIMEOUT, finished, progressUpdate);
	}

	struct ProgressUpdateWrapper {
		std::function<void(float)> progressUpdate;
	};

	void GetAsync(std::string url, long timeout, std::function<void(long, std::string)> finished, std::function<void(float)> progressUpdate, std::string apiKey) {
		std::thread t (
			[url, timeout, progressUpdate, finished, apiKey] {
				std::string val;
				// Init curl
				auto* curl = curl_easy_init();
				struct curl_slist *headers = NULL;
				headers = curl_slist_append(headers, "Accept: */*");
				if (apiKey != "") {
					headers = curl_slist_append(headers, ("Authorization: " + apiKey).c_str());
				}
				
				// Set headers
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

				curl_easy_setopt(curl, CURLOPT_URL, query_encode(url).c_str());

				// Don't wait forever, time out after TIMEOUT seconds.
				curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

				// Follow HTTP redirects if necessary.
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);

				ProgressUpdateWrapper* wrapper = new ProgressUpdateWrapper { progressUpdate };
				if(progressUpdate) {
					// Internal CURL progressmeter must be disabled if we provide our own callback
					curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
					curl_easy_setopt(curl, CURLOPT_XFERINFODATA, wrapper);
					// Install the callback function
					curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, 
						+[] (void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
							float percentage = (float)dlnow / (float)dltotal * 100.0f;
							if(isnan(percentage))
								percentage = 0.0f;
							reinterpret_cast<ProgressUpdateWrapper*>(clientp)->progressUpdate(percentage);
							return 0;
						}
					);
				}

				long httpCode(0);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &val);
				curl_easy_setopt(curl, CURLOPT_USERAGENT, std::string("Nya/ImageLib").c_str());
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			
				auto res = curl_easy_perform(curl);
				/* Check for errors */ 
				if (res != CURLE_OK) {
					curl_easy_cleanup(curl);
					delete wrapper;
					return finished(0, val);
				}
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
				curl_easy_cleanup(curl);
				delete wrapper;
				finished(httpCode, val);
			}
		);
		t.detach();
	}

	void GetAsyncBytes(std::string url, long timeout, std::function<void(long, std::shared_ptr<std::vector<uint8_t>>)> finished, std::function<void(float)> progressUpdate) {
		val = std::make_shared<std::vector<uint8_t>>();
		std::thread t (
			[url, timeout, progressUpdate, finished] {
				//std::vector<uint8_t> val;
				// Init curl
				auto* curl = curl_easy_init();
				struct curl_slist *headers = NULL;
				headers = curl_slist_append(headers, "Accept: */*");
				// Set headers
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

				curl_easy_setopt(curl, CURLOPT_URL, query_encode(url).c_str());

				// Don't wait forever, time out after TIMEOUT seconds.
				curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

				// Follow HTTP redirects if necessary.
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_Bytes);

				ProgressUpdateWrapper* wrapper = new ProgressUpdateWrapper { progressUpdate };
				if(progressUpdate) {
					// Internal CURL progressmeter must be disabled if we provide our own callback
					curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
					curl_easy_setopt(curl, CURLOPT_XFERINFODATA, wrapper);
					// Install the callback function
					curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, 
						+[] (void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
							float percentage = (float)dlnow / (float)dltotal * 100.0f;
							if(isnan(percentage))
								percentage = 0.0f;
							reinterpret_cast<ProgressUpdateWrapper*>(clientp)->progressUpdate(percentage);
							return 0;
						}
					);
				}

				long httpCode(0);
				
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, val->data());
				curl_easy_setopt(curl, CURLOPT_USERAGENT, std::string("Nya/ImageLib").c_str());
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			
				auto res = curl_easy_perform(curl);
				/* Check for errors */ 
				if (res != CURLE_OK) {
					curl_easy_cleanup(curl);
					delete wrapper;
					return  finished(0, val);
				}
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
				curl_easy_cleanup(curl);
				delete wrapper;
				finished(httpCode, val);
			}
		);
		t.detach();
	}
}
