// Shoop de whoop!

#define OCTAVES_PER_SCALE 8
#define NOTES_PER_SCALE 7

struct note {
	double freq[OCTAVES_PER_SCALE];	// The frequency of the note, index is the octave
	const char *desc;		// Description of note
	char octave;			// Stores a referenced octave
};

struct scale {
	const struct note *n[NOTES_PER_SCALE];	// The notes associated with this scale
	const char *desc;			// Description of scale
};

static const struct note C = {{16.35,32.70, 65.41,130.81,261.63,523.25,1046.50,2093.00},"C",0};
static const struct note D = {{18.35,36.71, 73.42,146.83,293.66,587.33,1174.66,2349.32},"D",0};
static const struct note E = {{20.60,41.20, 82.41,164.81,329.63,659.26,1318.51,2637.02},"E",0};
static const struct note F = {{21.83,43.65, 87.31,174.61,349.23,698.46,1396.91,2793.83},"F",0};
static const struct note G = {{24.50,49.00, 98.00,196.00,392.00,783.99,1567.98,3135.96},"G",0};
static const struct note A = {{27.50,55.00,110.00,220.00,440.00,880.00,1760.00,3520.00},"A",0};
static const struct note B = {{30.87,61.74,123.47,246.94,493.88,987.77,1975.53,3951.07},"B",0};

static const struct scale cmaj = {{&C,&D,&E,&F,&G,&A,&B}, "C Major"};

struct note freq2note(double freq) {
	double up,down;
	int i,j;
	for(i=0;i<=OCTAVES_PER_SCALE;i++) {
		for(j=0;j<=NOTES_PER_SCALE;j++) {
			// Iterate upwards until we pass the upper frequency
			if(freq > cmaj->n[j]->freq[i]) {
				// Find the closest note and return
				up = cmaj->n[j]->freq[i] - freq;
				down = freq - cmaj->n[j-1]->freq[i];
				if(up > down) {
					//scale->n[j].octave = i;
					return cmaj->n[j];
				}
				//scale->n[j].octave = i-1;
				return cmaj->n[j-1];
			}
		}	
	}	
}
