----------------------------------------
                                     say
         Darwin/OS X command line speech

                                    v1.1

                  Scott "Jerry" Lawrence
                                2002 May
                     jsl@umlautllama.com

              http://www.umlautllama.com


----------------------------------------
                                OVERVIEW

    This is a simple (and tiny) command line application that will
    enable the user to speak the contents of any file, or from data
    coming in from another program in any voice they chose.  It
    also supports an interactive mode so that the user can type in
    text to be spoken.


----------------------------------------
                                   USAGE

    There are two main ways to work with 'say', interactive and
    non-interactive.  An example usage for an interactive session
    might be:

	    ./say -o -v 15

    The user will see some information about voice 15, and then
    they can start typing out things to be spoken.

    Due to the "-o" flag, as soon as the user types a new line,
    the previous line will be cut off.  Try it with and without
    this flag to understand its behavior.


    An example usage for a non-interactive session might be:

	    date "+It is now %l:%M on %A %B %m %Y" | ./say -v r -q

    This will speak the current date and time in a random voice.

	    date "+It is now %l:%M on %A %B %m %Y" | ./say -v m -q

    This will speak the current date and time in a random male voice.


    To get a list of all available voices, type:

	    ./say -l 

    Use the number that is in the leftmost column of the table that
    the above printed to select a voice.  For example, if 'say -l'
    returned something like this:

	  1   F 35 Agnes
	  2   M 30 Albert
	  3   F 30 Kathy

    and you wanted to speak the text in the file "/etc/motd" in
    the "Kathy" voice, then you would type:

	    ./say  -v 3  -f /etc/motd

    To get rid of the extra output, which says the pitch, rate,
    and number of the voice chosen, use the '-q' option to "quiet"
    it down.

	    ./say  -q  -v 3  -f /etc/motd

    Once you have selected a voice, you chan change the pitch and
    rate which the text is spoken.  When you are not running in
    "quiet" mode, you will see the pitch and rate of the voice
    selected, as you saw in the example above.

    To speed up the rate of speech playback to 300 words per minute,
    with the pitch setting set at 65, try this:

	    ./say  -v 3  -p 65  -r 300  -f /etc/motd

    Note: some voices ignore the pitch setting, or do strange things
    with it.


    For a complete list of all of the parameters, type:

	    ./say -h


----------------------------------------
				 rsay.pl

    This is a short little perl script that will start up a network
    server on your machine on port 4242 (confifurable in the script)
    that lets people telnet to your machine and type in their name
    and a message to be spoken.  You can change the parameters to
    'say' to specify the voice to be spoken with.


----------------------------------------
				   FILES

    Say currently reads in files on a line-by-line basis.  This
    means that it will assume that a 'sentence' ends at the end of
    the line.  This isn't entirely useful yet for all tasks.  Future
    releases of Say will allow you to speak the text in a file on
    a "sentence by sentence" or a "paragraph by paragraph" basis.
    Check back at the website for this update, when it happens.



----------------------------------------
				   BUILD

Mac OS X:  (Macintosh)

    If you have the Apple OS X Developers tools installed properly,
    you should be able to build the executable by typing:

	make test

    It will speak something to you when it has completed.


Other build environments:

    Since the Carbon API hasn't been ported to any other platforms,
    and there is no command line in Mac OS 9, no other platform
    has been tested.  If you have gotten this to build under OS 9,
    let me know! :]


----------------------------------------
                                 VERSION

    The latest version of this should be available off of 

	http://www.cis.rit.edu/~jerry/Software/MacOSX/#say



----------------------------------------
                                 LICENSE


    This program is totally free.  If you like it, drop me an email.
    Feel free to redistribute this, modify the source, whatever.
