#include <u.h>
#include <libc.h>
/*
usage: ./sample 1995 | pic | troff | page -w

*/

/* Structure to hold planetary positions */
typedef struct {
	char *name;
	double tropical_long; /* 0 to 360 degrees */
} Planet;

/* Calculates Lahiri Ayanamsha based on birth year */
double
get_ayanamsha(int year)
{
	double base_1900 = 22.466388;      /* 22° 27' 59" */
	double annual_drift = 0.0234722;   /* 1' 24.5" in decimal degrees */
	return base_1900 + (annual_drift * (year - 1900));
}

void
main(int argc, char *argv[])
{
	int i, birth_year;
	double angle, aya, sidereal_long, rad;
	
	/* Sample dataset: Sun at 45.0° (Taurus), Moon at 120.0° (Leo) */
	Planet planets[] = {
		{"Sun", 45.0},
		{"Moon", 120.0},
		{"Mars", 215.5}
	};
	int num_planets = sizeof(planets) / sizeof(planets[0]);

	/* Default to current year if no argument is passed */
	birth_year = 2026;
	if(argc > 1)
		birth_year = atoi(argv[1]);

	aya = get_ayanamsha(birth_year);

	print(".PS\n");
	/* Draw the bounding framework */
	print("circle radius 2.5 at 0,0\n");
	print("circle radius 1.5 at 0,0\n");

	/* Draw the 12 house division spokes */
	for(i = 0; i < 12; i++) {
		angle = (i * 30.0) * (PI / 180.0);
		print("line from %f,%f to %f,%f\n", 
			1.5 * cos(angle), 1.5 * sin(angle), 
			2.5 * cos(angle), 2.5 * sin(angle));
	}

	/* Calculate and plot the sidereal planets */
	for(i = 0; i < num_planets; i++) {
		/* Apply the non-tropical adjustment */
		sidereal_long = planets[i].tropical_long - aya;
		if(sidereal_long < 0.0)
			sidereal_long += 360.0;

		/* Convert the absolute degree to radians for layout placement */
		angle = sidereal_long * (PI / 180.0);

		/* Radius 2.0 sits perfectly between the 1.5 and 2.5 circles */
		rad = 2.0; 
		print("\"%s\" at %f,%f\n", 
			planets[i].name, rad * cos(angle), rad * sin(angle));
	}

	print(".PE\n");
	exits(nil);
}


