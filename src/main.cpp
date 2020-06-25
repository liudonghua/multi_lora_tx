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
			sleep(5);
		}
	}
	while(1){
		sleep(10);
	}
	in.close();
	return 0;

}