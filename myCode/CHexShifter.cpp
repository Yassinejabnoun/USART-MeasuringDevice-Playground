/*
 * CHexShifter.cpp
 *
 *  Created on: 31.05.2022
 *      Author: jabno
 */
#include "CHexShifter.h"

CHexShifter::CHexShifter(CSevenSegmentDisplay *display, BusIn *keys,
		BusOut *leds, BufferedSerial *usart) {

	m_display = display;
	m_leds = leds;
	m_usart = usart;
	// ursprüngliche Verhalten : rechts einfügen
	m_insertLeft = false;
	// ein Exemplar von CDebouncer  wird auf dem Heap erzeugt
	m_keys = new CDebouncer(keys, 200);
}

CHexShifter::~CHexShifter() {
	// Dynamischer Speicher wird freigegeben
	delete m_keys;
}

/**
 * Prüft, ob das übergebene Zeichen zulässig ist. Wenn ja,
 * konvertiert das Zeichen in eine Zahl zwischen 0 und 15
 * und ruft processDigit mit dem Ergebnis auf.
 *
 * @param c das zu bearbeitende Zeichen
 */
void CHexShifter::processChar(char c) {
	// eine muster Liste für die zulässige Zeichen
	char pattern_list[16] = { '0','1','2','3','4','5','6','7','8','9',
			'a','b','c','d','e','f'};
	// es wird überprüft ob die übergebene Zeichen zulässig ist
	for (int i = 0; i < 16; i++) {
		// ist das Zeichen zulässig ?
		if (c == pattern_list[i]) {
			// processDigit aufrufen
			processDigit(i);
		}
	}
}


/**
 * Sendet die aktuell in der 7-Segment-Anzeige dargestellten
 * Zeichen als Zeile über die serielle Schnittstelle.
 */
void CHexShifter::sendDisplayedDigits() const {
	// Array zum speichern der aktuell in der
	// 7-Segment-Anzeige dargestellten Zeichen
	char buffer[6];
	//  Alle zeichen vom SSD im buffer zu schreiben
	for (int i = 0; i < 4; i++) {
		if ((*m_display)[i] <= 9) {
			//die zahl in char umwandeln und im Buffer speichern
			buffer[i] = (*m_display)[i] + '0';
		} else {
			// die passende Buchsatabe wird in das Buffer gespeichert
			buffer[i] = (*m_display)[i] + 'a' - 10;
		}
	}
	// Zeilenumbruch
	buffer[4]='\r';
	buffer[5]='\n';
	// buffer wird auf dem Terminal geschrieben
	m_usart->write(buffer, 6);
}

/**
 * Schiebt in der Grundfunktion alle Stellen auf der
 * 7-Segment-Anzeige nach links und stellt den übergebenen
 * Wert auf der Stelle ganz rechts dar.
 *
 * Bei umschaltbarere Einfügerichtung wird entsprechend
 * der ausgewählten Richtung geschoben und eingefügt.
 *
 * Ruft abschließend sendDisplayedDigits auf.
 *
 * @param value der einzufügende Wert
 */
void CHexShifter::processDigit(uint8_t value) {
	// Die Zahlen werden nach rechts verschoben
	if (m_insertLeft) {
		for (int digit = 3; digit >0; digit--) {
			(*m_display)[digit] = (*m_display)[digit-1];
		}
		// Value wird recht auf dem display angezeigt
		(*m_display)[0] = value;
	}
    // Die Zahlen werden nach link verschoben
	else {
		for (int digit = 0; digit <3; digit++) {
			(*m_display)[digit] = (*m_display)[digit + 1];
		}
		// Value wird link auf dem display angezeigt
		(*m_display)[3] = value;

	}
	sendDisplayedDigits();
}

/**
 * Prüft, ob der Taster ganz rechts gedrückt wurde und ändert
 * die Einfügerichtung entsprechend.
 */
void CHexShifter::pollKeys() {
        //led ganz recht umschalten durch das taster drunter
		static uint8_t previousKeyStates = 0;
		uint8_t newKeyStates = m_keys->read();
		uint8_t keysPushed = (newKeyStates & ~previousKeyStates);
		*m_leds = (*m_leds ^ keysPushed) & 0x01 ;
		previousKeyStates = newKeyStates;

		if (*m_leds == 1){
			m_insertLeft = true;
		}
		else{
			m_insertLeft = false;
		}
}

/**
 * Prüft, ob über die serielle Schnittstelle ein neues Zeichen
 * empfangen wurde. Wenn das der Fall ist, wird das Zeichen
 * zur Verarbeitung an processChar weitergegeben.
 */
void CHexShifter::pollUsart() {
	char data[1];
	// wenn über die serielle Schnittstelle ein neues
	// Zeichen empfangen wurde
	if (m_usart->readable()) {
		m_usart->read(data, 1);
		// das Zeichen wird an processChar weitergegeben.
		processChar(data[0]);
	}
}

/**
 * Ruft die privaten poll...-Methoden zur Verarbeitung der
 * verschiedenen Eingabequellen auf.
 */
void CHexShifter::poll() {
	pollKeys();
	pollUsart();
}
