/*

 TiltGap - Cryptocurrency Bulk Address Generator
 Copyright (c) 2021 Lone Dynamics Corporation

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libbtc/include/btc/ecc.h"
#include "libbtc/include/btc/chainparams.h"
#include "libbtc/include/btc/tool.h"

enum addr_type { ADDR_P2PKH, ADDR_P2SH_P2WPKH, ADDR_P2WPKH };

btc_chainparams ltc_chainparams_main = {
	"ltcmain",
	0x30,
	0x32,
	"ltc",
	0xb0,
	0x019d9cfe,
	0x019da462,
	{0},
	{0},
	0,
	{0}
};

btc_chainparams ltc_chainparams_test = {
	"ltctest",
	0x6f,
	0x3a,
	"tltc",
	0xef,
	0x0436ef7d,
	0x0436f6e1,
	{0},
	{0},
	0,
	{0}
};

btc_chainparams doge_chainparams_main = {
	"dogemain",
	0x1e,
	0x16,
	"doge",
	0x9e,
	0x02fac398,
	0x02facafd,
	{0},
	{0},
	0,
	{0}
};

btc_chainparams doge_chainparams_test = {
	"dogetest",
	0x71,
	0xc4,
	"tdoge",
	0xf1,
	0x02fac398,
	0x02facafd,
	{0},
	{0},
	0,
	{0}
};

void show_usage(char **argv);
int generate_addresses(enum addr_type type, char *currency, int qty);

int main(int argc, char *argv[]) {

	int opt;
	enum addr_type opt_addr_type = ADDR_P2PKH;

	while ((opt = getopt(argc, argv, "hpsb")) != -1) {
		switch (opt) {
			case 'h': show_usage(argv); return(0); break;
			case 'p': opt_addr_type = ADDR_P2PKH; break;
			case 's': opt_addr_type = ADDR_P2SH_P2WPKH; break;
			case 'b': opt_addr_type = ADDR_P2WPKH; break;
		}
	}

	if (optind + 1 >= argc) {
		show_usage(argv);
		return(1);
	}

	int qty = strtol(argv[optind + 1], NULL, 10);
	return(generate_addresses(opt_addr_type, argv[optind], qty));

}

void show_usage(char **argv) {
	printf("usage: %s [-hpsb] <currency> <quantity>\n" \
      " -h\tdisplay help\n" \
      " -p\tgenerate p2pkh addresses (default)\n" \
      " -s\tgenerate p2sh-p2wpkh addresses\n" \
		" -b\tgenerate p2wpkh (Bech32) addresses\n" \
      " supported currencies: btc, tbtc, ltc, tltc, doge, tdoge\n",
		argv[0]);
}

int generate_addresses(enum addr_type type, char *currency, int qty) {

	size_t addrlen = 128;
	size_t pubkeylen = 128;
	size_t privkeywiflen = 128;

	char privkeywif[privkeywiflen];

	btc_ecc_start();

	const btc_chainparams *ccp = NULL;

	if (!strncmp(currency, "btc", 5)) ccp = &btc_chainparams_main;
	if (!strncmp(currency, "tbtc", 5)) ccp = &btc_chainparams_test;
	if (!strncmp(currency, "ltc", 5)) ccp = &ltc_chainparams_main;
	if (!strncmp(currency, "tltc", 5)) ccp = &ltc_chainparams_test;
	if (!strncmp(currency, "doge", 5)) ccp = &doge_chainparams_main;
	if (!strncmp(currency, "tdoge", 5)) ccp = &doge_chainparams_test;

	if (ccp == NULL) {
		printf("error: unsupported currency\n");
		return(1);
	}

	char pubkeyhex[pubkeylen];
	char address_p2pkh[addrlen];
	char address_p2sh_p2wpkh[addrlen];
	char address_p2wpkh[addrlen];

	FILE *ft, *fj;
	time_t ts = time(NULL);
	char out_dir[256];
	char out_txt[256];
	char out_json[512];
	char *addr;

	snprintf(out_dir, 255, "tiltgap-%s-%li", currency, ts); 

	printf("making directory %s\n", out_dir);
	mkdir(out_dir, 0700);

	snprintf(out_txt, 255, "tiltgap-%s-%li.csv", currency, ts); 
	ft = fopen(out_txt, "w");

	printf("generating addresses and keys ...\n");

	for (int i = 0; i < qty; i++) {

		addr = NULL;

		gen_privatekey(ccp, privkeywif, privkeywiflen, NULL);

		pubkey_from_privatekey(ccp, privkeywif, pubkeyhex, &pubkeylen);

		addresses_from_pubkey(ccp, pubkeyhex,
			address_p2pkh, address_p2sh_p2wpkh, address_p2wpkh);

		switch (type) {
			case ADDR_P2PKH: addr = address_p2pkh; break;
			case ADDR_P2SH_P2WPKH: addr = address_p2sh_p2wpkh; break;
			case ADDR_P2WPKH: addr = address_p2wpkh; break;
		}

		snprintf(out_json, 511, "%s/%s.%s.json", out_dir, currency, addr); 

		fj = fopen(out_json, "w");

		fprintf(fj,
			"{\"currency\":\"%s\",\"address\":\"%s\",\"plain_wif\":\"%s\"}",
			currency, addr, privkeywif);

		fclose(fj);

		fprintf(ft, "%s,%s\n", currency, addr);

		memset(privkeywif, 0, strlen(privkeywif));

	}

	fclose(ft);

	printf("done.\n");

	return(0);

}
