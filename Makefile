
CC			:= gcc
CXX			:= g++
SRCDIR		:= src
BUILDDIR	:= build
TARGETDIR	:= bin

CFLAGS		:= -G # -Wall
INC			:= -Iinclude
LIBZMQ		:= -lczmq -lzmq
LIBGL		:= -lGL -lglut

codec		:= $(patsubst %, $(TARGETDIR)/%, decode_worker encode_worker)
devices		:= $(patsubst %, $(TARGETDIR)/%, fowarder proxy)


.PHONY : all clean

all : $(codec) $(TARGETDIR)/gpu_worker $(devices)

$(codec) : $(TARGETDIR)/% : $(SRCDIR)/%.c $(SRCDIR)/worker.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) $(LIBZMQ) $^ -o $@

$(TARGETDIR)/gpu_worker : $(wildcard $(SRCDIR)/gpu/*) | $(BUILDDIR)
	$(CXX) $(CFLAGS) $(INC) $(LIBZMQ) $(LIBGL) $^ -o $@

$(devices) : $(TARGETDIR)/% : $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) $(LIBZMQ) $< -o $@

$(BUILDDIR) :
	mkdir $(BUILDDIR)

clean:
	rm -rf build






