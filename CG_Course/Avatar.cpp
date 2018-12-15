//#include "stdafx.h"
#include "Avatar.h"

/**
* Avatar's Constructor: Initialise the body parts of the Avatar
* @param : NONE
* @return : NONE
*/
Avatar::Avatar()
{
	//Spheres
	_head = new Spheres();
	_elbow[LEFT] = new Spheres();
	_elbow[RIGHT] = new Spheres();
	_knee[LEFT] = new Spheres();
	_knee[RIGHT] = new Spheres();
	_hand[LEFT] = new Spheres();
	_hand[RIGHT] = new Spheres();

	//Cylinders
	_neck = new Cylinder();
	_torso = new Cylinder();
	_shoulder[LEFT] = new Cylinder();
	_shoulder[RIGHT] = new Cylinder();
	_arm[LEFT] = new Cylinder();
	_arm[RIGHT] = new Cylinder();
	_forearm[LEFT] = new Cylinder();
	_forearm[RIGHT] = new Cylinder();
	_hip[LEFT] = new Cylinder();
	_hip[RIGHT] = new Cylinder();
	_thigh[LEFT] = new Cylinder();
	_thigh[RIGHT] = new Cylinder();
	_leg[LEFT] = new Cylinder();
	_leg[RIGHT] = new Cylinder();
	_foot[LEFT] = new Cylinder();
	_foot[RIGHT] = new Cylinder();

	// avatar initialization

	_head->buildSphere(1.5, 100, 100);
	_elbow[LEFT]->buildSphere(0.55, 100, 100);
	_elbow[RIGHT]->buildSphere(0.55, 100, 100);
	_knee[LEFT]->buildSphere(0.80, 100, 100);
	_knee[RIGHT]->buildSphere(0.80, 100, 100);
	_hand[LEFT]->buildSphere(0.40, 100, 100);
	_hand[RIGHT]->buildSphere(0.40, 100, 100);

	_torso->buildCylinder(2.5, 3.75, 7.0, 100, 1, 2.25);

	_neck->buildCylinder(_head->_radius / 2, _head->_radius / 2, 1.5, 100, 1, 1);

	_arm[LEFT]->buildCylinder(0.5, 0.5, 4.5, 100, 1, 1);
	_arm[RIGHT]->buildCylinder(0.5, 0.5, 4.5, 100, 1, 1);

	_shoulder[LEFT]->buildCylinder(_torso->_topRadius, 0.55, _torso->_topRadius + _arm[LEFT]->_topRadius * 2, 100, 2.25, 3);
	_shoulder[RIGHT]->buildCylinder(_torso->_topRadius, 0.55, _torso->_topRadius + _arm[LEFT]->_topRadius * 2, 100, 2.25, 3);

	_forearm[LEFT]->buildCylinder(0.5, 0.25, 4.5, 100, 1, 1);
	_forearm[RIGHT]->buildCylinder(0.5, 0.25, 4.5, 100, 1, 1);


	/*_hip[LEFT]->buildCylinder(_torso->_baseRadius, 0.55, _torso->_baseRadius, 100, 2.25, 3);
	_hip[RIGHT]->buildCylinder(_torso->_baseRadius, 0.55, _torso->_baseRadius, 100, 2.25, 3);
	*/
	_thigh[LEFT]->buildCylinder(1.25, 0.75, 5.0, 100, 1, 1);
	_thigh[RIGHT]->buildCylinder(1.25, 0.75, 5.0, 100, 1, 1);
	_leg[LEFT]->buildCylinder(0.75, 0.25, 5.0, 100, 1, 1);
	_leg[RIGHT]->buildCylinder(0.75, 0.25, 5.0, 100, 1, 1);
	_hip[LEFT]->buildCylinder(_thigh[LEFT]->_topRadius, _thigh[LEFT]->_topRadius, _torso->_baseRadius, 100, 1, 1);
	_hip[RIGHT]->buildCylinder(_thigh[RIGHT]->_topRadius, _thigh[RIGHT]->_topRadius, _torso->_baseRadius, 100, 1, 1);

	_foot[LEFT]->buildCylinder(0.5, 0.25, 2, 100, 1, 3);
	_foot[RIGHT]->buildCylinder(0.5, 0.25, 2, 100, 1, 3);

	_Tangle = 0.;

	_LAangle = M_PI;
	_LFAangle = 0.;

	_RAangle = M_PI;
	_RFAangle = 0.;

	_LLangle = 0.;
	_LTangle = 0.;

	_RLangle = M_PI;
	_RTangle = M_PI;

	_head->InitVBO();
	_elbow[LEFT]->InitVBO();
	_elbow[RIGHT]->InitVBO();
	_knee[LEFT]->InitVBO();
	_knee[RIGHT]->InitVBO();
	_hand[LEFT]->InitVBO();
	_hand[RIGHT]->InitVBO();

	_torso->InitVBO();
	_neck->InitVBO();
	_shoulder[LEFT]->InitVBO();
	_shoulder[RIGHT]->InitVBO();
	_arm[LEFT]->InitVBO();
	_arm[RIGHT]->InitVBO();
	_forearm[LEFT]->InitVBO();
	_forearm[RIGHT]->InitVBO();
	_hip[LEFT]->InitVBO();
	_hip[RIGHT]->InitVBO();
	_thigh[LEFT]->InitVBO();
	_thigh[RIGHT]->InitVBO();
	_leg[LEFT]->InitVBO();
	_leg[RIGHT]->InitVBO();
	_foot[LEFT]->InitVBO();
	_foot[RIGHT]->InitVBO();
}

/**
* Avatar's Destructor: Destroy the body parts of the Avatar
* @param : NONE
* @return : NONE
*/
Avatar::~Avatar()
{
	//Sphere deleting
	delete _head;
	delete _elbow[LEFT];
	delete _elbow[RIGHT];
	delete _knee[LEFT];
	delete _knee[RIGHT],
	delete _hand[LEFT];
	delete _hand[RIGHT];

	//Cylinder deleting
	delete _torso;
	delete _neck;
	delete _shoulder[LEFT];
	delete _shoulder[RIGHT];
	delete _arm[LEFT];
	delete _arm[RIGHT];
	delete _forearm[LEFT];
	delete _forearm[RIGHT];
	delete _hip[LEFT];
	delete _hip[RIGHT];
	delete _thigh[LEFT];
	delete _thigh[RIGHT];
	delete _leg[LEFT];
	delete _leg[RIGHT];
	delete _foot[LEFT];
	delete _foot[RIGHT];
}

/**
* Avatar's Dodge Update Function: Update Avatar's angles for animation purpose
* @param t: a double representing time
* @return : NONE
*/
void Avatar::updateDodge(double t) {
	
	// Animation stops from 21 seconds
	if (_LTangle < 3*M_PI / 2 - 0.1)
	{
		// Torso and thighs animation: the "main" dodging
		_Tangle += 0.00002f;
		_RTangle += 0.00025f;

		// Right arm and forearm (remember: right <-> left) animation
		if (t < 1.2f)
		{
			_LAangle += 0.005f;
			_LFAangle -= 0.0007f;
		}
		else if (t < 3.5f)
		{
			_LAangle += 0.001f;
			_LFAangle += 0.0002f;
		}
		else
		{
			if (_LFAangle >(-M_PI / 15.f))
			{
				_LFAangle -= 0.0003f;
			}
			_LAangle -= 0.0006f;
		}


		// Left arm and forearm (remember: right <-> left) animation
		if (t < 1.f)
		{
			_RAangle -= 0.003f;
		}
		else if (t < 7.f)
		{
			_RAangle += 0.0008f;
			_RFAangle += 0.0001f;
		}
		else
		{
			_RAangle += 0.0008f;
			_RFAangle -= 0.0002f;
		}
	}

	// NOTE: the thighs move accordingly
	_LTangle = _RTangle;
}

/**
* Avatar's Fail Update Function: Update Avatar's angles for animation purpose
* @param b1: a boolean representing the impact of the first bullet
* @param b2: a boolean representing the impact of the second bullet
* @param b3: a boolean representing the impact of the third bullet
* @return : NONE
*/
void Avatar::update(bool b1, bool b2, bool b3) {

	// Animation stops from 9.5 seconds
	if (b1==1 && b2==0 && b3==0)
	{

		_Tangle += 0.0003f;
		_LAangle += 0.0003f;

	}
	else if (b1 == 1 && b2 == 1 && b3 == 0) {
		_Tangle += 0.0006f;
		_LAangle += 0.0008f;
	}
	else if (b1 == 1 && b2 == 1 && b3 == 1 && _RTangle <= (M_PI + M_PI / 2)) {
		_Tangle -= 0.0006f;
		_RTangle += 0.0008f;
		_LLangle += 0.0008f;
		_LAangle -= 0.0007f;
	}

	// NOTE: the thighs move accordingly
	_LTangle = _RTangle;
	_RLangle = _LLangle;
	_RAangle = _LAangle;


}
