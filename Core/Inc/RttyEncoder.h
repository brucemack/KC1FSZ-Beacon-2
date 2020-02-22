/*
 * RttyEncoder.h
 *
 *  Created on: Feb 22, 2020
 *      Author: Bruce MacKinnon KC1FSZ
 */

#ifndef SRC_RTTYENCODER_H_
#define SRC_RTTYENCODER_H_

#include "SystemEnv.h"
#include "VFOInterface.h"
#include "Pollable.h"

class RttyEncoder : Pollable {

public:

	RttyEncoder(SystemEnv* sysEnv, VFOInterface* vfo);
	virtual ~RttyEncoder();

	void poll();

	void setFreq(unsigned int freqHz);
	void setMessage(const char* message);
	void setDelay(int delaySeconds);

private:

	void _startSymbol(unsigned char symbol);
	void _txChar(char c);
	void _txBits(int bits);
	void _queueMark();
	void _queueSpace();
	void _queueSilence();

	SystemEnv* _sysEnv;
	VFOInterface* _vfo;
	bool _shiftState;
	unsigned int _baseFreqHz;
	static const unsigned int _outStreamCapacity = 2048;
	unsigned char _outStream[_outStreamCapacity];
	unsigned int _outStreamSize;
	unsigned int _outStreamPtr;
	unsigned int _symbolDurationMs;
	unsigned long _symbolStartMs;
	int _delaySeconds;
};

#endif /* SRC_RTTYENCODER_H_ */
