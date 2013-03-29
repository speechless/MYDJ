#include "stdafx.h"

#include <openssl\bio.h>
#include <openssl\ssl.h>
#include <openssl\err.h>

int main (int argc, char * argv[])
{
	CRYPTO_malloc_init();
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	BIO* bio = BIO_new_connect("www.google.com:80");
	if (bio == NULL) {
		printf("Error creating BIO!\n");
		ERR_print_errors_fp(stderr);
		return 0;
	}

	if (BIO_do_connect(bio) <= 0) {
		printf("Failed to connect!");
		return 0;
	}

	std::string buf;
	buf.resize(1024);
	
	std::string send =	"GET / HTTP/1.1\r\n"
						"Host:www.google.com\r\n"
//						"User Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n"
						"Connection: Close\r\n\r\n";
	BIO_puts(bio, send.c_str());

	while (1) {
		int x = BIO_read(bio, &buf[0], buf.length());

		if (x == 0) {
			break;
		}
		else if (x < 0) {
			if (!BIO_should_retry(bio)) {
				printf("\nRead Failed!\n");
				BIO_free_all(bio);
				return 0;
			}
		}
		else {
			buf[x] = 0;
			printf("%s",buf);
		}
	}

	BIO_free_all(bio);
	return 0;
}