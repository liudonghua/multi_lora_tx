#include <json/json.h>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <stdlib.h> 
#include "sx1276_tx.hpp"

int main(int argc,char **argv)
{
	int i = 0;
//	std::vector<SX1276Tx> txs;
	std::ifstream in(argv[1]);
	Json::Value txConfs;
	
	in >> txConfs;
	if(txConfs.isArray()){
		for(i=0;i<txConfs.size();i++){
			
			std::thread t(&SX1276Tx::threadtx,SX1276Tx(txConfs[i]));
			t.detach();
			sleep(2);
		}
	}
	// for (auto tx = txs.begin(); tx != txs.end(); ++tx) 
	// {
	// 	tx[0].thdTx().detach();
	// }

	while(1){

	}
	// while (!txs.empty())
	// 	txs.pop_back();

	in.close();
	return 0;

}