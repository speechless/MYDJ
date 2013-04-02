#include "stdafx.h"
#include "YouTube.h"


int main (int argc, char * argv[])
{
	CRYPTO_malloc_init();
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	std::string videoID = "HgNLrVk1BXU";

/*	// Debug: delete this
	printf("Enter youtube video id\n");
	printf("eg: for https://www.youtube.com/watch?v=D0ZUDvoJFzY the id will be: D0ZUDvoJFzY\n");
	std::cin >> videoID;
	printf("\n\n");
	printf("User input:[%s]\n",videoID.c_str());
	printf("Getting download link\n");
	// Debug: end
*/
	std::string videolink = YouTube::GetDownloadLink(videoID,"22","37","18");
	
	// Debug: delete this
//	printf("Download link generated -----\n%s\n",videolink.c_str());
	// Debug: end

	int result = YouTube::DownloadVideoToHDD(videolink,videoID + ".mp4");

/*	std::string virtualhdd;
	int result = YouTube::DownloadVideoToRAM(videolink, virtualhdd);

	std::fstream file;
	file.open(videoID + ".mp4", std::ios::out | std::ios::binary | std::ios::trunc);
	file.write(virtualhdd.c_str(),virtualhdd.length());
	file.close();
	virtualhdd.clear();
	virtualhdd.shrink_to_fit();
*/	
	if (result == 0) {
		printf("Download Successful!\n");
	}
	else if (result == -1) {
		printf("Error, invalid videoID\nPlease make sure you have typed the ID in correctly\n");
	}
	else if (result > 0) {
		printf("Download failed, %i bytes missing\n", result);
	}
	else {
		printf("System Error! CODE: %i\n", result);
	}
	
	system ("pause");

	return 0;
}