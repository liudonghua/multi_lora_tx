#include <json/json.h>
#include <fstream>
#include <cstring>
#include <thread> 
#include <stdlib.h> 
#include "sx1276_tx.hpp"

#define MAX_TX		8





int main(int argc,char **argv)
{
	int i = 0;
	
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
				Json::Value freqs = conf["freqs"];
				for (int j=0;j<tx.freq_size;j++)
					tx.freqs[j] = freqs[j].asInt();
			}

			tx.max_count	= conf["max_count"].asInt();
			tx.maxaddr		= conf["maxaddr"].asInt();
			tx.plen			= conf["plen"].asInt();
			
			tx.sf_size		= conf["sf_size"].asInt();
			if(conf["sfs"].isArray()){
				Json::Value sfs = conf["sfs"];
				for (int k=0;k<tx.sf_size;k++)
					tx.sfs[k]	= sfs[k].asInt();
			}
			

			std::thread thd(&SX1276Tx::threadtx,&tx);
			thd.join();

		}
	}
	in.close();
	return 0;

}