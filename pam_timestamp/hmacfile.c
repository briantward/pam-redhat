#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hmacsha1.h"

static void
testvectors(void)
{
	char *hmac;
	size_t hmac_len;
	int i, j;
	char hex[3];
	struct vector {
		const unsigned char *key;
		int key_len;
		const unsigned char *data;
		int data_len;
		const char *hmac;
	} vectors[] = {
		{
		"\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b", 20,
		"Hi There", 8,
		"b617318655057264e28bc0b6fb378c8ef146be00",
		},

		{
		"Jefe", 4,
		"what do ya want for nothing?", 28,
		"effcdf6ae5eb2fa2d27416d5f184df9c259a7c79",
		},

		{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 20,
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd", 50,
		"125d7342b9ac11cd91a39af48aa17b4f63f175d3",
		},

		{
		"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 25,
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd",
		50,
		"4c9007f4026250c6bc8414f9bf50c86c2d7235da",
		},

		{
		"\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c", 20,
		"Test With Truncation", 20,
		"4c1a03424b55e07fe7f27be1d58bb9324a9a5a04",
		},

		{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa",
		80,
		"Test Using Larger Than Block-Size Key - Hash Key First", 54,
		"aa4ae5e15272d00e95705637ce8a3b55ed402112",
		},

		{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa",
		80,
		"Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", 73,
		"e8e99d0f45237d786d6bbaa7965c7808bbff1a91",
		},
	};
	for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
		hmac = NULL;
		hmac_len = 0;
		hmac_sha1_generate(&hmac, &hmac_len,
				   vectors[i].key, vectors[i].key_len,
				   vectors[i].data, vectors[i].data_len);
		if (hmac != NULL) {
			for (j = 0; j < hmac_len; j++) {
				snprintf(hex, sizeof(hex), "%02x",
					 hmac[j] & 0xff);
				if (strncasecmp(hex,
						vectors[i].hmac + 2 * j,
						2) != 0) {
					printf("Incorrect result for vector %d\n", i + 1);
					exit(1);

				}
			}
			free(hmac);
		} else {
			printf("Error in vector %d.\n", i + 1);
			exit(1);
		}
	}
}

int
main(int argc, char **argv)
{
	char *hmac;
	size_t maclen;
	const char *keyfile;
	int i, j;

	testvectors();

	keyfile = argv[1];
	for (i = 2; i < argc; i++) {
		hmac_sha1_generate_file(&hmac, &maclen, keyfile, -1, -1,
					argv[i], strlen(argv[i]));
		if (hmac != NULL) {
			for (j = 0; j < maclen; j++) {
				printf("%02x", hmac[j] & 0xff);
			}
			printf("  %s\n", argv[i]);
			free(hmac);
		}
	}
	return 0;
}