#include <json/json.h>
#include <fstream>
#include <cstring>
//#include <vector>
#include <stdlib.h> 
#include "sx1276_tx.hpp"

#define MAX_TX		8





int main(int argc,char **argv)
{
	int i = 0;
	Json::Value conf;
	//std::vector<SX1276Tx> txs;
	//std::vector<std::thread> thds;
	std::ifstream in(argv[1]);
	Json::Value txConfs;
	in >> txConfs;
	for(i=0;i<MAX_TX;i++){
		if(txConfs.isArray()){
			
			conf = txConfs[i];
			SX1276Tx tx(conf);
			// strcpy(tx.device,conf["device"].asString().c_str());
			// if(conf["freqs"].isArray()){
			// 	tx.freqs=(uint32_t *)malloc(tx.freq_size*sizeof(uint32_t));

			// 	Json::Value freqs = conf["freqs"];
			// 	tx.freq_size 		= freqs.size();
			// 	for (int j=0;j<tx.freq_size;j++)
			// 		tx.freqs[j] = freqs[j].asInt();
			// }else{
			// 	printf("NO FREQUENCY\n");
			// 	exit(-1);
			// }

			// tx.max_count	= conf["max_count"].asInt();
			// tx.maxaddr		= conf["maxaddr"].asInt();
			// tx.plen			= conf["plen"].asInt();
			// tx.txpow		= conf["txpow"].asInt();
			// tx.timeout		= conf["timeout"].asInt();
			
			// if(conf["sfs"].isArray()){
			// 	Json::Value sfs = conf["sfs"];
			// 	tx.sf_size = sfs.size();
			// 	tx.sfs=(uint8_t *)malloc(tx.sf_size*sizeof(uint8_t));
			// 	for (int k=0;k<tx.sf_size;k++)
			// 		tx.sfs[k]	= sfs[k].asInt();
			// }else{
			// 	printf("NO SPREAD FACTOR\n");
			// 	exit(-1);
			// }
			
			//thds.push_back(tx.thd());
			//txs.push_back(tx);
			tx.thd().join();
		}
	}

	// for (auto thd = thds.begin(); thd != thds.end(); ++thd) 
	// {
	// 	thd[0].join();
	// }

	// for (auto tx = txs.begin(); tx != txs.end(); ++tx) 
	// {
	// 	free(tx[0].sfs);
	// 	free(tx[0].freqs);
	// }

	// while (!txs.empty())
	// 	txs.pop_back();

	in.close();
	return 0;

}