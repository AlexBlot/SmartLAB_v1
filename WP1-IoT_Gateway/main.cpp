/*
============================================================================
 Name : main.cpp
 Author : Alexandre BLOT
 Version :
 Copyright : Blot Copyright
 Description : Exe source file

Required:                   
    Connect Power through USB/External supply 12v
             ___________
            |   IDK     |
            |___________|

 ============================================================================
 */

#include "mbed.h"
#include "Shields.h"
#include "nrf24l01.h"
#include "IDdatabase.h"

#include "string.h"

#define SECURE WizFi250::SEC_AUTO			// Security setting for accesspoint
#define SSID "SmartLAB"						// Replace with your accesspoint name
#define PASS "SmartLAB2018"					// Replace with your accesspoint password

#define bit_is_set(b,n) (b & (1<<n))

NHD_C0216CZ lcd;

NRF24L01P radio;

WizFi250Interface wizfi;

Serial pc(USBTX,USBRX);

NCS36000 pir;

NOA1305 als;

Ticker timerinterrupt;

int interruptrequest=0;






//var to represent interrupt.
int testPIR=0;

char url[256];

void interrupt(){
	interruptrequest=1;             // Timer interrupt for HTTP request
}



void pir_routine()                            // Movement Detector PIR Interrupt when motion is detected
{

	pc.printf("Movement Detected\r\n");
	testPIR=1;

}




int main() {


	uint32_t IDRFID[100];
	char* statusaccess[100];
	uint32_t IDreceived;
	int IDcheckreceived=0;
	int IDnumber=0;
	boolean RFIDaccess=false;
	char* statustosend="";

	int ret;

	uint8_t data[5];
	uint8_t datatosend[5];
	uint8_t status1;

	int powercompreceived=0;
	int powercomp2received=0;
	int powercomp3received=0;
	float powerconsumption=0;
	float powerconsumption2=0;
	float powerconsumption3=0;

	float temperature=0;
	int tempreceived=0;

	int light=0;
	int lightinlux=0;

	int doorstatusreceived=0;
	int doorstatus=0; // closed


	pc.printf("Connecting to wifi\r\n");
	if (wizfi.connect(SSID, PASS) != 0) {
		pc.printf("Connection to access point failed\r\n");
		return 1;
	}
	else {
		pc.printf("WIFI connected to hotspot\r\n");
	}

	pc.printf("\r\nBaseboard IP address:'%s'\n", wizfi.get_ip_address());

	CSWifi=1; // Disable Wifi chip select which was causing errors with radio module

	//Initialize ALS
	if (als.init() != ALS_SUCCESS) {
		pc.printf("Error device is not matching\r\n");
		return -1;
	}
	else {
		pc.printf("ALS initialized\r\n");

	}

	pir.registerCallback(pir_routine); // Interrupt initialization

	pc.printf("PIR initialized\r\n");


	ret=radio.init();
	if(ret != RF_RET_SUCCESS){
		pc.printf("Radio Module : initialization failed, error = %d !!!\r\n", ret);



	}else{
		pc.printf("Radio Module : initialization success ...\r\n");

	}

	timerinterrupt.attach(&interrupt,100.0); // call this interrupt every 60 seconds
	pc.printf("Timer initialized \r\n");

	FillIDS(IDRFID,statusaccess); // Establish database

	for(int i=0;i<sizeIDdatabase;i++){
		pc.printf("Here is an ID : 0x%08x \r\n",IDRFID[i]);
		pc.printf("Here is the status corresponding : %s \r\n",statusaccess[i+1]);
	}



	while(1) {           // Idle Loop


		radio.writeRx(NRF_CONFIG,0x7B);     // CONFIG FOR RECEIVING
		radio.writeRx(RF_CH,0x2C);

		status1 = radio.registerRead(NRF_STATUS);  // read status register

		data[0]=0;
		data[1]=0;
		data[2]=0;
		data[3]=0;
		data[4]=0;

		datatosend[0]=0;
		datatosend[1]=0;
		datatosend[2]=0;
		datatosend[3]=0;
		datatosend[4]=0;

		RFIDaccess=false;

		als.read(light);
		lightinlux=als.getAbmienceInLux();


		if(bit_is_set(status1,6)){ // bit 6 = RX_DR in status register  , it means that we received data

			radio.writeRx(NRF_STATUS,(status1 | 0x40)); // clear RX_DR

			if(bit_is_set(status1,5)){ // bit 5 = TX_DS in status register

				radio.writeRx(NRF_STATUS,(status1 | 0x20)); // clear TX_DS

			}

			radio.ReadPayload(5,data); // then read the data we received in the payload
			radio.WriteCommand(FLUSH_RX); // clear fifo

			if(data[0]==1){ // YAWN sensor node 1 sending data (LOULOU)
				pc.printf("YAWN Sensor node 1 sending data \r\n");
				pc.printf("YAWN IDENTIFIER : %d\r\n ",data[1]);
				if(data[1]==2){ // Double check with YAWN identifier to receive only from one yawn
					temperature= ((data[2]*256+data[3])/16)*0.0625;
					pc.printf("SMARTLAB TEMPERATURE : %.1f °C\r\n",temperature);
					tempreceived=1;
				}


			}

			else if(data[0]==2){ // YAWN sensor node 2 sending data (PIERRE)
				pc.printf("YAWN Sensor node 2 sending data \r\n");
				pc.printf("YAWN IDENTIFIER : %d\r\n ",data[1]);
				if(data[1]==14){  // Double check with YAWN identifier to receive only from one yawn and identify which one is sending
					pc.printf("YAWN Sensor 2.1 sending data \r\n");
					powerconsumption=(data[3]<<8)|data[2];
					powerconsumption=powerconsumption*3.09;
					powerconsumption=(powerconsumption/153.7)*220;
					pc.printf("Power Consumption 2.1 : %.1f WATT \r\n ",powerconsumption);
					powercompreceived=1;
				}
				if(data[1]==8){  // Double check with YAWN identifier to receive only from one yawn and identify which one is sending
					pc.printf("YAWN Sensor 2.2 sending data \r\n");
					powerconsumption2=(data[3]<<8)|data[2];
					powerconsumption2=powerconsumption2*3.09;
					powerconsumption2=(powerconsumption2/153.7)*220;
					pc.printf("Power Consumption 2.2 : %.1f WATT \r\n ",powerconsumption2);
					powercomp2received=1;
				}
				if(data[1]==13){  // Double check with YAWN identifier to receive only from one yawn and identify which one is sending
					pc.printf("YAWN Sensor 2.3 sending data \r\n");
					powerconsumption3=(data[3]<<8)|data[2];
					powerconsumption3=powerconsumption3*3.09;
					powerconsumption3=(powerconsumption3/153.7)*220;
					pc.printf("Power Consumption 2.3 : %.1f WATT \r\n ",powerconsumption3);
					powercomp3received=1;
				}
			}






			else if(data[0]==3){ // YAWN sensor node 3 sending data (JORIS)
				pc.printf("YAWN Sensor node 3 sending data \r\n");
				pc.printf("YAWN IDENTIFIER : %d\r\n ",data[1]);
				doorstatus=data[2];
				if(doorstatus==1){
					pc.printf("SmartLAB door is open \r\n");
					doorstatusreceived=1;
				}
				else if(doorstatus==0){
					pc.printf("SmartLAB door is closed \r\n");
					doorstatusreceived=1;
				}

			}



			else if(data[0]==30){ // YAWN sensor node 3 asking for ID check (JORIS)
				pc.printf("Checking for ID in database \r\n");
				pc.printf("0x%02x\r\n",data[1]);
				pc.printf("0x%02x\r\n",data[2]);
				pc.printf("0x%02x\r\n",data[3]);
				pc.printf("0x%02x\r\n",data[4]);
				IDreceived=(data[1]<<24 | data[2]<<16 | data[3]<<8 |data[4]);
				pc.printf("Total ID : 0x%08x\r\n",IDreceived);
				for(int i=0;i<sizeIDdatabase;i++){
					if(IDreceived==IDRFID[i]){
						RFIDaccess=true;
						IDnumber=i+1;
						statustosend=statusaccess[i+1];


					}

				}
				IDcheckreceived=1;
				if(RFIDaccess==true){

					pc.printf("Access Granted !!!! \r\n");
					//wait(0.5);
					radio.writeRx(NRF_CONFIG,0x7A); // TX MODE
					radio.writeRx(RF_CH,0x28); // CHANNEL 40 for sending data to sensor node 3 (Joris)
					radio.writeRx(NRF_STATUS,0x70); // Clear Status Register
					datatosend[0]=70;
					datatosend[1]=1;
					radio.WritePayload(5,datatosend);
					pc.printf("Sent \r\n");
					radio.WriteCommand(FLUSH_RX);
					radio.WriteCommand(FLUSH_TX);

				}
				else{
					pc.printf("Access Denied ....\r\n");
					//wait(0.5);
					radio.writeRx(NRF_CONFIG,0x7A); // TX MODE
					radio.writeRx(RF_CH,0x28); // CHANNEL 40 for sending data to sensor node 3 (Joris)
					radio.writeRx(NRF_STATUS,0x70); // Clear Status Register
					datatosend[0]=70;
					datatosend[1]=0;
					radio.WritePayload(5,datatosend);
					pc.printf("Sent \r\n");
					radio.WriteCommand(FLUSH_RX);
					radio.WriteCommand(FLUSH_TX);
					IDnumber=0;
					statustosend=statusaccess[0];

				}

			}

		}

		if(interruptrequest==1){    // HTTP REQUEST BASED ON WHICH DATA WE RECEIVED BEFORE TO SEND ONLY FRESH DATA WITH ALL THE CASES POSSIBLE, SYNTAX HAS TO BE IMPROVED

			if(powercomp3received){

				if(powercomp2received){

					if(tempreceived && powercompreceived && doorstatusreceived && IDcheckreceived ){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;
						tempreceived=0;
						IDcheckreceived=0;
					}

					else if(tempreceived && powercompreceived && doorstatusreceived ){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;
						tempreceived=0;
					}

					else if(tempreceived && powercompreceived && IDcheckreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field6=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,IDnumber,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but door status...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;
						tempreceived=0;
						IDcheckreceived=0;
					}

					else if(tempreceived && powercompreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field7=%f&field8=%f ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but door status and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;
						tempreceived=0;
					}

					else if(tempreceived && IDcheckreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field6=%d&field1=%d&field2=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,IDnumber,lightinlux,testPIR,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but door status and current...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						IDcheckreceived=0;
						tempreceived=0;
					}

					else if(tempreceived && doorstatusreceived && IDcheckreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but current...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						tempreceived=0;
						IDcheckreceived=0;
					}

					else if(tempreceived && doorstatusreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but current and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						tempreceived=0;
					}




					else if(powercompreceived && doorstatusreceived && IDcheckreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but temp...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;
						IDcheckreceived=0;

					}


					else if(powercompreceived && doorstatusreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but temp and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;

					}

					else if(powercompreceived && IDcheckreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field6=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,IDnumber,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but temp and doorstatus...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;

					}

					else if(IDcheckreceived && doorstatusreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&field5=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS", IDnumber,lightinlux,testPIR,doorstatus,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but temp and current...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;

					}

					else if(powercompreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but temp and door status and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						powercompreceived=0;

					}

					else if(tempreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but current and door status and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						tempreceived=0;
					}

					else if(doorstatusreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field5=%d&field1=%d&field2=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",doorstatus ,lightinlux,testPIR,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but current and temp and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						tempreceived=0;
					}

					else if(IDcheckreceived){
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&status=%s&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS",IDnumber ,lightinlux,testPIR,statustosend,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but current and temp and door status...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;
						tempreceived=0;
						IDcheckreceived=0;
					}


					else{
						CSWifi=0;
						url[0]=0;
						sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field1=%d&field2=%d&field7=%f&field8=%f  ", "VO1UTTWIX67E90QS", lightinlux,testPIR,powerconsumption2,powerconsumption3);
						HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
						pc.printf("Sending all Data but temp and current and door status and IDcheck...\r\n");
						pc.printf("+ Sending powercomp2...\r\n");
						pc.printf("+ Sending powercomp3...\r\n");
						get_req->send();
						pc.printf("Done \r\n");
						delete get_req;

					}
					powercomp2received=0;
				} /// Powercomp2 if end

				else if(tempreceived && powercompreceived && doorstatusreceived && IDcheckreceived ){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
					IDcheckreceived=0;
				}

				else if(tempreceived && powercompreceived && doorstatusreceived ){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
				}

				else if(tempreceived && powercompreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field6=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,IDnumber,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but door status...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
					IDcheckreceived=0;
				}

				else if(tempreceived && powercompreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
				}

				else if(tempreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field6=%d&field1=%d&field2=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,IDnumber,lightinlux,testPIR,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but door status and current...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					IDcheckreceived=0;
					tempreceived=0;
				}

				else if(tempreceived && doorstatusreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
					IDcheckreceived=0;
				}

				else if(tempreceived && doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
				}




				else if(powercompreceived && doorstatusreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					IDcheckreceived=0;

				}


				else if(powercompreceived && doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(powercompreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field6=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,IDnumber,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and doorstatus...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(IDcheckreceived && doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&field5=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS", IDnumber,lightinlux,testPIR,doorstatus,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and current...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(powercompreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field8=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(tempreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field8=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
				}

				else if(doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field5=%d&field1=%d&field2=%d&field8=%f  ", "VO1UTTWIX67E90QS",doorstatus ,lightinlux,testPIR,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and temp and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
				}

				else if(IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&status=%s&field8=%f  ", "VO1UTTWIX67E90QS",IDnumber ,lightinlux,testPIR,statustosend,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and temp and door status...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
					IDcheckreceived=0;
				}


				else{
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field1=%d&field2=%d&field8=%f  ", "VO1UTTWIX67E90QS", lightinlux,testPIR,powerconsumption3);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and current and door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp3...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;

				}
				powercomp3received=0;
			} // IF power comp 3 end
			else{


			if(powercomp2received){

				if(tempreceived && powercompreceived && doorstatusreceived && IDcheckreceived ){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
					IDcheckreceived=0;
				}

				else if(tempreceived && powercompreceived && doorstatusreceived ){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
				}

				else if(tempreceived && powercompreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field6=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,IDnumber,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but door status...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
					IDcheckreceived=0;
				}

				else if(tempreceived && powercompreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field7=%f ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					tempreceived=0;
				}

				else if(tempreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field6=%d&field1=%d&field2=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,IDnumber,lightinlux,testPIR,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but door status and current...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					IDcheckreceived=0;
					tempreceived=0;
				}

				else if(tempreceived && doorstatusreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
					IDcheckreceived=0;
				}

				else if(tempreceived && doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
				}




				else if(powercompreceived && doorstatusreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;
					IDcheckreceived=0;

				}


				else if(powercompreceived && doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field7=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(powercompreceived && IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field6=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,IDnumber,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and doorstatus...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(IDcheckreceived && doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&field5=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS", IDnumber,lightinlux,testPIR,doorstatus,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and current...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(powercompreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field7=%f  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					powercompreceived=0;

				}

				else if(tempreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field7=%f  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
				}

				else if(doorstatusreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field5=%d&field1=%d&field2=%d&field7=%f  ", "VO1UTTWIX67E90QS",doorstatus ,lightinlux,testPIR,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and temp and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
				}

				else if(IDcheckreceived){
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&status=%s&field7=%f  ", "VO1UTTWIX67E90QS",IDnumber ,lightinlux,testPIR,statustosend,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but current and temp and door status...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;
					tempreceived=0;
					IDcheckreceived=0;
				}


				else{
					CSWifi=0;
					url[0]=0;
					sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field1=%d&field2=%d&field7=%f  ", "VO1UTTWIX67E90QS", lightinlux,testPIR,powerconsumption2);
					HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
					pc.printf("Sending all Data but temp and current and door status and IDcheck...\r\n");
					pc.printf("+ Sending powercomp2...\r\n");
					get_req->send();
					pc.printf("Done \r\n");
					delete get_req;

				}

				powercomp2received=0;
			} /// Powercomp2 if end


			else if(tempreceived && powercompreceived && doorstatusreceived && IDcheckreceived ){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;
				tempreceived=0;
				IDcheckreceived=0;
			}

			else if(tempreceived && powercompreceived && doorstatusreceived ){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field5=%d  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,doorstatus);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;
				tempreceived=0;
			}

			else if(tempreceived && powercompreceived && IDcheckreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d&field6=%d&status=%s  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR,IDnumber,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but door status...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;
				tempreceived=0;
				IDcheckreceived=0;
			}

			else if(tempreceived && powercompreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field4=%f&field1=%d&field2=%d  ", "VO1UTTWIX67E90QS",temperature ,powerconsumption,lightinlux,testPIR);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but door status and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;
				tempreceived=0;
			}

			else if(tempreceived && IDcheckreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field6=%d&field1=%d&field2=%d&status=%s  ", "VO1UTTWIX67E90QS",temperature ,IDnumber,lightinlux,testPIR,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but door status and current...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				IDcheckreceived=0;
				tempreceived=0;
			}

			else if(tempreceived && doorstatusreceived && IDcheckreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus,IDnumber,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but current...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				tempreceived=0;
				IDcheckreceived=0;
			}

			else if(tempreceived && doorstatusreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d&field5=%d  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR,doorstatus);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but current and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				tempreceived=0;
			}




			else if(powercompreceived && doorstatusreceived && IDcheckreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d&field6=%d&status=%s  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus,IDnumber,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but temp...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;
				IDcheckreceived=0;

			}


			else if(powercompreceived && doorstatusreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field5=%d  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,doorstatus);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but temp and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;

			}

			else if(powercompreceived && IDcheckreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d&field6=%d&status=%s  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR,IDnumber,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but temp and doorstatus...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;

			}

			else if(IDcheckreceived && doorstatusreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&field5=%d&status=%s  ", "VO1UTTWIX67E90QS", IDnumber,lightinlux,testPIR,doorstatus,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but temp and current...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;

			}

			else if(powercompreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field4=%f&field1=%d&field2=%d  ", "VO1UTTWIX67E90QS", powerconsumption,lightinlux,testPIR);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but temp and door status and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				powercompreceived=0;

			}

			else if(tempreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field3=%f&field1=%d&field2=%d  ", "VO1UTTWIX67E90QS",temperature ,lightinlux,testPIR);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but current and door status and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				tempreceived=0;
			}

			else if(doorstatusreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field5=%d&field1=%d&field2=%d  ", "VO1UTTWIX67E90QS",doorstatus ,lightinlux,testPIR);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but current and temp and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				tempreceived=0;
			}

			else if(IDcheckreceived){
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field6=%d&field1=%d&field2=%d&status=%s  ", "VO1UTTWIX67E90QS",IDnumber ,lightinlux,testPIR,statustosend);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but current and temp and door status...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;
				tempreceived=0;
				IDcheckreceived=0;
			}


			else{
				CSWifi=0;
				url[0]=0;
				sprintf( url, "http://api.thingspeak.com/update?api_key=%s&field1=%d&field2=%d  ", "VO1UTTWIX67E90QS", lightinlux,testPIR);
				HttpRequest* get_req = new HttpRequest(&wizfi, HTTP_GET, url);
				pc.printf("Sending all Data but temp and current and door status and IDcheck...\r\n");
				get_req->send();
				pc.printf("Done \r\n");
				delete get_req;

			}


			}

			interruptrequest=0;
			testPIR=0;
		}

	}



}
