#include "mbed.h"

#include "breadboard.h"
#include "CHexShifter.h"
#include  "CMeasuringDevice.h"


// Legen Sie die globalen Objekte, die die Peripheriekomponenten
// repräsentieren, hier an (vor den Funktionen).

BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3, BB_BTN_4, BB_BTN_5,
BB_BTN_6, BB_BTN_7);
BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5,
BB_LED_6, BB_LED_7);

DigitalOut ssdSerialData(SSD_SER);
DigitalOut ssdShiftRegisterClock(SSD_SCLK);
DigitalOut ssdBufferRegisterClock(SSD_RCLK);

DigitalOut ssdDigit0(SSD_DIGIT_0, OpenDrainNoPull);
DigitalOut ssdDigit1(SSD_DIGIT_1, OpenDrainNoPull);
DigitalOut ssdDigit2(SSD_DIGIT_2, OpenDrainNoPull);
DigitalOut ssdDigit3(SSD_DIGIT_3, OpenDrainNoPull);

/**
 * Implementierung der Lösung zur Aufgabe 1.
 */
void task1() {
	// Schreiben Sie vor der Enlosschleife den einmal auszuführenden Code

	BufferedSerial usart(PA_2, PA_3, 115200);
    while (true) {
    	// Schreiben Sie in der Endlosschleife den immer wieder auszuführenden
    	// Code
    	char data[] = { '*' };
    	usart.write(data, 1);
    }
}

void task2() {
	//erzeugt Objekt für Schieberegister
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);
	//ssdDigits zuweisen
	DigitalOut *ssdDigits[4];
	ssdDigits[0] = &ssdDigit0;
	ssdDigits[1] = &ssdDigit1;
	ssdDigits[2] = &ssdDigit2;
	ssdDigits[3] = &ssdDigit3;
	//Erzeugt Objekt für SSD
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigits);
	// Eine Taktperiode von 5 ms erzeugen
	CPolledTimer digitTimer(5);
	BufferedSerial usart(PA_2, PA_3, 115200);
	//Schnittstelle auf „non blocking I/O“ gesetzt
	usart.set_blocking(false);
	CHexShifter application(&ssd,&keys,&leds,&usart);
	while (true) {
		if (digitTimer.timeReached()) {
			ssd.activateNextDigit();
		}
		application.poll();
	}
}

void task3(){
	//erzeugt Objekt für Schieberegister
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);
	//ssdDigits zuweisen
	DigitalOut *ssdDigits[4];
	ssdDigits[0] = &ssdDigit0;
	ssdDigits[1] = &ssdDigit1;
	ssdDigits[2] = &ssdDigit2;
	ssdDigits[3] = &ssdDigit3;
	//Erzeugt Objekt für SSD
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigits);
	// Analoger Eingang
	AnalogIn poti(BB_POTI, 3.3);
	// Eine Taktperiode von 5 ms erzeugen
	CPolledTimer digitTimer(5);
	BufferedSerial usart(PA_2, PA_3, 115200);
	//Schnittstelle auf „non blocking I/O“ gesetzt
	usart.set_blocking(false);
	CMeasuringDevice application(&ssd,&keys,&leds,&usart,&poti);
	while (true) {
		if (digitTimer.timeReached()) {
			ssd.activateNextDigit();
		}
		application.poll();
	}

}





// Legen Sie weitere Funktionen taskN() (mit N = 3, 4, 5, ...) nach Bedarf an.

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {
	// Falls notwendig, ergänzen Sie hier Aufrufe, um Konfigurationen
	// an globalen Objekten vorzunehmen, die nicht durch die Angabe von
	// Argumenten beim Aufruf des Konstruktors vorgenommen werden können.


	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
	//task1();
	task2();
	//task3();


	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2()
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgeführt. Für den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
    while (true) {
    }
}

