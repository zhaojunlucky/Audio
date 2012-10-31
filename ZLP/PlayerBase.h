#pragma once
enum PlayMode
{
	PMSingle,
	PMOrder,
	PMCircle,
	PMRandom
};

typedef struct 
{
	int volume;
	bool mute;
	PlayMode pm;
}PlayerConfig;