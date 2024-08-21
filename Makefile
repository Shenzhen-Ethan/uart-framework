#/*
# * Copyright 2023 Ethan. All rights reserved.
# */

EXEC := app
TARGET_LDLIBS = -lpthread
LIB :=
DIR := .
SRC := .
SOURCES := \
	./src/utils/logger/log.c \
	./src/utils/timer/time_utils.c \
	./src/utils/timer/timer.c \
	./src/utils/intaccess.c \
	./src/wcu.c \
	./src/uart_client.c \
	./main.c

INC := -I \
	./inc

TARGET_CFLAGS :=  \
	$(INC)

.PHONY: make build inc src

-include $(SRC)/make/defs.mk

-include $(SRC)/make/cc.mk
