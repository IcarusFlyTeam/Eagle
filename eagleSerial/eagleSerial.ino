#include <IcarusXbee.h>
#include <IcarusCelle.h>
#include <IcarusIO.h>

#define myDebugSerial Serial
#define SerialXbee Serial3

IcarusIO IO(&myDebugSerial);
IcarusXbee myXbee(&IO, eagle, &SerialXbee);
typeCmdFire CmdFireReceived;

int16_t t_countdown;
uint16_t contare;

int selezione;
bool esci;

void setup() {
  myDebugSerial.begin(115200);
  displayInfo();
  
  setupXbee();
  esci = false;
}

void loop() {

  display_menu();
  switch (selezione) {
    case 1: myDebugSerial.println("Communication Test");
            myDebugSerial.println("Invio Chiave");
			myXbee.SendCmdFire(testConnessione);
			if(myXbee.CheckAcknCmdFire(testConnessione, T_TOLL))
				myDebugSerial.println("Test Completato con Successo");
			else
				myDebugSerial.println("Test fallito");
            break;
    case 2: myXbee.SendCmdFire(Carica);
			if (myXbee.CheckAcknCmdFire(Carica,T_TOLL))
			myDebugSerial.println("Carica condensatori avviata!");
			else
			{myDebugSerial.println("Problema con la carica!");
			break;}
			esci = false;
            for (int i=(T_CHARGE/1000);i>=0;i--) {
				if (myDebugSerial.available()) {
					while (myDebugSerial.available())
					{
						myDebugSerial.read();
					}
					myDebugSerial.println("Carica annullata!");
					myXbee.SendCmdFire(stopEmergenza);
					scaricaEmergenza();
					esci = true;
					break;
				}
				myDebugSerial.println(i);
              delay(1000);}
			if (esci) break;
			if (myXbee.CheckAcknCmdFire(Carica, T_TOLL))
				myDebugSerial.println("Carica condensatori completata!");
			else
				myDebugSerial.println("Credo che la carica sia completata!");
			break;
    case 3: while (myDebugSerial.available())
					myDebugSerial.read();
			myDebugSerial.println("Inserire la durata del conto alla rovescia desiderata\nespressa in secondi");
            while(myDebugSerial.available()==0);
            if (myDebugSerial.available()>0)
				contare = myDebugSerial.readString().toInt();
			myDebugSerial.println("Premere un qualsiasi tasto (+ invio) per annullare.");
			IO.Cmd = contare;
			myXbee.addToPayload(Comando);
			myXbee.SendCmdFire(avviaCountdown);
			if (myXbee.CheckAcknCmdFire(avviaCountdown, T_TOLL))
				myDebugSerial.println("Avvio conto alla rovescia...");
			else {
				myDebugSerial.println("Problema con l'avvio del conto alla rovescia!");
				scaricaEmergenza();
				break;
			}
			esci = false;
			for (int i = contare; i >= 0; i--) {
				myDebugSerial.println(i);
				delay(1000);
				if (myDebugSerial.available()) {
					while (myDebugSerial.available())
					{
						myDebugSerial.read();
					}
					myDebugSerial.println("Annullato!");
					scaricaEmergenza();
					esci = true;
					break;
				}
			}
			if (esci) break;
          	myXbee.SendCmdFire(Fuoco);
			if (myXbee.CheckAcknCmdFire(Fuoco, T_TOLL)) 
			{
				myDebugSerial.println("Fuoco");
				if (myXbee.CheckAcknCmdFire(Fuoco, T_FIRE+T_TOLL)) 
					myDebugSerial.println("Fuoco completato!");
				else
					myDebugSerial.println("Credo che il fuoco sia completato!");
			}
			else {
				myDebugSerial.println("Problema con l'innesco!");
				scaricaEmergenza();
				break;
			}       
            break;
    case 4: myDebugSerial.println("Scarica in sicurezza avviata");
			myXbee.SendCmdFire(scaricaSicura);
			for (int i = (T_SAFE_DISCHARGE / 1000); i >= 0; i--) {
				myDebugSerial.println(i);
				delay(1000);
			}
			if (myXbee.CheckAcknCmdFire(scaricaSicura, T_TOLL))
				myDebugSerial.println("Scarica sicura condensatori completata!");
			else
				myDebugSerial.println("Credo che la scarica sia completata!");
            break;
    case 5: myDebugSerial.println("Communication Test continuo avviato");
			while (1) {
				if (myDebugSerial.available()) {
					while (myDebugSerial.available() > 0)
					{
						myDebugSerial.read();
					}
					myDebugSerial.println("Test di comunicazione interrotto!");
						break;
				}
			myDebugSerial.println("Invio Chiave");
			myXbee.SendCmdFire(testConnessione);
			if (myXbee.CheckAcknCmdFire(testConnessione, T_TOLL))
				myDebugSerial.println("Test Completato con Successo");
			else
				myDebugSerial.println("Test fallito");
			delay(1000);
			}
            break;
	case 6: myDebugSerial.println("Invio comando di avvio del Log");
			myXbee.SendCmdFire(startLog);
			if (myXbee.CheckAcknCmdFire(startLog, T_TOLL))
				myDebugSerial.println("Log avviato con Successo");
			else
				myDebugSerial.println("Non so se il log si sia avviato!");
			break;
	case 7: myDebugSerial.println("Invio comando di arresto del log");
			myXbee.SendCmdFire(stopLog);
			if (myXbee.CheckAcknCmdFire(stopLog, T_TOLL))
				myDebugSerial.println("Log arrestato con Successo");
			else
				myDebugSerial.println("Non so se il log si sia arrestato!");
			break;
			break;
    default: Serial.println("Operazione non disponibile! Ritenta!");
            break;
            }
	while (myDebugSerial.available())
	{
		myDebugSerial.read();
	}
    myDebugSerial.println("");
	delay(1000);
}

void display_menu(){
  myDebugSerial.println("Inserire il numero corrispondente all'operazione desiderata");
  myDebugSerial.println("1- Connection Test");
  myDebugSerial.println("2- Carica condensatori");
  myDebugSerial.println("3- Avvia conto alla rovescia");
  myDebugSerial.println("4- Scarica condensatori in sicurezza");
  myDebugSerial.println("5- Communication test continuo");
  myDebugSerial.println("6- Start logging celle di carico");
  myDebugSerial.println("7- Stop logging celle di carico");
  
  while(Serial.available()==0);
  if (Serial.available()>0)
    selezione=Serial.read()-'0';
  }

void displayInfo() {
	myDebugSerial.println("*******************************************************");
	myDebugSerial.println("*                                                     *");
	myDebugSerial.println("*                 CROCODILE PROJECT                   *");
	myDebugSerial.println("*                     CONSOLE DI                      *");
	myDebugSerial.println("*   					   LANCIO                        *");
	myDebugSerial.println("*                                                     *");
	myDebugSerial.println("*                        DAVE                         *");
	myDebugSerial.println("*                                                     *");
	myDebugSerial.println("*******************************************************");
	delay(1000);
}

void setupXbee() {
	SerialXbee.begin(115200);
	if (!SerialXbee)
	{
		myDebugSerial.println("Xbee not ready!");
		while (1);
	}
	if (SerialXbee) 
	{
		while (myDebugSerial.available()) myDebugSerial.read();
		myDebugSerial.println("Communications initialized");
	}
}

void scaricaEmergenza() {
	myXbee.SendCmdFire(stopEmergenza);
	if (myXbee.CheckAcknCmdFire(scaricaSicura, T_SAFE_DISCHARGE + T_TOLL))
		myDebugSerial.println("Scarica di emergenza completata!");
	else
		myDebugSerial.println("Credo che la scarica di emergenza sia completata!");
}
