
CC			:= gcc
CXX			:= g++
#CFLAGS		:= -G -Wall
CXXFLAGS	:= $(CFLAGS) -std=c++11

BIN			:= bin
BUILD		:= build
SRC			:= ultrafast
INC			:= -I$(SRC) -Ilib
LIBZMQ		:= -lzmq -lczmq
LIBGL		:= -lGL -lglut
LIBJPEG     := -Llib -ljpeg

targets		:= $(addprefix $(BIN)/, proxy forwarder decode_worker encode_worker gpu_worker)
codecdir	:= $(SRC)/codec
gpudir		:= $(SRC)/gpu
clusterdir	:= $(SRC)/cluster
codec 		:= decode_worker encode_worker
cluster 	:= forwarder proxy


.PHONY : all clean

all : $(targets)

#codec
$(addprefix $(BIN)/, $(codec)) : $(BIN)/% : $(codecdir)/%.c $(wildcard $(codecdir)/worker.*)
	$(CC) $(CFLAGS) $(INC) $(filter-out %.h, $^) $(LIBZMQ) $(LIBJPEG) -o $@

#gpu
$(BIN)/gpu_worker : $(wildcard $(gpudir)/*)
	$(CXX) $(CXXFLAGS) $(INC) $(filter-out %.h, $^) $(LIBZMQ) $(LIBGL) -o $@

#cluster
$(addprefix $(BIN)/, $(cluster)) : $(BIN)/% : $(clusterdir)/%.c
	$(CC) $(CFLAGS) $(INC) $< $(LIBZMQ) -o $@

clean:
	rm -rf $(BUILD)
	mkdir $(BUILD)
	rm -rf $(BIN)
	mkdir $(BIN)

