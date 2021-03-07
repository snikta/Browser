#include "httpGet.h"
#define CURL_STATICLIB
#include <curl\curl.h>
#include <string>
using std::string;

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

void httpGet(string url, string& src) {
	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		/* Forcing HTTP/3 will make the connection fail if the server isn't
		   accessible over QUIC + HTTP/3 on the given host and port.
		   Consider using CURLOPT_ALTSVC instead! */
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &src);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		/*if (res != CURLE_OK) {

		}*/

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
}