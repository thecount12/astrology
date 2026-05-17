#include <u.h>
#include <libc.h>
/*
usage: ./chart 1995 45.0 | pic | troff | page -w

*/

/* Safely guard PI if Plan 9's headers already defined it */
#ifndef PI
#define PI 3.14159265358979323846
#endif

/* Data structure for planetary inputs */
typedef struct {
	char *name;
	double tropical_long; /* Absolute degrees 0.0 to 360.0 */
} Planet;

/* Lookup tables for classical zodiac names */
static char *zodiac_signs[] = {
	"Aries", "Taurus", "Gemini", "Cancer", 
	"Leo", "Virgo", "Libra", "Scorpio", 
	"Sagittarius", "Capricorn", "Aquarius", "Pisces"
};

/* 
 * Calculates the Lahiri Ayanamsha mathematical offset.
 * Base: Jan 1, 1900 is anchored at 22.466388 decimal degrees.
 * Annual precision drift rate is 50.2384 seconds of arc (0.0139551 degrees/year).
 */
double
get_ayanamsha(int year)
{
	double base_1900 = 22.466388;
	double annual_drift = 0.0139551;
	return base_1900 + (annual_drift * (year - 1900));
}

/* Normalizes any angle calculation to a strict 0.0 to 360.0 coordinate ring */
double
normalize_deg(double deg)
{
	while(deg < 0.0)
		deg += 360.0;
	while(deg >= 360.0)
		deg -= 360.0;
	return deg;
}

void
main(int argc, char *argv[])
{
	int i;
	int birth_year = 1995; /* Default fallback year */
	double raw_asc = 45.0;  /* Default Tropical Ascendant (15 deg Taurus) */
	double aya, sidereal_asc;
	int asc_sign_index;
	double house_start_deg;

	/* 
	 * Comprehensive traditional planetary tracking array.
	 * Input requires raw, uncorrected observational (Tropical) data.
	 */
	Planet planets[] = {
		{"Sun", 52.1},    /* Sample Tropical Positions */
		{"Moon", 212.4},
		{"Mercury", 38.9},
		{"Venus", 71.2},
		{"Mars", 145.6},
		{"Jupiter", 242.1},
		{"Saturn", 331.8}
	};
	int num_planets = sizeof(planets) / sizeof(planets[0]);

	/* Process command line parameters: ./chart [year] [tropical_ascendant_deg] */
	if(argc > 1)
		birth_year = atoi(argv[1]);
	if(argc > 2)
		raw_asc = atof(argv[2]);

	/* Compute structural corrections */
	aya = get_ayanamsha(birth_year);
	sidereal_asc = normalize_deg(raw_asc - aya);

	/* 
	 * Classical Whole Sign House logic:
	 * Find which 30-degree constellation contains the Sidereal Ascendant.
	 * That constellation becomes the entire 1st house, starting at 0.0 degrees.
	 */
	asc_sign_index = (int)(sidereal_asc / 30.0);
	house_start_deg = asc_sign_index * 30.0;

	/* Begin rendering structural layout vectors via pic macro tags */
	print(".PS\n");
	
	/* Output Chart Metadata Titles */
	print("\"\\fBClassical Sidereal Chart\\fP\" at 0,3.3\n");
	print("\"Year: %d | Lahiri Ayanamsha: -%.2f\\(de\" at 0,3.1\n", birth_year, aya);
	print("\"Sidereal Ascendant: %.2f\\(de %s\" at 0,-3.2\n", 
		sidereal_asc - (asc_sign_index * 30.0), zodiac_signs[asc_sign_index]);

	/* Draw Main Concentric Structural Framework */
	print("circle radius 2.6 at 0,0\n"); /* Outer boundary rim */
	print("circle radius 1.7 at 0,0\n"); /* Planet track boundary */
	print("circle radius 1.1 at 0,0\n"); /* Inner house track core */

	/* Render 12 Symmetrical Whole-Sign House Spokes and Constellation Markers */
	for(i = 0; i < 12; i++) {
		/* Calculate relative orientation angle aligned to the Ascendant house boundary */
		double spoke_deg = i * 30.0;
		double spoke_rad = spoke_deg * (PI / 180.0);

		/* Print geometric boundary vectors */
		print("line from %f,%f to %f,%f\n", 
			1.1 * cos(spoke_rad), 1.1 * sin(spoke_rad), 
			2.6 * cos(spoke_rad), 2.6 * sin(spoke_rad));

		/* Calculate centered midpoints inside each house sector */
		double mid_rad = (spoke_deg + 15.0) * (PI / 180.0);

		/* Map sequential house numbers (1 to 12) inside the innermost tracking circle */
		print("\"\\s-2%d\\s+2\" at %f,%f\n", i + 1, 0.9 * cos(mid_rad), 0.9 * sin(mid_rad));

		/* Map the corresponding Sidereal Zodiac Constellation names on the outer edge */
		int current_sign_index = (asc_sign_index + i) % 12;
		print("\"\\s-3%s\\s+3\" at %f,%f\n", 
			zodiac_signs[current_sign_index], 2.85 * cos(mid_rad), 2.85 * sin(mid_rad));
	}

	/* Map Sidereal Planets into the Whole Sign Framework */
	for(i = 0; i < num_planets; i++) {
		double sidereal_long = normalize_deg(planets[i].tropical_long - aya);
		
		/* 
		 * Calculate structural placement displacement relative to the chart's 
		 * Ascendant house offset angle baseline.
		 */
		double chart_relative_deg = normalize_deg(sidereal_long - house_start_deg);
		double planet_rad = chart_relative_deg * (PI / 180.0);
		
		/* Calculate localized house coordinate details */
		int planet_house = (int)(chart_relative_deg / 30.0) + 1;
		double sign_degree = normalize_deg(sidereal_long);
		int sign_index = (int)(sign_degree / 30.0);
		double exact_rem_deg = sign_degree - (sign_index * 30.0);

		/* Print the planet name and precise location details onto the tracking space */
		print("\"\\fB%s\\fP\" at %f,%f\n", planets[i].name, 2.1 * cos(planet_rad), 2.1 * sin(planet_rad));
		print("\"\\s-4%.1f\\(de\\s+4\" at %f,%f\n", exact_rem_deg, 1.4 * cos(planet_rad), 1.4 * sin(planet_rad));
	}

	print(".PE\n");
	exits(nil);
}
