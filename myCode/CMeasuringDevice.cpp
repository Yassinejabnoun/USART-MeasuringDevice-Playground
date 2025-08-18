/*
 * CMeasuringDevice.cpp
 *
 *  Created on: 04.01.2023
 *      Author: jabno
 */

#include "CMeasuringDevice.h"

CMeasuringDevice::CMeasuringDevice(CSevenSegmentDisplay *display, BusIn *keys,
		BusOut *leds, BufferedSerial *usart, AnalogIn *voltageInput) {
	m_display = display;
    m_leds=leds;
	m_usart=usart;
	m_voltageInput=voltageInput;

	// Exemplar von CDebouncer auf dem Heap erzeugt
	m_keys = new CDebouncer(keys, 30);

	// e instiealisieren
    measuredValue =0;
    measuredVoltage=0;

	// Timer dynamisch erzeugen
	m_MeasuringTimer = new CPolledTimer(100);

	// Beim einschalten ist der Anzeigemodus balken
	m_displayMode = BALKEN;

    // Beim Einschalten  wird keine Messung durchgeführt
	m_MeasuringMode=DO_NOTHING;

	// Startnachricht auf dem Terminal angezeigt
	write("MIT-Digitalvoltmeter : SW-Rev. 0.1");
}

CMeasuringDevice::~CMeasuringDevice() {
	// frigabe von dynamische allokierte speicher über den destruktor
	delete m_keys;
	delete m_MeasuringTimer;

}

void CMeasuringDevice::Measuring() {
	//  vom ADC nehmen.
	measuredValue = m_voltageInput->read_u16();
	measuredVoltage = m_voltageInput->read_voltage();

	// e auf SSD dargestellen
	*m_display = measuredVoltage;
}

void CMeasuringDevice::pollkeys() {
	// Werte vom Taster einzulesen
	static uint8_t previousKeyStates = 0;
	uint8_t newKeyStates = m_keys->read();
	uint8_t keysPushed = (newKeyStates & ~previousKeyStates)&newKeyStates;
	previousKeyStates = newKeyStates;

    // Mess-AnzeigModus entsprechend den Gedrückten aktivieren
	setMode(keysPushed);
}

void CMeasuringDevice::setMode(uint8_t keyState) {

	// Taster 7
	if (keyState == 0x80) {
		m_displayMode = BALKEN;
		write("Displaymodus: Balken");
	}

	// Taster 6
	else if (keyState == 0x40) {
		m_displayMode = BCD;
		write("Displaymodus: BCD");
	}

	// Taster 5
	else if (keyState == 0x20) {
		m_displayMode = BINARY;
		write("Displaymodus: BINARY");
	}

	// Taster 4
	else if (keyState == 0x10) {
		m_displayMode = PC;
		PcMode();
	}

	// Taster 4
	else if (keyState == 0x08) {
		m_MeasuringMode = SingleMeasurement;
		write("Einzelmessung gestartet");
	}

	// Taster 3
	else if (keyState == 0x04) {
		m_MeasuringMode = ContinuousMeasurement;
		write("Dauermessung gestartet");
	}

	// Taster 2
	else if (keyState == 0x02) {
		// umschalten zwischen fortlaufend und pausierte Messung
		if (m_MeasuringMode == ContinuousMeasurement) {
			m_MeasuringMode = HOLD;
			write("Dauermessung pausiert\n");
		} else if (m_MeasuringMode == HOLD) {
			m_MeasuringMode = ContinuousMeasurement;
			write("Dauermessung fortgesetzt\n");
		}
	}

	// Taster 1
	else if (keyState == 0x01) {
		// Stop taster bei vortlauvenden Messmodus
		if (m_MeasuringMode == ContinuousMeasurement) {
			m_MeasuringMode = STOP;

			write("Dauermessung gestoppt");
		}

	}
	//anzeig modus an der methode changemode() weiter gegeben
	changeMode();
}


void CMeasuringDevice::changeMode() {
	// Auswahl von AnzeigModus

	// Anzeigemodus ist BALKEN
	if (m_displayMode == BALKEN) {
		BalkenMode();
	}

	// Anzeigemodus ist BCD
	else if (m_displayMode == BCD) {
		BCDMode();
	}

	// Anzeigemodus ist BINARY
	else if (m_displayMode == BINARY) {
		BinaryMode();
	}

	// Anzeigemodus ist PC
	else if (m_displayMode == PC) {
		// Damit der Nachricht (Dauermesseung Pausiert ) auf dem Terminal
		// dargestellt wird, soll PCMode(), wenn das Messgerät angehalten
		// wird, nicht aufgerufen werden.
		if (m_MeasuringMode != HOLD && m_MeasuringMode != DO_NOTHING) {
			PcMode();
		}
	}
	// Auswahl von MessModus

	// Einzelmessung
	if (m_MeasuringMode == SingleMeasurement) {
		Measuring();
		m_MeasuringMode = DO_NOTHING;
	}

	// FortlaufendeMessung
	else if (m_MeasuringMode == ContinuousMeasurement) {
		// ein wert jedes 100ms wird gemessen
		if (m_MeasuringTimer->timeReached()) {
			Measuring();
		}
	}

	// Taste "Stop"
	else if (m_MeasuringMode == STOP) {
		// e auf Null gesetzt
		measuredValue = 0;
		measuredVoltage = 0;

		// das Display wird zurückgesetzt
		*m_display = 0;

		// MessModus auf DO_NOTING gesetzt
		m_MeasuringMode = DO_NOTHING;
	}
}

void CMeasuringDevice::pollkeyboard() {
	// wenn über die serielle Schnittstelle ein neues
	// Zeichen empfangen wurde
	char keybord[1];
	if (m_usart->readable()) {
		m_usart->read(&keybord, 1);
		pollUsart(keybord[0]);
	}
}

void CMeasuringDevice::pollUsart(char keybord) {

	// Taster b
	if (keybord == 'b')
		setMode(0x80);


		// Taster c
	else if (keybord == 'c')
		setMode(0x40);


		// Taster i
	else if (keybord == 'i')
		setMode(0x20);


		// Taster p
	else if (keybord == 'p')
		setMode(0x10);


		// Taster v
	else if (keybord =='v')
		// Startnachricht wird auf dem Terminal dargestellt
		write("MIT-Digitalvoltmeter : SW-Rev. 0.1");

	     // Taster e
	else if ( keybord == 'e')
		setMode(0x08);


		// Taster f
	else if (keybord == 'f')
		setMode(0x04);


		// Taster h
	else if (keybord == 'h')
		setMode(0x02);


		// Taster s
	else if (keybord == 's')
		setMode(0x01);

	else
		// alte e auf dem terminal anzeigen auch beim stoppen des
		// fortlaufende messsung
		if ((m_MeasuringMode == HOLD && keybord=='p') ||
				(m_MeasuringMode == DO_NOTHING && keybord=='p')) {
			PcMode();
		}
		else{
			write("Unbekannter Befehln\n");
		}

}

void CMeasuringDevice::BCDMode() {
	// Erste Stelle vor dem Komma auf die 4 linken Leds dargestellt
	int8_t for_left_leds = ((int) measuredVoltage)<<4;

	// Erste Stelle nach dem Komma wird auf die 4 rechten Leds dargestellt
	float value = (measuredVoltage * 10);
	int four_rigth_leds = (int) value % 10;
	*m_leds = (for_left_leds & 0xf0) | (four_rigth_leds & 0x0f);
}

void CMeasuringDevice::BalkenMode() {
	// Anzahl leuchtender LEDs wird als  wert
	// vom Interwall zwischen 0 und 9  berechnet.
	unsigned int voltageSegment = measuredValue * 9 / 0xffff;
	if (voltageSegment >= 9) {
		voltageSegment = 8;
	}
	m_leds->write(0xff00 >> voltageSegment);
}

void CMeasuringDevice::BinaryMode() {
	*m_leds = (measuredValue >> 8);
}

void CMeasuringDevice::PcMode() {
	// String zum Speichern des Messwerte
	string messwert = "Displaymodus: PC = ";

	// der gesamte , mit allen Nachkommastellen wird
	// im Sting gespeichert
	messwert += to_string(measuredVoltage);

	// Einhat zu messwert wird addiert
	messwert += " V";

	// jede 100ms wird die Anzeige auf dem Terminal aktualiesiert
	if (m_MeasuringTimer->timeReached()) {
		write(messwert);
	}
}

void CMeasuringDevice::send(string text) {
	// Es wird am Anfang der Zeile geschrieben
	char toStart[1] = { '\r' };
	m_usart->write(toStart, 1);

	// Konvertiert text in einem Array aus Chars
	char buffer[40];
	strcpy(buffer, text.c_str());
	m_usart->write(buffer, text.length());
}

void CMeasuringDevice::clear()
{
	// String von Leeren Zeichnen
	string space="                                             ";
	send(space);
}

void CMeasuringDevice::write(string text)
{
	clear();
	send(text);
}

void CMeasuringDevice::poll()
{
	pollkeys();
	pollkeyboard();
}
