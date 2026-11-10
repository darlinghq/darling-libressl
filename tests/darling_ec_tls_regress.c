/*
 * Regression coverage for Darling's LibreSSL 2.8.3 NIST EC arithmetic.
 *
 * LibreSSL's old C NIST reducers alias BN_ULONG limbs as 32-bit words. With
 * strict aliasing enabled, optimized P-256/P-384 reduction can return wrong
 * residues and reject valid public points as "point is not on curve", breaking
 * TLS certificate decoding for common HTTPS endpoints.
 */

#include <stdio.h>
#include <stdlib.h>

#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/x509.h>

static const unsigned char github_p256_spki[] = {
	0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce,
	0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
	0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xbd, 0xd3, 0x72,
	0xa4, 0x47, 0x29, 0x30, 0xd7, 0xf1, 0xe0, 0x67, 0x9f, 0x16,
	0xa0, 0xf7, 0x30, 0x83, 0xd5, 0xb1, 0x9a, 0x93, 0xa7, 0x92,
	0x17, 0x8d, 0x0c, 0x06, 0x29, 0x8e, 0x8f, 0x39, 0xdb, 0x89,
	0xea, 0xc0, 0x8f, 0x0b, 0xdc, 0x6b, 0x6d, 0xf5, 0x42, 0xd4,
	0xa0, 0x12, 0xba, 0x48, 0x1d, 0xaf, 0x00, 0x4e, 0x89, 0x2c,
	0x7a, 0x9e, 0xcd, 0x11, 0x7b, 0xdf, 0x7a, 0x6f, 0xdd, 0x11,
	0xfd
};

static const unsigned char github_p384_spki[] = {
	0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce,
	0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22,
	0x03, 0x62, 0x00, 0x04, 0x76, 0xfa, 0x99, 0xa9, 0x6e, 0x20,
	0xed, 0xf9, 0xd7, 0x77, 0xe3, 0x07, 0x3b, 0xa8, 0xdb, 0x3d,
	0x5f, 0x38, 0xe8, 0xab, 0x55, 0xa6, 0x56, 0x4f, 0xd6, 0x48,
	0xea, 0xec, 0x7f, 0x2d, 0xaa, 0xc3, 0xb2, 0xc5, 0x79, 0xec,
	0x99, 0x61, 0x7f, 0x10, 0x79, 0xc7, 0x02, 0x5a, 0xf9, 0x04,
	0x37, 0xf5, 0x34, 0x35, 0x2b, 0x77, 0xce, 0x7f, 0x20, 0x8f,
	0x52, 0xa3, 0x00, 0x89, 0xec, 0xd5, 0xa7, 0xa2, 0x6d, 0x5b,
	0xe3, 0x4b, 0x92, 0x93, 0xa0, 0x80, 0xf5, 0x01, 0x94, 0xdc,
	0xf0, 0x68, 0x07, 0x1e, 0xcd, 0xee, 0xfe, 0x25, 0x52, 0xb5,
	0x20, 0x43, 0x1c, 0x1b, 0xfe, 0xeb, 0x19, 0xce, 0x43, 0xa3
};

static int
test_curve(int nid, const char *name)
{
	EC_GROUP *group = NULL;
	EC_KEY *key = NULL;
	EC_KEY *decoded = NULL;
	unsigned char *der = NULL;
	unsigned char *der_write;
	const unsigned char *derp;
	int der_len;
	int ret = 0;

	if ((group = EC_GROUP_new_by_curve_name(nid)) == NULL) {
		fprintf(stderr, "%s: EC_GROUP_new_by_curve_name failed\n", name);
		goto done;
	}
	if (EC_GROUP_check(group, NULL) != 1) {
		fprintf(stderr, "%s: EC_GROUP_check failed\n", name);
		goto done;
	}

	if ((key = EC_KEY_new_by_curve_name(nid)) == NULL) {
		fprintf(stderr, "%s: EC_KEY_new_by_curve_name failed\n", name);
		goto done;
	}
	if (EC_KEY_generate_key(key) != 1) {
		fprintf(stderr, "%s: EC_KEY_generate_key failed\n", name);
		goto done;
	}
	if (EC_KEY_check_key(key) != 1) {
		fprintf(stderr, "%s: EC_KEY_check_key failed\n", name);
		goto done;
	}

	der_len = i2d_EC_PUBKEY(key, NULL);
	if (der_len <= 0) {
		fprintf(stderr, "%s: i2d_EC_PUBKEY length failed\n", name);
		goto done;
	}
	if ((der = malloc(der_len)) == NULL) {
		fprintf(stderr, "%s: malloc failed\n", name);
		goto done;
	}

	der_write = der;
	if (i2d_EC_PUBKEY(key, &der_write) != der_len) {
		fprintf(stderr, "%s: i2d_EC_PUBKEY encode failed\n", name);
		goto done;
	}

	derp = der;
	if ((decoded = d2i_EC_PUBKEY(NULL, &derp, der_len)) == NULL) {
		fprintf(stderr, "%s: d2i_EC_PUBKEY failed\n", name);
		goto done;
	}
	if (EC_KEY_check_key(decoded) != 1) {
		fprintf(stderr, "%s: decoded EC_KEY_check_key failed\n", name);
		goto done;
	}

	ret = 1;

done:
	if (!ret)
		ERR_print_errors_fp(stderr);
	EC_KEY_free(decoded);
	free(der);
	EC_KEY_free(key);
	EC_GROUP_free(group);
	return ret;
}

static int
test_spki(const unsigned char *spki, size_t spki_len, const char *name)
{
	const unsigned char *spkip = spki;
	EVP_PKEY *pkey = NULL;
	EC_KEY *key = NULL;
	int ret = 0;

	if ((pkey = d2i_PUBKEY(NULL, &spkip, spki_len)) == NULL) {
		fprintf(stderr, "%s: d2i_PUBKEY failed\n", name);
		goto done;
	}
	if ((key = EVP_PKEY_get1_EC_KEY(pkey)) == NULL) {
		fprintf(stderr, "%s: EVP_PKEY_get1_EC_KEY failed\n", name);
		goto done;
	}
	if (EC_KEY_check_key(key) != 1) {
		fprintf(stderr, "%s: EC_KEY_check_key failed\n", name);
		goto done;
	}

	ret = 1;

done:
	if (!ret)
		ERR_print_errors_fp(stderr);
	EC_KEY_free(key);
	EVP_PKEY_free(pkey);
	return ret;
}

int
main(void)
{
	int failed = 0;

	failed |= !test_curve(NID_X9_62_prime256v1, "prime256v1");
	failed |= !test_curve(NID_secp384r1, "secp384r1");
	failed |= !test_curve(NID_secp521r1, "secp521r1");
	failed |= !test_spki(github_p256_spki, sizeof(github_p256_spki),
	    "github-p256-spki");
	failed |= !test_spki(github_p384_spki, sizeof(github_p384_spki),
	    "github-p384-spki");

	return failed;
}
