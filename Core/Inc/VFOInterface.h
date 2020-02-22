#ifndef _VFOInterface_h
#define _VFOInterface_h

/**
 * A generic interface for interacting with a VFO
 */
class VFOInterface {

public:
	/**
	 * Used to turn the output on/off
	 */
	virtual void setOutputEnabled(bool e) = 0;

	/**
	 * Used to set the frequency of output
	 */
	virtual void setFrequency(unsigned int freqHz) = 0;
};

#endif 
