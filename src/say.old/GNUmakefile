# Makefile for the sample code to speak using Carbon/Darwin
#
#   Scott "Jerry" Lawrence
#   jsl@umlautllama.com
#   http://www.umlautllama.com
#   2002 April 17
#
#   Use this for whatever you like.  No copyrights held.
#   If it's useful for you, run with it.  :]
#
# Ref: http://developer.apple.com/techpubs/macosx/Carbon/multimedia/SpeechSynthesisManager/speechsynthesis.html
#
############################################################
prog:
all:
############################################################
NAME    := say
VERSION := 1.1

TARGET := say

SRCS := say.c

OBJS := $(SRCS:%.c=%.o)

LIBS := -framework Carbon

CFLAGS  := -I/Developer/Headers/FlatCarbon

all: prog 

prog:	$(TARGET) $(DATA)

clean:
	rm -rf $(OBJS) $(TARGET)

############################################################

$(TARGET):	$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)


############################################################
test:	prog
	echo greetings `whoami`.  Enjoy. | ./say

############################################################

BAK_NAME := $(NAME)_$(VERSION)

backup: clean
	touch ../$(BAK_NAME).tar.gz
	mv ../$(BAK_NAME).tar.gz ../$(BAK_NAME)_old.tar.gz
	cd .. ; tar -cvf $(BAK_NAME).tar $(NAME)/* ; gzip $(BAK_NAME).tar

.PHONY: backup
############################################################

.PHONY: all
.PHONY: prog
.PHONY: size
.PHONY: clean
.PHONY: tidy

.SECONDARY: $(OBJS)
.SECONDARY: $(SRCS)
.SECONDARY: $(HDRS)

############################################################
