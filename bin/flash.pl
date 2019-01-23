#! /usr/bin/perl

###########################################################
#
# flash, a perl script to improve short term visual memory 
# it flashes random numbers or words in a file on the screen
# for a short time then the user has to enter them.
# 
###########################################################
#
use strict;

# set auto flush so STDIO is printed before return character
$| = 1;

# global variables 
my(%flags,$BEEDIR);

# set path to Bee program 
if(!$ENV{'BEEDIR'})
{
  print "must include BEEDIR environment variable in your .cshrc\n";
  exit(0);
}
else
{
  $BEEDIR = $ENV{'BEEDIR'};
}

#main
{ 
 # main variables
 my(@words);

 # clear screen 
 `clear`;

 # get runtime options
 %flags = &getargs;
# print "flags\n";
# foreach(sort(keys(%flags))){print "$_: $flags{$_}\n";}
 
 # get words from file 
 if($flags{file}){
  open(WORDFILE,"<$flags{file}") or die ;
  my @tmpwords = <WORDFILE>; 
  my $nn = @tmpwords;
  $nn = $flags{"-n"} if($flags{"-n"} < $nn);
  $flags{"-n"} = $nn; 
   for($nn; $nn > 0; $nn--)
   {
     my $indx = int(rand($#tmpwords+0.5));
     die("error in loadwords\n") if($indx > $#tmpwords);
     @words = (@words, splice(@tmpwords,$indx,1));
     chomp(@words);
   }
 }
 # generate random numbers
 else{
   for(my $nn = $flags{"-n"}; $nn > 0; $nn--){
     my $mlen = $flags{"-b"}-$flags{"-s"};
     my $len = int(rand($mlen)+0.5)+$flags{"-s"};

     foreach(my $ii=0;$ii<$len;$ii++){
       @words[$nn] = @words[$nn].int(rand(9)+0.5);
     }
   }
 }

#print $_,"\n" foreach(@words);

 # flash words
  sleep 3; 
  my $nguess = 0;
  for(my $ii=0;$ii < @words;$ii++){
   my $guess = '';

   while($guess ne $words[$ii]){
    print $words[$ii];
    sleep $flags{"-t"}; 
    for(my $jj=1;$jj<=length($words[$ii]);$jj++){
      print "";
    }
    for(my $jj=1;$jj<=length($words[$ii]);$jj++){
      print " ";
    }
    print "\n";
    $guess = <STDIN>;
    chomp($guess);
    play_word('correct') if ($guess eq $words[$ii]);
    play_word('boo') if ($guess ne $words[$ii]);
#    print "guess: $guess\nword : $words[$ii]\n";
    $nguess++;
  }
 }

 print "\n ",$flags{'-n'}," words in $nguess trys\n";

 # set logfile name. 
 my ($logfile,$plotfile);
 if(!$flags{file}){
   $logfile = "$BEEDIR/log/flash_$flags{-s}_$flags{-b}.log";
   $plotfile = "$BEEDIR/log/flash_$flags{-s}_$flags{-b}.plt";
 }
 else{
   exit(0); # I NEED TO fix this the file name sometime includes
   # the entire path so it messes up my logging i need to pull the file
   # name off with base name.
   $logfile = "$BEEDIR/log/$flags{file}.log";
   $plotfile ="$BEEDIR/log/$flags{file}.plt";
  }

 # print results to log file
 open LOG, ">>$logfile";
 print LOG "$flags{'-n'} $nguess\n";
 close LOG;
 open LOG, "<$logfile";
 my @history = <LOG>;
 close LOG;

 #  plot results
 open PLT, ">$plotfile";
 my $ii=0;
 foreach(@history){
   my($num,$try) = split;
   print PLT $ii++," ",$num/$try,"\n";
 }
 close PLT;
 open GNUPLOT,"|gnuplot";
 print GNUPLOT 'set xlabel "test"',"\n";
 print GNUPLOT 'set ylabel "score [percentage]"',"\n";
 print GNUPLOT 'set title "improvement monitor for flash"',"\n";
 print GNUPLOT "set grid; set yrange [0 to 1.1]\n";
 print GNUPLOT "set style data linespoints\n";
 print GNUPLOT "plot \"$plotfile\"\n";

} # end of main


################## 
sub getargs{
 # declair variables 
   my($tmp,$smallestSet,$biggestSet,$numberSet);

 # set defaults runtime options
   my(@args) = reverse(@ARGV); # reverse so pop works first to last arg 
   $flags{'-h'} = 0;      # print usage
   $flags{'-s'} = 6;      # smallest word 
   $flags{'-b'} = 7;      # biggest word 
   $flags{'-t'} = 1;      # number of seconds word is displayed 
   $flags{'-n'} = 10;     # number of quiz words 
   $flags{'-v'} = 50;     # volume of check 
   $flags{'file'} = "";   # word file 

#   print "@args\n";
   while($tmp = pop(@args))
   {
     SWITCH: 
     {
        if($tmp eq '-h') {usage()};
        if($tmp eq '-s') 
	{
	   $flags{'-s'} = pop(@args); 
	   if(defined($flags{$flags{'-s'}})) {usage()};
	   $smallestSet = 1; 
           last SWITCH;
	};
        if($tmp eq '-b') 
	{
	   $flags{'-b'} = pop(@args); 
	   if(defined($flags{$flags{'-b'}})) {usage()};
	   $biggestSet = 1; 
           last SWITCH;
	};
        if($tmp eq '-t') 
	{
	   $flags{'-t'} = pop(@args); 
	   if(defined($flags{$flags{'-t'}})) {usage()};
           last SWITCH;
	};
        if($tmp eq '-v') 
	{
	   $flags{'-v'} = pop(@args); 
	   if(defined($flags{$flags{'-v'}})) {usage()};
           last SWITCH;
	};
        if($tmp eq '-n') 
	{
	   $flags{'-n'} = pop(@args); 
	   if(defined($flags{$flags{'-n'}})) {usage()};
	   $numberSet = 1; 
           last SWITCH;
	};
        if($#args != -1){print "Usage error:\n"; usage()};
	if(-R "$tmp"){
	  $flags{'file'} = $tmp;
	  $flags{'-s'} = 1 if(!$smallestSet);
	  $flags{'-b'} = 10000 if(!$biggestSet);
	  $flags{'-n'} = 10000 if(!$numberSet);
        }
	elsif(-R "$BEEDIR/etc/$tmp"){
	  $flags{'file'} = "$BEEDIR/etc/$tmp";
	  $flags{'-s'} = 1 if(!$smallestSet);
	  $flags{'-b'} = 10000 if(!$biggestSet);
	  $flags{'-n'} = 10000 if(!$numberSet);
        }
	else{
           die("flash: can not find file: $tmp or $BEEDIR/etc/$tmp\n");
        }
     }
   }

     return %flags;
}
################## 
sub usage{
print "\nflash [-h] [-s n] [-b n] [-t n] [-n n] [file]\n";
print "-h\tprint this message\n";
print "-s\tlength of smallest word\n";
print "-b\tlength of biggest word\n";
print "-t\ttime that word is displayed [seconds]\n";
print "-n\tnumber of words flashed before test ends\n";
print "-v\tvolume 1 to 100 default 50 \n";
print "file\toptional name of file containing words to be flashed\n\n";
exit;
}

################## 
sub play_word{
   my($vol) = $flags{'-v'}; # playing volume
   my($word) = $_[0];
   $word =~ s/\n//;
   `echo $word | $BEEDIR/bin/say -q`;
}
################## 
