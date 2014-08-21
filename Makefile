
CC			:= gcc
CXX			:= g++
#CFLAGS		:= -G -Wall
#CXXFLAGS	:= $(CFLAGS)

BIN			:= bin
BUILD		:= build
SRC			:= ultrafast
INC			:= -I$(SRC)
LIBZMQ		:= -lczmq -lzmq
LIBGL		:= -lGL -lglut

targets		:= $(addprefix $(BIN)/, proxy forwarder decode_worker encode_worker gpu_worker)
codecdir	:= $(SRC)/codec
gpudir		:= $(SRC)/gpu
clusterdir	:= $(SRC)/cluster
codec 		:= decode_worker encode_worker
cluster 	:= forwarder proxy


.PHONY : all clean

all : $(targets)

#codec
$(addprefix $(BIN)/, $(codec)) : $(BIN)/% : $(codecdir)/%.c $(codecdir)/worker.h
	$(CC) $(CFLAGS) $(INC) $(LIBZMQ) $< -o $@

#gpu
$(BIN)/gpu_worker : $(wildcard $(gpudir)/*)
	$(CXX) $(CXXFLAGS) $(INC) $(LIBZMQ) $(LIBGL) $(filter-out %.h, %^) -o $@

#cluster
$(addprefix $(BIN)/, $(cluster)) : $(BIN)/% : $(clusterdir)/%.c
	$(CC) $(CFLAGS) $(INC) $(LIBZMQ) $< -o $@

clean:
	rm -rf $(BUILD)
	mkdir $(BUILD)

