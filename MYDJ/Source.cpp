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

	std::string videolink;
	for (int attempt = 1; attempt <= 3; attempt++) {
		printf("attempt %i\n", attempt);

		videolink = YouTube::GetDownloadLink(videoID,"22","37","18");
		if (!videolink.empty()) {
			break;
		}

		if (attempt == 3) {
			printf("failed to access the youtube servers\n");
			return 0;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	int result = YouTube::DownloadVideoToHDD(videolink,videoID + ".mp4");

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