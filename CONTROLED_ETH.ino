// <?
#include <SPI.h>
#include <Ethernet.h>
IPAddress ip(192,168,1,177); byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xAA, 0xED };EthernetServer server(80);

String vers; vers = "CONTROLED - 28-04-2015";
int valSL1;
int PR;
int CENA;
int RTIME[6];  // 3 CENAS
int PTIME[10]; // 5 COR
int CENAlimit;
int PRESlimit;
int TESTdelay;
String comando;

// Pinagem Reles dimerizacao - geral(12,13)
int RLCOR[] = {7,8,9,10,11,12,13};
// Pinagem Sensor Presença cada corredor
int SPCOR[] = {2,3,4,5,6};
// Pinagem Sensor de luz Serial.println(success ? "success" : "failed");
int SL1 = A0;

void setup() {
	
	Ethernet.begin(mac, ip);server.begin();Serial.begin(9600);
	 
	 		pinMode(SL1, INPUT);digitalWrite(SL1, LOW);
			for (int i=0;i<sizeof(SPCOR)/sizeof(int);i++) {pinMode(SPCOR[i],  INPUT);digitalWrite(SPCOR[i], HIGH);}
			for (int i=0;i<sizeof(RLCOR)/sizeof(int);i++) {pinMode(RLCOR[i], OUTPUT);digitalWrite(RLCOR[i], LOW);}	
		
			CENA = 2;
			comando="auto";				 			
			
	Serial.println(vers);Serial.print(">");		 			
}


void loop() { delay(500);
	// HTTP listen
	EthernetClient client = server.available();
  	//Serial Listen
	String serialIN = ""; char cha; while(Serial.available()) {cha= Serial.read();serialIN.concat(cha);}

 	//Default
	if (serialIN == "auto") comando = "auto";
	else if (serialIN == "teste") comando = "teste";
 
		if (client) {
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		String str;
			while (client.connected()) {
						if (client.available()) {
							char c = client.read(); str.concat(c);
								
									  if(str.endsWith("/cena1")) CENA =1;
								else if(str.endsWith("/cena2")) CENA =2;
								else if(str.endsWith("/cena3")) CENA =3;
								else if(str.endsWith("/teste")) comando = "teste";
								else if(str.endsWith("/auto"))  comando = "auto";
		
								if (c == '\n' && currentLineIsBlank) {
										client.println("HTTP/1.1 200 OK");
										client.println("Content-Type: text/html");
										client.println();
										client.println("<h3>NAVE Control</h3>");
										client.println("<h4>Cena atual: ");client.println(CENA);
										client.println(" Comando: "); client.print(comando); client.print("</h4>");
										break;
									}
							if (c == '\n') {currentLineIsBlank = true;} 
							else if (c != '\r') {currentLineIsBlank = false;}					
							}

					}
		}
	// close the connection:
	delay(100); client.stop();
		
		
	if (comando == "auto") {CENAlimit = 1000; // Delay para chaveamento
	
					valSL1=analogRead(SL1);



				// 1 - Dia claro / SLuz
				if(valSL1 >=600){
						if(CENA!=3){
							if(RTIME[3]>CENAlimit){CENA=3;RTIME[3]=CENAlimit;RTIME[1]=0;RTIME[2]=0;} 
							else {RTIME[3]++;}
						}else {
								if(RTIME[1]>0)RTIME[1]--;
								if(RTIME[2]>0)RTIME[2]--;
						}	
				}
				
				else if(valSL1 <600 && valSL1 >400){
						if(CENA!=2){
							if(RTIME[2]>CENAlimit){CENA=2;RTIME[2]=CENAlimit;RTIME[1]=0;RTIME[3]=0;} 
							else {RTIME[2]++;}
						}else {
								if(RTIME[1]>0)RTIME[1]--;
								if(RTIME[3]>0)RTIME[3]--;
						}	
				}
				
				else if(valSL1 <400){
						if(CENA!=1){
							if(RTIME[1]>CENAlimit){CENA=1;RTIME[1]=CENAlimit;RTIME[3]=0;RTIME[2]=0;} 
							else {RTIME[1]++;}
						}else {
								if(RTIME[2]>0)RTIME[2]--;
								if(RTIME[3]>0)RTIME[3]--;
						}	
				}

	}
	else if (comando == "teste") {	TESTdelay = 700;
  	
          Serial.println("");Serial.print("Testando Reles");
		
  				for (int i=0;i<sizeof(RLCOR)/sizeof(int);i++) {
  					                     digitalWrite(RLCOR[i], HIGH);Serial.print(".");delay(TESTdelay); 
                                    digitalWrite(RLCOR[i], LOW);delay(TESTdelay/2); 
               }
  
			Serial.println("");Serial.print("Testando Dimerizacao");
			
 					digitalWrite(RLCOR[5], HIGH);delay(TESTdelay);digitalWrite(RLCOR[6], HIGH);Serial.print(".");delay(TESTdelay);
 					digitalWrite(RLCOR[6], LOW);delay(TESTdelay);digitalWrite(RLCOR[5], LOW);Serial.print(".");delay(TESTdelay*2);
 					
			Serial.println("");Serial.print("Testando Circuitos");
			
				for (int i=0;i<sizeof(RLCOR)/sizeof(int)-2;i++) {
					digitalWrite(RLCOR[i], HIGH);Serial.print("."); delay(TESTdelay);
					digitalWrite(RLCOR[i], LOW); delay(TESTdelay);
					}
					
			Serial.println("");Serial.print(">");
			comando = "auto";
	}
	
	
// Serial.println("");Serial.print("A0 = ");Serial.println(valSL1);Serial.print("RTIME[1] = ");Serial.println(RTIME[1]);Serial.print("RTIME[2] = ");Serial.println(RTIME[2]);Serial.print("RTIME[3] = ");Serial.println(RTIME[3]);Serial.print("CENA = ");Serial.println(CENA);


//Debug CENA = 1;


switch (CENA) {
	
    case 3:
 // Dia claro / SLuz
					Serial.println("");Serial.print("Cena 3");
					for (int i=0;i<sizeof(RLCOR)/sizeof(int);i++) {digitalWrite(RLCOR[i], LOW);}
					digitalWrite(RLCOR[5], HIGH);digitalWrite(RLCOR[6], HIGH);

					 break;
				
		case 2:
 // Anoitecer / SLuz
					Serial.println("");Serial.print("Cena 2");
					for (int i=0;i<sizeof(RLCOR)/sizeof(int);i++) {digitalWrite(RLCOR[i], LOW);}
					digitalWrite(RLCOR[5], LOW);digitalWrite(RLCOR[6], HIGH);
					break;
				
		case 1:
 // Noite / SLuz
					Serial.println("");Serial.print("Cena 1");
               for (int i=0;i<sizeof(RLCOR)/sizeof(int)-2;i++) {digitalWrite(RLCOR[i], HIGH);}
					digitalWrite(RLCOR[5], LOW);digitalWrite(RLCOR[6], LOW);
                                        
                                        
 				// Dimeriza cada corredor / SPresenca
 
 				PRESlimit = 5000;
 					for (int i=0;i<sizeof(SPCOR)/sizeof(int);i++) { 
 
							PR=digitalRead(SPCOR[i]);
							
// DEBUG Serial.println("");Serial.print(i); Serial.print("PR=");Serial.print(PR);Serial.print("PTIME =");Serial.print(PTIME[i]);

							if(PR ==0){
								digitalWrite(RLCOR[i], LOW);PTIME[i]=PRESlimit;
							}	
							else {
								if(PTIME[i]>0)PTIME[i]--;
								if(PTIME[i]<=0){digitalWrite(RLCOR[i], HIGH);}
								else {digitalWrite(RLCOR[i], LOW);}									
							}
						}
						break;
				
		default:
			for (int i=0;i<sizeof(RLCOR)/sizeof(int);i++) {pinMode(RLCOR[i], OUTPUT);digitalWrite(RLCOR[i], LOW);}
				
		}
}
