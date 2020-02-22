/*
 * RttyEncoder.cpp
 *
 *  Created on: Feb 22, 2020
 *      Author: Bruce MacKinnon KC1FSZ
 */
#include "RttyEncoder.h"

static const char shiftLetters = 0x1f;
static const char shiftFigures = 0x1b;
static const int baudotLen = 32;
static const char baudotLetters[] = { 0, 'T', '\r', 'O', ' ', 'H', 'N', 'M', '\n', 'L', 'R', 'G',
  'I', 'P', 'C', 'V', 'E', 'Z', 'D', 'B', 'S', 'Y', 'F', 'X', 'A', 'W', 'J', 0, 'U', 'Q', 'K', 0 };
static const char baudotFigures[] = { 0, '5', '\r', '9', ' ', 0, ',', '.', '\n', ')', '4', '&',
  '8', '0', ':', ';', '3', '\"', '$', '?', 0, '6', '!', '/', '-', '2', '\'', 0, '7', '1', '(', 0 };

// 45.45 baud
static const int baudDelayMs = 22;
// Standard Amateur shift
static const int shiftHz = 170;
static const unsigned char MARK_SYM = 1;
static const unsigned char SPACE_SYM = 2;

RttyEncoder::RttyEncoder(SystemEnv* sysEnv, VFOInterface* vfo)
:	_sysEnv(sysEnv),
	_vfo(vfo),
	_shiftState(false),
	_outStreamSize(0),
	_outStreamPtr(0),
	_symbolDurationMs(baudDelayMs) {
}

RttyEncoder::~RttyEncoder() {
}

/**
 * This function is called periodically.  It advances through a state
 * machine.
 */
void RttyEncoder::poll() {
	if (_outStreamSize != 0 &&
		_sysEnv->getTimeMs() - _symbolStartMs >= _symbolDurationMs) {
		// Start the symbol
		_startSymbol(_outStream[_outStreamPtr]);
		_symbolStartMs = _sysEnv->getTimeMs();
		// Advance to the next symbol
		_outStreamPtr++;
		if (_outStreamPtr == _outStreamSize) {
			_outStreamPtr = 0;
		}
	}
}

void RttyEncoder::_startSymbol(unsigned char symbol) {
	// RTTY: Convention: Mark is the upper RF frequency and space is the lower RF frequency.
	// The RTTY "operating frequency" generally coincides with the mark frequency.
	if (symbol == 0) {
		_vfo->setOutputEnabled(false);
	} else if (symbol == MARK_SYM) {
		_vfo->setOutputEnabled(true);
		_vfo->setFrequency(_baseFreqHz);
	} else if (symbol == SPACE_SYM) {
		_vfo->setOutputEnabled(true);
		_vfo->setFrequency(_baseFreqHz - shiftHz);
	}
}

void RttyEncoder::setFreq(unsigned int freqHz) {
	_baseFreqHz = freqHz;
}

void RttyEncoder::setMessage(const char* txt) {
	_outStreamPtr = 0;
	_outStreamSize = 0;
	// Transmit a few mark characters
	_queueMark();
	_queueMark();
	_queueMark();
	// Start the receiver in letter mode
	_txBits(shiftLetters);
	_shiftState = false;
	// Transmit the actual data
	while (*txt != 0) {
		_txChar(*txt);
		txt++;
	}
}

void RttyEncoder::_txChar(char c) {
	// Figure out what character we are dealing with
	int letterIndex = -1;
	int figureIndex = -1;
	for (int i = 0; i < baudotLen; i++) {
		if (letterIndex == -1 && baudotLetters[i] == c) {
			letterIndex = i;
		}
		if (figureIndex == -1 && baudotFigures[i] == c) {
			figureIndex = i;
		}
	}
	if (figureIndex == -1 && letterIndex == -1) {
		// Unrecognized character!  Skip it.
	}
	// Look for case where no shifting is required
	else if (figureIndex == letterIndex) {
		_txBits(figureIndex);
	}
	else if (letterIndex != -1) {
		// Un-shift if needed
		if (_shiftState) {
			_txBits(shiftLetters);
			_shiftState = false;
		}
		_txBits(letterIndex);
	} else if (figureIndex != -1) {
		// Shift if needed
		if (!_shiftState) {
			_txBits(shiftFigures);
			_shiftState = true;
		}
		_txBits(figureIndex);
	}
}

void RttyEncoder::_txBits(int bits) {
	// Start bit
	_queueSpace();
	if (bits & 0x10) _queueMark(); else _queueSpace();
	if (bits & 0x08) _queueMark(); else _queueSpace();
	if (bits & 0x04) _queueMark(); else _queueSpace();
	if (bits & 0x02) _queueMark(); else _queueSpace();
	if (bits & 0x01) _queueMark(); else _queueSpace();
	// Stop bits
	_queueMark();
	_queueMark();
}

void RttyEncoder::_queueMark() {
	if (_outStreamSize < _outStreamCapacity) {
		_outStream[_outStreamSize++] = MARK_SYM;
	}
}

void RttyEncoder::_queueSpace() {
	if (_outStreamSize < _outStreamCapacity) {
		_outStream[_outStreamSize++] = SPACE_SYM;
	}
}

void RttyEncoder::_queueSilence() {
	if (_outStreamSize < _outStreamCapacity) {
		_outStream[_outStreamSize++] = 0;
	}
}





