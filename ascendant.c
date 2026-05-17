#include <u.h>
#include <libc.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

static char *zodiac_lookup[] = {
	"aries", "taurus", "gemini", "cancer", 
	"leo", "virgo", "libra", "scorpio", 
	"sagittarius", "capricorn", "aquarius", "pisces"
};

double
rad(double deg)
{
	return deg * (PI / 180.0);
}

double
deg(double rad)
{
	return rad * (180.0 / PI);
}

double
rev(double deg)
{
	while(deg < 0.0) deg += 360.0;
	while(deg >= 360.0) deg -= 360.0;
	return deg;
}

void
main(int argc, char *argv[])
{
	int year = 1974, month = 5, day = 18;
	double local_hour = 12.0;
	double tz_offset = -8.0; /* Pacific Standard Time baseline */
	double lat = 36.33;      /* NAS Lemoore Latitude (36.20N converted to decimal) */
	double lon = -119.95;    /* NAS Lemoore Longitude (119.57W converted to decimal) */

	double ut, d, jc, gmst, lst, obliquity, ramc, asc_absolute, local_deg;
	int sign_idx;

	/* Parse inputs sequentially: ./ascendant [year] [month] [day] [local_hour] [tz_offset] [lat] [lon] */
	if(argc > 3) {
		year = atoi(argv[1]);
		month = atoi(argv[2]);
		day = atoi(argv[3]);
	}
	if(argc > 4) local_hour = atof(argv[4]);
	if(argc > 5) tz_offset = atof(argv[5]);
	if(argc > 6) lat = atof(argv[6]);
	if(argc > 7) lon = atof(argv[7]);

	/* 1. Convert local time to Universal Time (UT) */
	ut = local_hour - tz_offset;
	if(ut >= 24.0) { ut -= 24.0; day += 1; }
	if(ut < 0.0)   { ut += 24.0; day -= 1; }

	/* 2. Calculate Julian Centuries since J2000 */
	/* Standard astronomical tracking loop formula for days elapsed */
	if(month <= 2) { year -= 1; month += 12; }
	d = (int)(365.25 * (year + 4716)) + (int)(30.6001 * (month + 1)) + day - 1524.5 - 2451545.0;
	jc = d / 36525.0;

	/* 3. Compute Greenwich Mean Sidereal Time (GMST) in absolute degrees */
	gmst = 280.46061837 + 360.98564736629 * d + 0.000387933 * jc * jc - (jc * jc * jc / 38710000.0);
	gmst = rev(gmst + (ut * 15.0)); /* Add the fraction of the current day's rotation */

	/* 4. Calculate Local Sidereal Time (LST) based on your longitude */
	lst = rev(gmst + lon);
	ramc = rad(lst); /* Right Ascension of the Medium Coeli */

	/* True obliquity of the ecliptic plane (axial tilt) */
	obliquity = rad(23.439291 - 0.01300416 * jc);

	/* 5. Solve the Spherical Trigonometry Horizon Intersection Equation */
	/* Finds exactly where the eastern horizon cuts the celestial equator at your latitude */
	asc_absolute = atan2(cos(ramc), -sin(ramc) * cos(obliquity) - tan(rad(lat)) * sin(obliquity));
	asc_absolute = rev(deg(asc_absolute));

	/* Split absolute 360-degree layout coordinate into Sign Name index and sub-degree */
	sign_idx = (int)(asc_absolute / 30.0);
	local_deg = asc_absolute - (sign_idx * 30.0);

	/* Output clean string formatting matching chart4 parameters */
	print("%s %.2f\n", zodiac_lookup[sign_idx], local_deg);

	exits(nil);
}
