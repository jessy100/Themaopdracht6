/*!
*   @author     Jessy Visch, Zehna van den berg, Koen de Groot, Joost Wagensveld
*   @project    Themaopdracht 6, Wasmachine
*   @brief      Abstract interface of sensors
*/

#ifndef _UPDATINGSENSOR_H
#define _UPDATINGSENSOR_H

/** \class UpdatingSensor
	* \brief Abstract interface of the sensors.
*/
class UpdatingSensor{
public:
	virtual void update() = 0;
};

#endif