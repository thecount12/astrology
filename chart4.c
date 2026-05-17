#include <u.h>
#include <libc.h>

/*
cpu% ./chart4 1974 taurus 15.0 12:00 05/18 36.20N 119.57W 57.0 356.5 74.5 29.0 103.5 343.0 69.0 | pic | troff | page -w

./chart4 1995 taurus 14.25 | pic | troff | page -w
./chart4 1995 taurus 14.25 14:30 05/12 45.50N 73.56W | pic | troff | page -w
*/

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct {
	char *name;
	double tropical_long;
} Planet;

static char *zodiac_signs[] = {
	"Aries", "Taurus", "Gemini", "Cancer", 
	"Leo", "Virgo", "Libra", "Scorpio", 
	"Sagittarius", "Capricorn", "Aquarius", "Pisces"
};

double
get_ayanamsha(int year)
{
	double base_1900 = 22.466388;
	double annual_drift = 0.0139551;
	return base_1900 + (annual_drift * (year - 1900));
}

double
normalize_deg(double deg)
{
	while(deg < 0.0) deg += 360.0;
	while(deg >= 360.0) deg -= 360.0;
	return deg;
}

double
sign_to_absolute(char *sign_name, double local_degree)
{
	int i;
	static char *lookup[] = {
		"aries", "taurus", "gemini", "cancer", 
		"leo", "virgo", "libra", "scorpio", 
		"sagittarius", "capricorn", "aquarius", "pisces"
	};
	for(i = 0; i < 12; i++) {
		if(cistrcmp(sign_name, lookup[i]) == 0) {
			return (i * 30.0) + local_degree;
		}
	}
	return local_degree;
}

void
main(int argc, char *argv[])
{
	int i, j;
	int birth_year = 1995;
	double raw_asc;
	double local_deg = 0.0;
	double aya, sidereal_asc, house_start_deg;
	int asc_sign_index;
	int num_planets = 7;
	double planet_chart_degs[7];

	/* Step 1: Declare your default string text fallbacks */
	char *birth_time = "12:00";
	char *birth_date = "01/01";
	char *latitude   = "0.00N";
	char *longitude  = "0.00W";

	double default_longs[] = {52.1, 212.4, 38.9, 71.2, 145.6, 242.1, 331.8};
	char *names[] = {"Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn"};
	Planet planets[7];

	for(i = 0; i < num_planets; i++) {
		planets[i].name = names[i];
	}

	if(argc > 1) birth_year = atoi(argv[1]);
	if(argc > 3) local_deg = atof(argv[3]);
	if(argc > 2) raw_asc = sign_to_absolute(argv[2], local_deg);
	else raw_asc = sign_to_absolute("aries", 0.0);

	/* 
	 * Bulletproof Context-Aware Planetary Parser 
	 * Reads backwards from the absolute end of your argument array
	 * making it completely immune to argument counting or time string shifts.
	 */
	if(argc > 8) {
		planets[0].tropical_long = atof(argv[argc - 7]); /* Sun */
		planets[1].tropical_long = atof(argv[argc - 6]); /* Moon */
		planets[2].tropical_long = atof(argv[argc - 5]); /* Mercury */
		planets[3].tropical_long = atof(argv[argc - 4]); /* Venus */
		planets[4].tropical_long = atof(argv[argc - 3]); /* Mars */
		planets[5].tropical_long = atof(argv[argc - 2]); /* Jupiter */
		planets[6].tropical_long = atof(argv[argc - 1]); /* Saturn */
	} else {
		/* Safety fallbacks populate ONLY if you pass zero planet coordinates */
		for(i = 0; i < num_planets; i++) {
			planets[i].tropical_long = default_longs[i];
		}
	}

	aya = get_ayanamsha(birth_year);
	sidereal_asc = normalize_deg(raw_asc - aya);
	asc_sign_index = (int)(sidereal_asc / 30.0);
	house_start_deg = asc_sign_index * 30.0;

	/* 
	 * Fixed Direct Index Metadata Header 
	 * Reads text anchors strictly from the front of argv, completely 
	 * unaffected by inline sub-shell expansions.
	 */
	print(".PS\n");
	print("\"\\fBClassical Sidereal Chart\\fP\" at 0,3.6\n");
	
	/* 
	 * argv is ALWAYS your local_hour string
	 * argv is ALWAYS your month/day string 
	 * argv is ALWAYS your latitude string
	 * argv is ALWAYS your longitude string
	 */
	print("\"DOB: %s/%d  Time: %s UT\" at 0,3.4\n", argv[5], birth_year, argv[4]);
	print("\"Loc: %s, %s  Ayanamsha: -%.2f\\(de\" at 0,3.2\n", argv[6], argv[7], aya);
	
	print("\"Sidereal Ascendant: %.2f\\(de %s\" at 0,-3.2\n", 
		sidereal_asc - (asc_sign_index * 30.0), zodiac_signs[asc_sign_index]);


	print("circle radius 2.6 at 0,0\n");
	print("circle radius 1.7 at 0,0\n");
	print("circle radius 1.1 at 0,0\n");

	for(i = 0; i < 12; i++) {
		double spoke_deg = i * 30.0;
		double spoke_rad = spoke_deg * (PI / 180.0);
		print("line from %f,%f to %f,%f\n", 
			1.1 * cos(spoke_rad), 1.1 * sin(spoke_rad), 
			2.6 * cos(spoke_rad), 2.6 * sin(spoke_rad));

		double mid_rad = (spoke_deg + 15.0) * (PI / 180.0);
		print("\"\\s-2%d\\s+2\" at %f,%f\n", i + 1, 0.9 * cos(mid_rad), 0.9 * sin(mid_rad));
		
		int current_sign_index = (asc_sign_index + i) % 12;
		print("\"\\s-3%s\\s+3\" at %f,%f\n", 
			zodiac_signs[current_sign_index], 2.85 * cos(mid_rad), 2.85 * sin(mid_rad));
	}

	for(i = 0; i < num_planets; i++) {
		double sidereal_long = normalize_deg(planets[i].tropical_long - aya);
		planet_chart_degs[i] = normalize_deg(sidereal_long - house_start_deg);
		double planet_rad = planet_chart_degs[i] * (PI / 180.0);
		
		double sign_degree = normalize_deg(sidereal_long);
		int sign_index = (int)(sign_degree / 30.0);
		double exact_rem_deg = sign_degree - (sign_index * 30.0);

		double r_offset = (i % 2 == 0) ? 2.2 : 1.9;
		print("\"\\fB%s\\fP\" at %f,%f\n", planets[i].name, r_offset * cos(planet_rad), r_offset * sin(planet_rad));
		print("\"\\s-4%.1f\\(de\\s+4\" at %f,%f\n", exact_rem_deg, 1.3 * cos(planet_rad), 1.3 * sin(planet_rad));
	}

	for(i = 0; i < num_planets; i++) {
		for(j = i + 1; j < num_planets; j++) {
			double diff = fabs(planet_chart_degs[i] - planet_chart_degs[j]);
			if(diff > 180.0) diff = 360.0 - diff;

			if(diff >= 114.0 && diff <= 126.0) {
				double rad_i = planet_chart_degs[i] * (PI / 180.0);
				double rad_j = planet_chart_degs[j] * (PI / 180.0);
				print("linethick = 2.0\n");
				print("line dashed from %f,%f to %f,%f\n", 
					1.1 * cos(rad_i), 1.1 * sin(rad_i), 1.1 * cos(rad_j), 1.1 * sin(rad_j));
				print("linethick = 1.0\n");
			}
			if(diff >= 84.0 && diff <= 96.0) {
				double rad_i = planet_chart_degs[i] * (PI / 180.0);
				double rad_j = planet_chart_degs[j] * (PI / 180.0);
				print("linethick = 0.5\n");
				print("line dashed from %f,%f to %f,%f\n", 
					1.1 * cos(rad_i), 1.1 * sin(rad_i), 1.1 * cos(rad_j), 1.1 * sin(rad_j));
				print("linethick = 1.0\n");
			}
			/* 1. Opposition Challenge Lines (180 deg with a 6-degree window) */
			/* Represents intense polarity, tension, and direct confrontation */
			if(diff >= 174.0 && diff <= 186.0) {
				double rad_i = planet_chart_degs[i] * (PI / 180.0);
				double rad_j = planet_chart_degs[j] * (PI / 180.0);
				print("linethick = 1.5\n"); /* Medium-thick presence */
				print("line dashed from %f,%f to %f,%f\n", 
					1.1 * cos(rad_i), 1.1 * sin(rad_i), 1.1 * cos(rad_j), 1.1 * sin(rad_j));
				print("linethick = 1.0\n");
			}

			/* 2. Conjunction Blending Markers (0 deg with a 6-degree window) */
			/* Planets are sitting right next to each other, fusing their energies */
			if(diff >= 0.0 && diff <= 6.0) {
				double rad_i = planet_chart_degs[i] * (PI / 180.0);
				double rad_j = planet_chart_degs[j] * (PI / 180.0);
				/* Instead of crossing the core, we draw a tiny indicator loop between them */
				print("arc dashed from %f,%f to %f,%f radius 0.15\n", 
					1.7 * cos(rad_i), 1.7 * sin(rad_i), 1.7 * cos(rad_j), 1.7 * sin(rad_j));
			}
		}
	}

	print(".PE\n");
	exits(nil);
}
