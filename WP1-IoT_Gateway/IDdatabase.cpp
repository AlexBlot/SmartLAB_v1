#include "IDdatabase.h"


#include "mbed.h"
#include "Shields.h"
#include "nrf24l01.h"
#include "IDdatabase.h"

#include "string.h"





void FillIDS(uint32_t IDRFID[100],char* statusaccess[100]){

	IDRFID[0]=ID1;
	IDRFID[1]=ID2;
	IDRFID[2]=ID3;
	IDRFID[3]=ID4;
	//IDRFID[x]=IDx;
	statusaccess[0]=statusdenied;
	statusaccess[1]=statusID1;
	statusaccess[2]=statusID2;
	statusaccess[3]=statusID3;
	//statusaccess[4]=statusID4;  // Remove this comment to use tag number 4
	//statusaccess[x]=statusIDx


}
