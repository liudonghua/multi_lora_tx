#include <json/json.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <stdlib.h> 
#include "sx1276_tx.hpp"

#define MAX_TX		8





int main(int argc,char **argv)
{
	int i = 0;
	std::vector<SX1276Tx> txs;
	std::vector<std::thread> thds;
	std::ifstream in(argv[1]);
	Json::Value txConfs;
	in >> txConfs;
	for (i=0;i<MAX_TX;i++){
		if(txConfs[i]){
			SX1276Tx tx;
			Json::Value conf = txConfs[i];
			strcpy(tx.device,conf["device"].asString().c_str());
			
			tx.freq_size	= conf["freq_size"].asInt();
			if(conf["freqs"].isArray()){
				tx.freqs=(uint32_t *)malloc(tx.freq_size*sizeof(uint32_t));

				Json::Value freqs = conf["freqs"];
				for (int j=0;j<tx.freq_size;j++)
					tx.freqs[j] = freqs[j].asInt();
			}

			tx.max_count	= conf["max_count"].asInt();
			tx.maxaddr		= conf["maxaddr"].asInt();
			tx.plen			= conf["plen"].asInt();
			tx.txpow		= conf["txpow"].asInt();
			tx.sf_size		= conf["sf_size"].asInt();
			tx.timeout		= conf["timeout"].asInt();
			
			if(conf["sfs"].isArray()){
				tx.sfs=(uint8_t *)malloc(tx.sf_size*sizeof(uint8_t));

				Json::Value sfs = conf["sfs"];
				for (int k=0;k<tx.sf_size;k++)
					tx.sfs[k]	= sfs[k].asInt();
			}
			
			thds.push_back(tx.thd());
			txs.push_back(tx);
		}
	}

	for (auto thd = thds.begin(); thd != thds.end(); ++thd) 
	{
		thd[0].join();
	}

	for (auto tx = txs.begin(); tx != txs.end(); ++tx) 
	{
		free(tx[0].sfs);
		free(tx[0].freqs);
	}

	while (!txs.empty())
		txs.pop_back();

	in.close();
	return 0;

}