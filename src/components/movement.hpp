#pragma once

struct MovementComponent{
    float   vX{}, 
            vY{}, 
            vZ{};
    float   vLinear{};
    bool    moving{};
	double 	movOrientation{};
    double	accel{550.0};
	double 	friction{0.80};
    double  gravity{98.0},
            maxGravity{980.0};
    bool    tryToRun{false},
            run{false};

	double	deadMov{0.00001};
    float 	maxVelocity{36.0},
            maxRunVelocity{54.0};
};