#ifndef _IDDATABASE_H
#define	_IDDATABASE_H

#include "mbed.h"
#include "Shields.h"


#include "string.h"

uint32_t ID1=0x7BB352C3; // ID number 1
uint32_t ID2=0x0C0952A3; // ID number 2
uint32_t ID3=0x70EBB72B; // ID number 3
uint32_t ID4=0x7015A32B; // ID number 4
// ADD HERE NEW IDS
// ADD IDRFID[x]=IDx in the IDdatabase.cpp

char* statusdenied="Access_Denied_ID_not_in_database";

char* statusID1="Access_Granted_ID1:_Tvrtko_Mandic";
char* statusID2="Access_Granted_ID2:_Adrijan_Baric";
char* statusID3="Access_Granted_ID3:_Luka_Modric";
char* statusID4="Access_Granted_ID4:_Ivan_Rakitic";
// ADD HERE NEW STATUS WITH NAME CORRESPONDING TO THE NEW ID
// ADD statusaccess[x]=statusIDx in the IDdatabase.cpp


int sizeIDdatabase=3; // DON'T FORGET TO CHANGE THE SIZE HERE CORRESPONDING TO THE NUMBER OF IDS IN THIS DATABASE

void FillIDS(uint32_t IDRFID[100],char* statusaccess[100]);





#endif /* IDdatabase */
