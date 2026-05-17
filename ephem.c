#include <u.h>
#include <libc.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

/* 
 * Keplerian Elements mapping semi-major axis (a), eccentricity (e), 
 * inclination (I), mean longitude (L), longitude of perihelion (p), 
 * and longitude of ascending node (O) along with their daily rates.
 */
typedef struct {
	char *name;
	double a, a_dot;
	double e, e_dot;
	double i, i_dot;
	double L, L_dot;
	double p, p_dot;
	double O, O_dot;
} Elements;

/* Fixed planetary orbital variables scaled to the J2000 epoch baseline */
static Elements jpl_data[] = {
	/* Name, a, a_dot, e, e_dot, i, i_dot, L, L_dot, p, p_dot, O, O_dot */
	{"Mercury", 0.387098, 0.0, 0.205630, 0.0, 7.0049, 0.0, 252.2503, 4.0923344, 77.4561, 0.0, 48.3308, 0.0},
	{"Venus",   0.723327, 0.0, 0.006772, 0.0, 3.3946, 0.0, 181.9798, 1.6021303, 131.5637, 0.0, 76.6799, 0.0},
	{"Earth",   1.000002, 0.0, 0.016708, 0.0, 0.0000, 0.0, 100.4664, 0.9856474, 102.9376, 0.0, 0.0000, 0.0},
	{"Mars",    1.523679, 0.0, 0.093400, 0.0, 1.8497, 0.0, 355.4474, 0.5240329, 336.0608, 0.0, 49.5581, 0.0},
	{"Jupiter", 5.202603, 0.0, 0.048497, 0.0, 1.3031, 0.0, 34.3515,  0.0830912, 14.3312,  0.0, 100.4644, 0.0},
	{"Saturn",  9.554909, 0.0, 0.055508, 0.0, 2.4889, 0.0, 50.0757,  0.0333917, 92.8613,  0.0, 113.6399, 0.0}
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

/* Approximate calculation of the Julian Day number relative to the calendar matrix */
double
get_julian_date(int y, int m, int d, double hour)
{
	int a, b;
	if(m <= 2) {
		y -= 1;
		m += 12;
	}
	a = y / 100;
	b = a / 4;
	return (2 - a + b) + d + (int)(365.25 * (y + 4716)) + (int)(30.6001 * (m + 1)) - 1524.5 + (hour / 24.0);
}

/* Solves Kepler's Equation for eccentricity and orbital variance */
double
solve_kepler(double M, double e)
{
	int i;
	double E = M;
	for(i = 0; i < 15; i++) {
		E = E - (E - deg(e * sin(rad(E))) - M) / (1.0 - e * cos(rad(E)));
	}
	return E;
}

void
get_heliocentric(Elements el, double d, double *x, double *y, double *z)
{
	double L = rev(el.L + el.L_dot * d);
	double p = rev(el.p + el.p_dot * d);
	double M = rev(L - p);
	double e = el.e + el.e_dot * d;
	
	double E = solve_kepler(M, e);
	double xv = el.a * (cos(rad(E)) - e);
	double yv = el.a * (sqrt(1.0 - e * e) * sin(rad(E)));
	
	double v = deg(atan2(yv, xv));
	double r = sqrt(xv * xv + yv * yv);
	
	double O_rad = rad(rev(el.O + el.O_dot * d));
	double ip_rad = rad(rev(p + v - el.O));
	double i_rad = rad(el.i + el.i_dot * d);
	
	*x = r * (cos(O_rad) * cos(ip_rad) - sin(O_rad) * sin(ip_rad) * cos(i_rad));
	*y = r * (sin(O_rad) * cos(ip_rad) + cos(O_rad) * sin(ip_rad) * cos(i_rad));
	*z = r * (sin(ip_rad) * sin(i_rad));
}

void
main(int argc, char *argv[])
{
	int year = 1974, month = 5, day = 18;
	double hour = 12.0; 
	double jd, d;
	double ex, ey, ez; 
	double px[5], py[5], pz[5]; /* Safely map exactly 5 planetary slots */
	double geo_long[7]; 
	int i;

	if(argc > 3) {
		year = atoi(argv[1]);
		month = atoi(argv[2]);
		day = atoi(argv[3]);
	}
	if(argc > 4) hour = atof(argv[4]);

	/* Calculate time metrics based on J2000 baseline */
	jd = get_julian_date(year, month, day, hour);
	d = jd - 2451545.0;

	/* 1. Extract Earth heliocentric tracking path baseline */
	get_heliocentric(jpl_data[2], d, &ex, &ey, &ez);

	/* 2. FIXED: Map explicit structural element array offsets perfectly */
	get_heliocentric(jpl_data[0], d, &px[0], &py[0], &pz[0]); /* Mercury */
	get_heliocentric(jpl_data[1], d, &px[1], &py[1], &pz[1]); /* Venus */
	get_heliocentric(jpl_data[3], d, &px[2], &py[2], &pz[2]); /* Mars */
	get_heliocentric(jpl_data[4], d, &px[3], &py[3], &pz[3]); /* Jupiter */
	get_heliocentric(jpl_data[5], d, &px[4], &py[4], &pz[4]); /* Saturn */

	/* 3. Convert Heliocentric paths to clean Geocentric angles relative to Earth */
	geo_long[0] = rev(deg(atan2(-ey, -ex))); /* Sun is Earth's direct inverse vector */
	geo_long[1] = rev(218.316 + 13.176396 * d); /* Moon direct lunar orbit model */

	/* Loop cleanly through the 5 calculated geocentric planet coordinates */
	for(i = 0; i < 5; i++) {
		double rx = px[i] - ex;
		double ry = py[i] - ey;
		geo_long[i+2] = rev(deg(atan2(ry, rx)));
	}

	/* 4. Output space-separated numbers for chart (Sun through Saturn) */
	print(" %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", 
		geo_long[0],  /* Sun */
		geo_long[1],  /* Moon */
		geo_long[2],  /* Mercury */
		geo_long[3],  /* Venus */
		geo_long[4],  /* Mars */
		geo_long[5],  /* Jupiter */
		geo_long[6]); /* Saturn */

	exits(nil);
}
