#include <u.h>
#include <libc.h>

/*

# Testing 12-hour clock format with an AM marker
cpu% ./hms 4:16 am
4.2667

# Testing 24-hour military clock fallback format
cpu% ./hms 16:30
16.5000

*/

void
main(int argc, char *argv[])
{
	char *time_str;
	char *ampm_str = "";
	char *p;
	char hbuf[10], mbuf[10];
	int h, m;
	double decimal_hours;

	if(argc < 2) {
		fprint(2, "usage: hms hh:mm [am|pm]\n");
		exits("usage");
	}

	time_str = argv[1];
	if(argc > 2)
		ampm_str = argv[2];

	/* Find the colon separator */
	p = strchr(time_str, ':');
	if(p == nil) {
		fprint(2, "hms: invalid time format, missing colon\n");
		exits("format");
	}

	/* Extract hour string component safely */
	if((p - time_str) >= sizeof(hbuf))
		exits("overflow");
	strncpy(hbuf, time_str, p - time_str);
	hbuf[p - time_str] = '\0';
	h = atoi(hbuf);

	/* Extract minute string component safely */
	p++; /* skip past the colon */
	if(strlen(p) >= sizeof(mbuf))
		exits("overflow");
	strcpy(mbuf, p);
	m = atoi(mbuf);

	/* Validate raw integer boundaries */
	if(h < 0 || h > 23 || m < 0 || m > 59) {
		fprint(2, "hms: hours must be 0-23, minutes 0-59\n");
		exits("bounds");
	}

	/* Handle 12-hour clock AM/PM adjustment logic */
	if(cistrcmp(ampm_str, "pm") == 0) {
		if(h < 12) h += 12;
	} else if(cistrcmp(ampm_str, "am") == 0) {
		if(h == 12) h = 0;
	}

	/* Compute precise floating-point decimal hours */
	decimal_hours = (double)h + ((double)m / 60.0);

	/* Print raw decimal result to standard output */
	print("%.4f\n", decimal_hours);
	exits(nil);
}
