#! /bin/bash

gcc msg.c process.c background.c clientServerEV.c server.c client.c main.c -lpthread -lrt -o main -g