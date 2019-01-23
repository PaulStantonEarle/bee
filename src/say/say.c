/*
 * Command line text-to-speech interface for Carbon / Darwin / OS X
 *
 *   Scott "Jerry" Lawrence
 *   jsl@umlautllama.com
 *   http://www.umlautllama.com
 *
 *   Use this for whatever you like.  No copyrights held.
 *   If it's useful for you, run with it.  :]
 *
 * Ref: http://developer.apple.com/techpubs/macosx/Carbon/multimedia/SpeechSynthesisManager/speechsynthesis.html
 *
 *  for fun:
 *    % date "+It is now %l:%M on %A %B %m %Y" | ./say -v r -q
 */

/*
2002 May 13    1.1
	added random male/female/neutral voice
 
2002 April 18  1.0
	initial release
 */

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "ApplicationServices.h"

/*
 *  globals that parseCommandLine() will read in
 */
int listvoices   = 0;  /* list voices and exit */
int usageflag    = 0;  /* display usage */
int overspeak    = 0;  /* overspeak */
int noisy        = 1;  /* lots of output */

int voice_number = 0;  /* 0 denotes default */
double rate      = 0;  /* rate of speech */
double pitch     = 0;  /* pitch of speech */

enum {
	fm_EOF = 0,	/* read to EOF before starting */
	fm_line,	/* line by line */
	fm_paragraph,	/* paragraph by paragraph */
	fm_sentence     /* sentence by sentence */
};
char * filename  = NULL;    /* input filename */
int    filemode  = fm_EOF;  /* mode of file reading */



/*
 * pickRandomVoice
 *
 *   Goes through the voice list, and picks a random male, female 
 *   or neutral voice.  If none was found, it returns 0
 */
#define V_NEUTRAL (1)
#define V_MALE    (2)
#define V_FEMALE  (4)
int
pickRandomVoice(
	int whichType
)
{
    VoiceSpec voicespec;
    OSErr retval = 0;
    SInt16 numvoices;
    SInt16 c;

    int select = 0;

    int male = 0;
    int female = 0;
    int neutral = 0;

    /* find out how many voices we have, first of all */
    retval = CountVoices( &numvoices );
    
    /* loop iteration 1: find the voice types */
    /* see if whe can get some info about these voices */
    for( c = 1 ; c <= numvoices ; c++ )
    {
	VoiceDescription description;

	/* get a handle to this voice */
	retval = GetIndVoice( c, &voicespec );

	/* retrieve the description for it */
	retval = GetVoiceDescription( &voicespec, 
			&description, 
			sizeof(VoiceDescription) );

	if (description.gender == kNeuter)
	    neutral++;
	else if (description.gender == kMale)
	    male++;
	else
	    female++;
    }

    /* select the random one. */
    select = 0;
    if ( whichType & V_MALE )
	select += 1+(random() % (male-1));
    if ( whichType & V_FEMALE )
	select += 1+(random() % (female-1));
    if ( whichType & V_NEUTRAL )
	select += 1+(random() % (neutral-1));

    /* loop iteration 2: select the correct voice number */
    /* now that we have the count, select the correct one.. */
    for( c = 1 ; c <= numvoices ; c++ )
    {
	VoiceDescription description;

	/* get a handle to this voice */
	retval = GetIndVoice( c, &voicespec );

	/* retrieve the description for it */
	retval = GetVoiceDescription( &voicespec, 
			&description, 
			sizeof(VoiceDescription) );

	if ( (description.gender == kNeuter) && (whichType & V_NEUTRAL) )
	    select--;

	if ( (description.gender == kMale) && (whichType & V_MALE) )
	    select--;

	if ( (description.gender == kFemale) && (whichType & V_FEMALE) )
	    select--;

	if (select <= 0)
	{
	    return( c );
	}
    }

    return( 0 );
}

/*
 * parseCommandLine
 *
 *   Read through the argument list, looking for flags and options
 */
void
parseCommandLine(
	int argc,
	char **argv
)
{
    int c = 0;

    while (argv[c] != NULL  &&  c<argc)
    {
	if (argv[c][0] == '-')
	{
	    switch( argv[c][1] )
	    {
	    /* -l  list voices */
	    case('l'):
		listvoices = 1;
		break;

	    /* -v  voice selection */
	    case('v'):
		{
		    SInt16 numvoices;
		    int    usersel;
		    OSErr retval = CountVoices( &numvoices );
		    srandom( (unsigned) time(NULL));

		    c++;
		    if (argv[c] == NULL) break;

		    if (argv[c][0] == 'r') {
			/* random voice */
			voice_number = 1+(random() % (numvoices-1));

		    } else if (argv[c][0] == 'm') { /* random male voice */
			voice_number = pickRandomVoice( V_MALE );

		    } else if (argv[c][0] == 'M') { /* random non-male voice */
			voice_number = pickRandomVoice( V_FEMALE | V_NEUTRAL );

		    } else if (argv[c][0] == 'f') {/* random female voice */
			voice_number = pickRandomVoice( V_FEMALE );

		    } else if (argv[c][0] == 'F') {/* random non-female voice */
			voice_number = pickRandomVoice( V_MALE | V_NEUTRAL );

		    } else if (argv[c][0] == 'n') { /* random neutral voice */
			voice_number = pickRandomVoice( V_NEUTRAL );

		    } else if (argv[c][0] == 'N') { /* random human voice */
			voice_number = pickRandomVoice( V_MALE | V_FEMALE );

		    } else {
			/* select a voice */
			usersel = atoi(argv[c]);
			if (usersel < 1 || usersel > numvoices)
			{
			    voice_number = 0;
			} else {
			    voice_number = usersel;
			}
		    }
		}
		break;

	    /* -r  set the rate */
	    case('r'):
		c++;
		if (argv[c] == NULL) break;
		rate = atof(argv[c]);
		break;

	    /* -p  set the pitch */
	    case('p'):
		c++;
		if (argv[c] == NULL) break;
		pitch = atof(argv[c]);
		break;

	    /* -q  turn off textual output */
	    case('q'):   noisy = 0;      break;

	    /* -o  turn on overspeak mode */
	    case('o'):   overspeak = 1;  break;

	    /* -f  specify an input filename */
	    case('f'):
		c++;
		if (argv[c] == NULL) break;
		filename = argv[c];
		break;

	    /* -F?  specify an file read mode */
	    case('F'):
		switch( argv[c][2] )
		{
		    /* -Fl  line by line */
		    case('l'):  filemode = fm_line;       break;

		    /* -Fp  paragraph by paragraph */
		    case('p'):  filemode = fm_paragraph;  break;

		    /* -Fs  sentence by sentence */
		    case('s'):  filemode = fm_sentence;   break;
		}
		break;

	    /* -?  dunno what it was,  show the usage */
	    default:
		usageflag = 1;
		break;
	    }
	} 

	if (argv[c] != NULL)
	    c++;
    }
}



/*
 * listVoiceNames
 *
 *   Display all of the voices in the system with their selector numbers.
 */
void
listVoiceNames(
	void
)
{
    VoiceSpec voicespec;
    OSErr retval = 0;
    SInt16 numvoices;
    SInt16 c;
    int n = 0;
    int p = 0;

    /* let's check out what we've got for voices... */
    retval = CountVoices( &numvoices );
    
    /* see if whe can get some info about these voices */
    for( c = 1 ; c <= numvoices ; c++ )
    {
	VoiceDescription description;

	/* get a handle to this voice */
	retval = GetIndVoice( c, &voicespec );

	/* retrieve the description for it */
	retval = GetVoiceDescription( &voicespec, 
			&description, 
			sizeof(VoiceDescription) );

	/* since the names seem to be pascal strings with a number at the 
           beginning of the string, we'll just only print what's valid ascii */
	printf( " %2d   %c", c,
		(description.gender == kNeuter)? ' ' :
		(description.gender == kMale)? 'M' : 'F'
		);

	/* print out age info */
	if (description.gender != kNeuter)
	{
	    printf( " %2d ",  description.age );
	} else {
	    printf( "    " );
	}

	/* and the name */
	n = 0;
	for( p = 0 ; p < strlen( description.name ) ; p++ )
	{
	    if( isalnum( description.name[p] )) n++;
	    if( n>0 )
		printf("%c", description.name[p]);
	}
	printf( "\n" );
    }
}



/* 
 * usage
 *
 *   display command usage to stderr
 */
void
usage(
	void
)
{
    fprintf( stderr,  
	"Say v1.1  2002-May-13\n"
        "\tby Scott \"Jerry\"Lawrence\n"
        "\tjsl@umlautllama.com\n"
        "\thttp://www.umlautllama.com\n"
	"\n");

    fprintf( stderr, 
	"Usage:   say [options]\n"
	"Options:\n"
	    "\t-l\tlist all available voices\n"
	    "\t-v N\tselect voice N where N is one of the above\n"
	    "\t-v 0\tselect the default system voice (default)\n"
	    "\t-v r\tselect a random voice\n"
	    "\t-v m\tselect a random male voice\n"
	    "\t-v f\tselect a random female voice\n"
	    "\t-v n\tselect a random neutral voice\n"
	    "\t-v M\tselect a random non-male voice\n"
	    "\t-v F\tselect a random non-female voice\n"
	    "\t-v N\tselect a random non-neutral voice\n"
	    "\n"
	    "\t-r N\tchange the rate to N words per minute (100-300 roughly)\n"
	    "\t-p N\tchange the pitch to N (40.0 to 60.0 roughly)\n"
	    "\n"
	    "\t-q\tquiet down the text output\n"
	    "\t-o\toverspeak: say the new thing before the previous ends\n"
	    "\n"
	    "\t-f F\tselect filename F for input instead of standard input\n"
/*  ** THESE ARE UNSUPPORTED IN 1.0 **
	    "\t-Fl\tsay each line as it comes in (default)\n"
	    "\t-Fp\tsay each paragraph as it comes in (unsupported)\n"
	    "\t-Fs\tsay each sentence as it comes in (unsupported)\n"
*/
    );
}



/*
 * main
 *
 *   main.  duh.
 */
int
main(
	int argc,
	char ** argv
)
{
    OSErr retval = 0;
    char buf[255];
    SpeechChannel speechchannel;
    VoiceSpec voicespec;
    SInt16 numvoices;
    OSType creator;
    OSType id;
    Fixed valueF;
    SInt16 c;
    FILE * inputFile = stdin;


    /* check the command line for options */
    parseCommandLine(argc, argv);

    /* dump out usage and exit, if we need to */
    if (usageflag)
    {
	usage();
	return( 0 );
    }

    /* dump out voice list and exit, if we need to */
    if (listvoices)
    {
	listVoiceNames();
	return( 0 );
    }


    /* open the input file, and bail out early on failure */
    if (filename)
    {
	inputFile = fopen( filename, "r" );
	if (!inputFile)
	{
	    fprintf( stderr, "%s: Unable to open input file.\n", filename);
	    return( -1 );
	}
    }

    /* retrieve the selected voice */
    if (voice_number > 0)
    {
	/* select the new voice */
	retval = GetIndVoice( voice_number, &voicespec );

	/* create the speech channel */
	NewSpeechChannel( &voicespec, &speechchannel );
    } else {
	/* create the speech channel with a random voice */
	NewSpeechChannel( NULL, &speechchannel );
    }


    /* set the pitch */
    if (pitch > 0.0)
    {
	valueF = X2Fix(pitch);
	SetSpeechPitch( speechchannel, valueF );
    }

    /* set the rate */
    if (rate > 0.0)
    {
	valueF = X2Fix(rate);
	SetSpeechRate( speechchannel, valueF );
    }

    /* dump out some information that the user might want to know */
    if (noisy)
    {
	(void) GetSpeechPitch( speechchannel, &valueF );
	printf("pitch: %5.2f\n", Fix2X(valueF));

	(void) GetSpeechRate( speechchannel, &valueF );
	printf(" rate: %5.2f\n", Fix2X(valueF));

	if (voice_number)
	    printf("voice: %d\n", voice_number );
	else
	    printf("voice: default system voice\n");

	printf("[ctrl]-[d] or EOF to end.\n");
    }

    /* read the input and speak it */
    buf[0] = '\0';
    while (1)
    {
	/*
	if (noisy) printf("? "); fflush(stdout);
	*/

	/* this only works on a line-by-line basis right now... */

	if(! fgets(buf, 255, inputFile))
	    break;

	/* speak the text */
	retval = SpeakText( speechchannel, buf, strlen(buf));
	/*
	retval = SpeakBuffer( speechchannel, buf, strlen(buf), 
		kNoEndingProsody|kNoSpeechInterrupt);
	*/

	/* wait for the speech to complete */
	if (!overspeak)
	    while (SpeechBusy());
    }
    if (noisy) printf("\n");

    /* destroy the speech channel */
    DisposeSpeechChannel(speechchannel);

    /* close the input file */
    fclose( inputFile );

    return( 0 );
}
