static const Block blocks[] = {
	/* Icon          Command                                                      Update Interval   Update Signal */
	{ " 💾 Mem: ",   "free -h | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g",	30,		         0 },
	{ " 📅 Date: ",  "date '+%b %d (%a)'",					                           60,		         0 },
   { " 🕛 Time: ",  "date '+%I:%M %p '",					                           60,	            0 },
};

/*
   Sets delimeter between status commands.
   A NULL character ('\0') means no delimeter.
*/
static char delim[] = " | ";
static unsigned int delimLen = 5;
