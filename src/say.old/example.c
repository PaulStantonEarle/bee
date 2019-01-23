/*
 * Sample code to speak using Carbon/Darwin
 *
 *   Scott "Jerry" Lawrence
 *   http://www.umlautllama.com
 *   2002 April 16
 *
 *   Use this for whatever you like.  No copyrights held.
 *   If it's useful for you, run with it.  :]
 *
 * Ref: http://developer.apple.com/techpubs/macosx/Carbon/multimedia/SpeechSynthesisManager/speechsynthesis.html
 *
 * Remember to build with:
 *   LIBS   += -framework Carbon
 *   CFLAGS += -I/Developer/Headers/FlatCarbon
 */
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <ApplicationServices.h>
//#include "CarbonCore/FixMath.h"

/*
 * There are two versions of the code here; the "short version" and the 
 * "long version".  The long version is first. 
 */

/* set this to 
 *    #undef LONGVERSION
 * to enable the short version instead...
 */
#define LONGVERSION


/*
 * The Long Version
 *
 *      This version of the code queries the system to determine
 *      which voice styles are available.  It then prompts the user
 *      to decide which voice to use.  After that, it continuously
 *      prompts the user for text to speak.  Due to the implementation,
 *      there is no need for a callback, and therefore, the user
 *      can override previous speech by having it speak something
 *      new.
 *
 *      Since this has the ability to change the voice, it has more
 *      code involved with it than the 'short version'.
 *
 */

#ifdef LONGVERSION 


/*
 *  freqs
 *
 *   a list of frequencies that we'll change the pitch of the voice
 *   to when a new phoneme happens...
 */
double freqs[] = { 37, 40, 43, 46, 49, 52, 55, 58, 
                   61, 58, 55, 52, 49, 46, 43, 40 };

/*
 *  MySpeechPhonemeCallback
 *
 *    a callback that gets called before every phoneme is 
 *    generated. We'll simply change the pitch of the channel
 *    based on the table above.  I've got no idea why you'd
 *    want to do this, but it's a good example of what CAN
 *    be done.
 */
int phonemeCounter = 0;
void MySpeechPhonemeCallback (
    SpeechChannel chan, 
    SInt32 refCon, 
    SInt16 phonemeOpcode
)
{
    Fixed valueF;
    phonemeCounter++;

    /* create the "Fixed-point" value */
    valueF = X2Fix( freqs[ phonemeCounter & 0x0f ] );

    /* and set the pitch */
    SetSpeechPitch( chan, valueF );
}

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
    int n = 0;
    int p = 0;

    /* let's check out what we've got for voices... */
    retval = CountVoices( &numvoices );
    printf ("%d voices in the system\n", numvoices);
    
    /* see if whe can get some info about these voices */
    for( c = 1 ; c <= numvoices ; c++ )
    {
	VoiceDescription description;
	retval = GetIndVoice( c, &voicespec );
	retval = GetVoiceDescription( &voicespec, 
			&description, 
			sizeof(VoiceDescription) );


	/* since the names seem to be pascal strings with a number at the 
           beginning of the string, we'll just only print what's valid ascii */
	printf( "%2d - (%4dyrs, %c)", c, description.age, 
		(description.gender == kNeuter)? '-' :
		(description.gender == kMale)? 'M' : 'F'
		);
	n = 0;
	for( p = 0 ; p < strlen( description.name ) ; p++ )
	{
	    if( isalnum( description.name[p] )) n++;
	    if( n>0 )
		printf("%c", description.name[p]);
	}
	printf( "\n" );
    }

    /* choose a new voice to use */
    printf ("choose a voice\n");
    c = 0;
    while (c < 1 || c > numvoices )
    { 
	printf("? "); fflush(stdout);
	fgets(buf, 255, stdin);
	
	c = atoi(buf);
	printf("selected %d\n", c);
    }
    
    /* actually select the new voice */
    retval = GetIndVoice( c, &voicespec );

    /* make a new voice spec */
    //creator = 0;
    //id = 0;
    //retval = MakeVoiceSpec( creator, id, &voicespec );

    /* create the speech channel */
    NewSpeechChannel( &voicespec, &speechchannel );


    /* do something funky */
    printf("%d\n", SetSpeechInfo( speechchannel, 
		    soPhonemeCallBack, 
		    &MySpeechPhonemeCallback ) );

    /* check out the pitch */
    (void) GetSpeechPitch( speechchannel, &valueF );
    printf("pitch is %f\n", Fix2X(valueF));
    /*
    valueF = X2Fix(5.00);
    SetSpeechPitch( speechchannel, valueF );
    */

    /* check out the rate */
    (void) GetSpeechRate( speechchannel, &valueF );
    printf(" rate is %f\n", Fix2X(valueF));

    printf("ctrl-d to end.\n");
    buf[0] = '\0';
    while (1)
    {
	printf("? "); fflush(stdout);
	if(! fgets(buf, 255, stdin))
	    break;

	/* speak the text */
	retval = SpeakText( speechchannel, buf, strlen(buf));
	printf("%d ", retval);
    }
    printf("\n");

    /* destroy the speech channel */
    DisposeSpeechChannel(speechchannel);

    return( 0 );
}




/*************************************************************/
#else
/*************************************************************/

/*
 * The Short Version
 *
 *      This version of the code simply uses the default voice,
 *      and therefore requires much less code.  It is made more
 *      complicated by the fact that because it only will speak
 *      the built in text string, it needs to know when it is done.
 *      A callback is used for this task.  The callback gets called
 *      when the speech has completed.
 *
 *      If the callback was not used, it would exit before the
 *      speech had completed, and you would never hear anything.
 */

/* 
 *  doneflag
 *
 * this flag will get incremented by the text done callback.
*/
int doneflag = 0;


/* 
 *  MySpeechTextDoneCallback()
 *
 * We will register this callback with the speak channel that we are
 * using.  This will get called when the speech is completed.  Since
 * this gets called at interrupt time, we will just increment a counter.
 *  we could probably exit() in here if we wanted to...
 */
void MySpeechTextDoneCallback (
    SpeechChannel chan, 
    SInt32 refCon, 
    void **nextBuf, 
    UInt32 *byteLen, 
    SInt32 *controlFlags
)
{
    doneflag++;
}


/*
 *  main()
 *
 * a very simple version of the speaker that uses the default 
 * system voice...
 */
int
main(
	int argc,
	char ** argv
)
{
    SpeechChannel speechchannel;
    char str[] = "This is macintosh speaking. .";

    /* setup a new speech channel, but use the default voice (NULL) */
    printf("%d\n", NewSpeechChannel( NULL, &speechchannel ) );

    /* setup the callback.  This isn't entirely necessary */
    printf("%d\n", SetSpeechInfo( speechchannel, 
		    soTextDoneCallBack, 
		    &MySpeechTextDoneCallback ) );

    /* and say the text... */
    printf("%d\n", SpeakText( speechchannel, str, strlen( str )) );

    /* then wait for the channel to finish, otherwise it will just stop */
    while (!doneflag);
    
    /*  or, this can be done instead:

    while (SpeechBusy());

    /* and close the channel */
    printf("%d\n", DisposeSpeechChannel(speechchannel) );
    return (0);
}
#endif
