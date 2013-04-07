#include "stdafx.h"
#include "YouTube.h"

#include <openssl\bio.h>
#include <openssl\ssl.h>
#include <openssl\err.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Definitions
const int MAXBUF = 8192;
const std::string newline = "\r\n";
const std::string Host = "www.youtube.com";
const std::string Port = "80";
const std::string UserAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:19.0) Gecko/20100101 Firefox/19.0";

// Data Structures
struct FMTstream {
	std::string	fallback_host, itag, quality, sig, type, url;
};

// Private Declarations
std::string DownloadVideoInformation (const std::string & videoID);
std::string getFMTstream (const std::string & feed);
std::vector <FMTstream> parseFMTstream (const std::string & serial);
std::string decodeURLstring (const std::string & url);

std::string YouTube::GetDownloadLink (const std::string & videoID, std::string option1, std::string option2, std::string option3)
{
	std::string response = DownloadVideoInformation(videoID);
	if (response.empty()) {
		return std::string();
	}

	response = getFMTstream(response);
	if (response.empty()) {
		return std::string();
	}

	response = decodeURLstring (response);

	std::vector <FMTstream> links = parseFMTstream(response);
	if (links.empty()) {
		return std::string();
	}

	// Option 1
	for (unsigned int option = 0; option < links.size(); option++)
	{
		if (links.at(option).itag == option1) {
			std::string link = links.at(option).url + "&signature=" + links.at(option).sig;
			return link;
		}
	}

	// Option 2
	for (unsigned int option = 0; option < links.size(); option++)
	{
		if (links.at(option).itag == option2) {
			std::string link = links.at(option).url + "&signature=" + links.at(option).sig;
			return link;
		}
	}

	// Option 3
	for (unsigned int option = 0; option < links.size(); option++)
	{
		if (links.at(option).itag == option3) {
			std::string link = links.at(option).url + "&signature=" + links.at(option).sig;
			return link;
		}
	}

	// Give up and return first parsed FMT
	std::string link = links.at(0).url + "&signature=" + links.at(0).sig;
	return link;
}


int YouTube::DownloadVideoToHDD (const std::string & address, const std::string & path)
{
	// Get host name
	std::string tag = "youtube.com";
	int found = address.find(tag) + tag.length();
	std::string host(address, std::string("http://").length(), found - std::string("http://").length());

	// Create request
	std::string buffer, request = 
		"GET " + address + " HTTP/1.1" + newline +
		"Host: " + host + newline +
		"User Agent: " + UserAgent + newline +
		"Connection: close" + newline +
		newline;

	// Open file
	std::fstream file;
	file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);

	// Connect to host
	std::string link = host + ":" + Port;	
	BIO* bio = BIO_new_connect(&link[0]);

	if (bio == NULL) {
		ERR_print_errors_fp(stderr);
		return -1;
	}

	// Send request
	BIO_puts(bio, request.c_str());

	// Download monitor
	std::string streambuf;
	int bytes = -1;
	int remaining;
	/**/ int GUI = 0;

	buffer.resize(MAXBUF);

	while (1) {
		int x = BIO_read(bio, &buffer[0], buffer.length());
		
		if (x == 0) {
			break;
		}
		else if (x < 0) {
			if (!BIO_should_retry(bio)) {
				BIO_free_all(bio);
				file.close();
				return -1;
			}
		}
		else {
			if (bytes == -1)
			{
				streambuf.append(buffer.substr(0,x));
				size_t found = streambuf.find("\r\n\r\n");

				if (found != std::string::npos)
				{
					size_t cls = streambuf.find("Content-Length: ") + std::string("Content-Length: ").length();
					size_t cle = streambuf.find("\r\n",cls);
					std::string cllen (streambuf, cls, cle - cls);
					std::stringstream ss;
					ss << cllen;
					ss >> bytes;
					
					std::string writetofile(streambuf, found + std::string("\r\n\r\n").length());
					file.write(writetofile.c_str(), writetofile.length());

					remaining = bytes - writetofile.length();
				}
			}
			else
			{
				file.write(buffer.c_str(),x);
				remaining -= x;
			/**/if (GUI < 1) {
			/**/	GUI = 250;
			/**/	printf("Remaining: %0.2fMB\n",(float)(remaining/1048576.0));
			/**/ }
			/**/ GUI--;
			}
		}
	}
	file.close();
	return remaining;
}

int YouTube::DownloadVideoToRAM (const std::string & address, std::string & virtualhdd)
{
	// Get Host Name
	std::string tag = "youtube.com";
	int found = address.find(tag) + tag.length();
	std::string host(address, std::string("http://").length(), found - std::string("http://").length());

	// Create Request
	std::string buffer, request = 
		"GET " + address + " HTTP/1.1" + newline +
		"Host: " + host + newline +
		"User Agent: " + UserAgent + newline +
		"Connection: close" + newline +
		newline;

	// Clear virtual hdd
	virtualhdd.clear();

	// Connect to host
	std::string link = host + ":" + Port;	
	BIO* bio = BIO_new_connect(&link[0]);

	if (bio == NULL) {
		ERR_print_errors_fp(stderr);
		return -1;
	}

	// Send Request
	BIO_puts(bio, request.c_str());

	// Download monitor
	std::string streambuf;
	int bytes = -1;
	int remaining;

	buffer.resize(MAXBUF);

	while (1) {
		int x = BIO_read(bio, &buffer[0], buffer.length());
		
		if (x == 0) {
			break;
		}
		else if (x < 0) {
			if (!BIO_should_retry(bio)) {
				BIO_free_all(bio);
				return -1;
			}
		}
		else {
			if (bytes == -1)
			{
				streambuf.append(buffer.substr(0,x));
				size_t found = streambuf.find("\r\n\r\n");

				if (found != std::string::npos)
				{
					size_t cls = streambuf.find("Content-Length: ") + std::string("Content-Length: ").length();
					size_t cle = streambuf.find("\r\n",cls);
					std::string cllen (streambuf, cls, cle - cls);
					std::stringstream ss;
					ss << cllen;
					ss >> bytes;
					
					virtualhdd.reserve(bytes);

					std::string writetofile(streambuf, found + std::string("\r\n\r\n").length());
					virtualhdd.append(writetofile);

					remaining = bytes - writetofile.length();
				}
			}
			else
			{
				virtualhdd.append(buffer.substr(0,x));
				remaining -= x;
			}
		}
	}

	return remaining;
}

// Downloads the video info from youtube
std::string DownloadVideoInformation (const std::string & videoID)
{
	std::string buffer, response, request = 
		"GET /get_video_info?video_id=" + videoID + " HTTP/1.1" + newline +
		"Host:" + Host + newline +
		"User Agent: " + UserAgent + newline +
		"Connection: close" + newline +
		newline;

	std::string address = Host + ":" + Port;
	BIO* bio = BIO_new_connect(&address[0]);

	if (bio == NULL) {
		ERR_print_errors_fp(stderr);
		return std::string();
	}
	
	BIO_puts(bio, request.c_str());

	while (1) {
		buffer.resize(MAXBUF);
		int x = BIO_read(bio, &buffer[0], buffer.length());

		if (x == 0) {
			break;
		}
		else if (x < 0) {
			if (!BIO_should_retry(bio)) {
				BIO_free_all(bio);
				return std::string();
			}
		}
		else {
			response.append(buffer.substr(0,x));
		}
	}

	BIO_free_all(bio);
	return response;
}

// Copies the FMT from the body of response
std::string getFMTstream (const std::string & feed)
{
	const std::string keyword = "&url_encoded_fmt_stream_map=";
	size_t start = feed.find(keyword);
	
	if (start == std::string::npos) {
		return std::string();
	}

	start += keyword.length();
	size_t end = feed.find("&",start);
	return std::string (feed, start, end - start);
}

// Parses FMT stream
std::vector <FMTstream> parseFMTstream (const std::string & serial)
{
	std::vector <FMTstream> videoList;
	FMTstream video;
	bool iterate = true;
	size_t pointer = 0;

	while (iterate)
	{
		for (int i = 0; i < 6; i++)
		{
			size_t ampersand = serial.find('&', pointer);
			size_t comma = serial.find(',', pointer);

			std::string param;

			if (ampersand == std::string::npos && comma == std::string::npos) {
				iterate = false;
			}
			else if (ampersand < comma) {
				param = std::string (serial, pointer, ampersand - pointer);
			}
			else {
				param = std::string (serial, pointer, comma - pointer);
			}

			if (std::string(param,0,14) == "fallback_host=") {
				video.fallback_host = std::string(param,14);
			}
			else if (std::string(param,0,5) == "itag=") {
				video.itag = std::string(param,5);
			}
			else if (std::string(param,0,8) == "quality=") {
				video.quality = std::string(param,8);
			}
			else if (std::string(param,0,4) == "sig=") {
				video.sig = std::string(param,4);
			}
			else if (std::string(param,0,5) == "type=") {
				video.type = std::string(param,5);
			}
			else if (std::string(param,0,4) == "url=") {
				video.url = std::string(param,4);
			}

			if (ampersand < comma) {
				pointer = ampersand + 1;
			}
			else {
				pointer = comma + 1;
			}
		}

		if (!video.fallback_host.empty() && !video.itag.empty() && !video.quality.empty()
			&& !video.sig.empty() && !video.type.empty() && !video.url.empty())
		{
			video.fallback_host = decodeURLstring(video.fallback_host);
			video.itag = decodeURLstring(video.itag);
			video.quality = decodeURLstring(video.quality);
			video.sig = decodeURLstring(video.sig);
			video.type = decodeURLstring(video.type);
			video.url = decodeURLstring(video.url);
			videoList.push_back(video);
		}
	}

	return videoList;
}

// Decodes url encoded data
std::string decodeURLstring (const std::string & encode)
{
	std::string decode;
	std::stringstream ss;
	int hexval;

	for (unsigned int i = 0; i < encode.length(); i++)
	{
		if (encode.at(i) == '%') {
			ss << std::hex << encode.at(++i);
			ss << std::hex << encode.at(++i);
			ss >> hexval;
			ss.clear();
			decode.push_back(hexval);
		}
		else {
			decode.push_back(encode.at(i));
		}
	}

	return decode;
}