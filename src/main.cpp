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

	std::ifstream in(argv[1]);
	Json::Value txConf;
	
	in >> txConf;

	Json::Value confs = txConf["conf"];
	if(confs && confs.isArray()){
		for(i=0;i<confs.size();i++){
			
			std::thread t(&SX1276Tx::threadtx,SX1276Tx(confs[i]));
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