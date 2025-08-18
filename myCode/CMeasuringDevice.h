/*
 * CMeasurementDevice.h
 *
 *  Created on: Nov 4, 2020
 *      Author: mnl
 */

#ifndef CMEASURINGDEVICE_H_
#define CMEASURINGDEVICE_H_

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"

/**
 * Repräsentiert die realisierte Anwendung "Messgerät".
 */

// enum für Anzeigemodus
typedef enum {
	BALKEN, 	// taster 7
	BCD,		// taster 6
	BINARY, 	// taster 5
	PC,		    // taster 4
} display_mode;

// enum für Messmoudus
typedef enum {
	SingleMeasurement,        // EinzelMessung (taster 3)
	ContinuousMeasurement,	  // Fortlaufende Messung (taster 2)
	HOLD,		              // Anhalten (taster 1)
	STOP, 	                  // Stop (taster 0)
	DO_NOTHING,               // Do Nothing
} Measuring_mode;

class CMeasuringDevice {
private:
	CSevenSegmentDisplay* m_display;
	CDebouncer* m_keys;
	BusOut* m_leds;
	BufferedSerial* m_usart;
	AnalogIn* m_voltageInput;
	display_mode m_displayMode; // Anzeigemodus
	Measuring_mode  m_MeasuringMode; // Messmoudus
	CPolledTimer*  m_MeasuringTimer; //timer
	uint16_t measuredValue; // Messwert von ADC als 16 Bit
	float measuredVoltage;// Messwert von ADC in volts

	/**
	 * @brief prüft welchen Taster gedrückt wurde, und ändert
	 * die Anzeigemodus bzw. Messmodus entsprechen dem gedrükten
	 * Taster
	 */
	void pollkeys();

	/**
	 * @brief prüft welches Zeichen vom Tastatur eingegeben wurde,
	 * und ändert die Anzeigemodus bzw. Messmodus entsprechen dem
	 * eingegebenen Zeichen.
	 *
	 * wenn ein ungültiges Zeichen eingegeben wird, wird eine
	 * Fehlermelldung auf dem Terminal gesendet
	 */
	void pollkeyboard();

	/**
	 * @brief ändert den Anzeigemodus bzw messmode entsprechen dem gedrückten,
	 * taster und gibt den aktualisierten Modus zu der Metode changeMode,
	 * um die Anzeigemodus zu ändern
	 *
	 *  @param KeyStates der Wert des gedrückten Tasters, der
	 */
	void setMode(uint8_t keystates);

	/**
	 * @brief ändert den Messmodus bzw anzeigmodus entsprechen dem gedrückten
	 *  Taster vom PC, und gibt den aktualisierten Modus zu  changeMode,
	 * zu ändern, bzw kontrolieren.
	 *
	 *  @param keyboard der Wert des gedrückten Tasters
	 */
	void pollUsart(char keyboard);

	/**
	 * @brief ruft die passende Methode entsprechen dem Anzeigemodus
	 * bzw messmodus
	 */
	void changeMode();

	/**
	 * @brief stellt den Messwert als BCD zahl auf die Leds dar
	 */
	void BCDMode();

	/**
	 * @brief stellt den Messwert als Balken auf die Leds dar
	 */
	void BalkenMode();

	/**
	 * @brief stellt den Messwert als Binär auf die Leds dar
	 */
	void BinaryMode();

	/**
	 * @brief stellt den Messwert auf dem Terminal dar
	 */
	void PcMode();

	/**
	 * @brief liest den aktuellen Wert des ADC,speichert den
	 * in den passenden Attributen, und zeigt den auf dem SSD
	 * an.
	 */
	void Measuring();

	/**
	 * @brief Hilfsmethode zum Schreiben auf dem Terminal
	 *
	 * @param word String, der auf dem Terminal geschrieben wird
	 */
	void send(string word);

	/**
	 * @brief löscht was auf dem Terminal geschieben ist, duchr Sendung
	 * von lerren Zeichnen
	 */
	void clear();

	/**
	 * @brief zuerst löscht die Zeile auf dem Terminal, danach schreibt
	 * den gegebenen String
	 *
	 * @param word String, der auf dem Terminal geschrieben wird
	 */
	void write(string word);

public:
	/**
	 * Erzeugt ein neues Exemplar der Anwendung, das die angegebenen
	 * Hardware-Komponenten benutzt.
	 *
	 * Die Taster werden über ein Exemplar von CDebouncer entprellt.
	 * Da die Klasse CDebouncer keinen Default-Konstruktor hat, kann
	 * das Objekt nicht als Attribut definiert werden, sondern muss im
	 * Konstruktor auf dem Heap erzeugt und im Destruktor wieder
	 * freigegeben werden.
	 *
	 * @param display die 7-Segment-Anzeige
	 * @param keys die Eingänge, an die die Taster angeschlossen sind
	 * @param leds die Ausgänge, an die die LEDs angeschlossen sind
	 * @param usart die serielle Schnittstelle
	 * @param voltageInput der Analogeingang
	 */
	CMeasuringDevice(CSevenSegmentDisplay* display, BusIn* keys,
			BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput);

	/**
	 * Gibt die allokierten Ressourcen wieder frei.
	 */
	~CMeasuringDevice();

	/**
	 * Ruft die privaten poll...-Methoden zur Verarbeitung der
	 * verschiedenen Eingabequellen auf.
	 */
	void poll();
};

#endif /* CMEASURINGDEVICE_H_ */
