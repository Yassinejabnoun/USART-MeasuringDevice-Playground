/*
 * CSevenSegmentDisplay.cpp
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#include "CSevenSegmentDisplay.h"

// ToDo: Tragen Sie hier (statt der 0) die Werte aus dem von Ihnen
// in der Vorbereitung erstellten Array mit Segment-Mustern ein.
uint8_t CSevenSegmentDisplay::patterns[] = { 0xD7, 0x11, 0x8F, 0x9B, 0x59, 0xDA, 0xDE, 0x91,
		0xDF, 0xDB, 0xDD, 0x5E, 0xC6, 0x1F, 0xCE, 0xCC };

CSevenSegmentDisplay::CSevenSegmentDisplay(
		CShiftRegisterOutputExtender *ssdSegments, DigitalOut* digits[4]) {
	// Übergeben Werte in Attribten speichern
	m_ssdSegments = ssdSegments;
	for (int digit = 0; digit < 4; digit++) {
		m_digits[digit] = digits[digit];
	}

	// Array mit anzuzeigenden Ziffern initialisieren
	for (int digit = 0; digit < 4; digit++) {
		m_values[digit] = 0;
	}

	// Als Startzustand wird Stelle 0 aktiviert und alle Segmente werden
	// ausgeschaltet.
	*m_digits[0] = 1;
	for (int digit = 1; digit < 4; digit++) {
		*m_digits[digit] = 0;
	}
	m_activeDigit = 0;
	*m_ssdSegments = 0xff;
}

void CSevenSegmentDisplay::activateNextDigit() {
	// Nächste Bitfolge wird in Schiebregister geschrieben
	m_ssdSegments->prepareOutput(
			~(patterns[m_values[(m_activeDigit + 1) % 4]]
					+ m_decimalPoint[(m_activeDigit + 1) % 4]));
	*m_digits[m_activeDigit] = 0;
	m_activeDigit = (m_activeDigit + 1) % 4;
	m_ssdSegments->updateOutput();
	*m_digits[m_activeDigit] = 1;
}

uint8_t& CSevenSegmentDisplay::operator[] (int position) {
	static uint8_t dummy = 0;
	if (position < 0 || position > 3) {
		return dummy;
	}
	return m_values[position];
}

void CSevenSegmentDisplay::setDecimalPoint(int digit) {
	// set alle Dezimalpunkte zurück
	for (uint8_t i = 0; i < 4; i++) {
		m_decimalPoint[i] = 0;
	}
	// neue Dezimalpunkt setzen
	if (digit >= 0 && digit <= 3) {
		m_decimalPoint[digit] = 0x20;
	}
}

void CSevenSegmentDisplay::setValue(int value) {
	// Ist der angegebene Wert größer 0 und kleiner 9999 ?
	if (value >= 0 && value <= 9999) {
		//dann wird Value auf dem Display angezeigt
		m_values[0] = value % 10;
		m_values[1] = (value / 10) % 10;
		m_values[2] = (value / 100) % 10;
		m_values[3] = (value / 1000) % 10;
	} else {
		//sonst wird 'EEEE' angezeigt
		for (int i = 0; i < 4; i++)
			m_values[i] = 0xE;
	}
}

CSevenSegmentDisplay& CSevenSegmentDisplay::operator =(int value) {
	setValue(value);
	return *this;
}

void CSevenSegmentDisplay::setValue(float value) {
	// Ist der angegebene Wert größer 0.001 und kleiner 9999 ?
	if (value >= 0.001 && value <= 9999.0) {
		// dann wird value mit anpassende Komma auf dem Display angezeigt

		//Stelle der Dezimalepunkt auf dem Display wurde am Anfang auf 0
		int digit = 0;

		float muster = value;
		//Position der Dezimalpunkt wird gesucht
		while (muster <= 1000 && digit < 3) {
			//Solange der Muster <=1000 und die Stelle der Dezimalpunkt
			// kleiner 3 wird die Schleife durchgeführt
			muster *= 10;
			digit += 1;
		}
		/**
		 * Nach der Schleife nimmt digit die Stelle der Dezimalpunkt an .
		 * der neue Wert von Muster ist der auf dem Display zu zeigen ist ,
		 * also ohne Komma
		 */
		setDecimalPoint(digit);
		setValue(int(muster));
	} else {
		//Ist der angegebene Wert größer 9999.0 ?
		if (value > 9999.0) {
			//	Dann werden 'EEEE' angezeigt
			for (int i = 0; i < 4; i++) {
				m_values[i] = 0xE;
			}
			setDecimalPoint(-1);
		}
		// Sonst der angegebene Wert kleiner 0.000
		else {
			// Dann werden '0000' angezeigt
			for (int i = 0; i < 4; i++) {
				m_values[i] = 0x0;
			}
			setDecimalPoint(3);
		}
	}
}

CSevenSegmentDisplay& CSevenSegmentDisplay::operator =(float value) {
	setValue(value);
	return *this;
}
