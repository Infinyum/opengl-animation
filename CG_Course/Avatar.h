#pragma once

#include "Cylinder.h"
#include "Spheres.h"

class Avatar
{
public:
	Avatar();
	~Avatar();

	void updateDodge(double);
	void update(bool b1, bool b2, bool b3);


	enum part { LEFT = 0, RIGHT = 1 };

	// avatar components and animation
	Spheres *_head, *_elbow[2], *_knee[2], *_hand[2];
	Cylinder *_neck, *_shoulder[2], *_torso, *_arm[2], *_forearm[2],
		*_hip[2], *_thigh[2], *_leg[2], *_foot[2];

	

	float _Yposition;
	// ANIMATION ANGLES
	double _Tangle; // Torse angle

	double _LAangle; // Left arm angle
	double _LFAangle; // Left forearm angle

	double _RAangle; // Right arm angle
	double _RFAangle; // Right forearm angle

	double _LLangle; // Left leg angle
	double _LTangle; // Left thigh angle

	double _RLangle; // Right leg angle
	double _RTangle; // Right thigh angle
};