#!/usr/bin/perl
#
#  rsay.pl
#
#   Scott "Jerry" Lawrence
#   jsl@umlautllama.com
#   http://www.umlautllama.com
#   2002 April 19
#

# some thingies we need for this to work
use IO::Socket;    ## for setting up the socket
use Net::hostent;  ## for gethostbyaddr

# turn on autoflush
$|=1;

# the port number we will be listening on
$listenport = 4242;

# paramaters to pass in to 'say'.  This picks a random female voice.
$sayparams = "-v f";

# setup the socket that we will use
$server = IO::Socket::INET->new(
		Proto     => 'tcp',
		LocalPort => $listenport,
		Listen    => SOMAXCONN,
		Reuse     => 1);

# bail if it fails
die "Can't setup server" unless $server;
printf "Listening on port $listenport\n";

# then just sit in a loop, listening for connections
while ($client = $server->accept())
{
    # autoflush the client (not really necessary anymore)
    $client->autoflush( 1 );

    # determine the current time
    ($sec,$min,$hour,$day,$month,$year,$wday,$yday,$isdst) = localtime(time);
    printf "%02d:%02d ", $hour, $min;

    # determine host info...
    $hostinfo = gethostbyaddr($client->peeraddr);
    printf "(%s) ", (defined $hostinfo)? 
		$hostinfo->name : $client->peerhost;

    # prompt for a name
    print $client "enter your name\n"; 
    $name = <$client>;
    $name =~ s/[\n\r]//g;
    $name =~ s/[\"-]/ /g;
    # clean up naughty words
    $name =~ s/[Ff]'*[Uu]'*[Cc]'*[Kk]'*/ f /g;
    $name =~ s/[Ss]'*[Hh]'*[Ii]'*[Tt]'*/ s /g;
    $name =~ s/[Aa']+[Ss']+/ a. /g;

    # prompt for their text
    print $client "enter your text\n"; 
    $text = <$client>;
    $text =~ s/[\n\r]//g;
    $text =~ s/[\"-]/ /g;
    # clean up naughty words
    $text =~ s/[Ff]'*[Uu]'*[Cc]'*[Kk]'*/ f /g;
    $text =~ s/[Ss]'*[Hh]'*[Ii]'*[Tt]'*/ s /g;
    $text =~ s/[Aa']+[Ss']+/ a. /g;

    # print out the message locally
    printf "%s: %s\n", $name, $text;

    # then speak it
    system( sprintf "echo \"%s says %s.\" | say -q %s", 
			$name, $text, $sayparams );

    # close the connection
    close( $client );
}
