# FIFO Exercises - Operating System 

This repository contains a list of exercise with FIFO (named pipe) from 
Operating System course. All exercise are (or will be) commented to explain
every step of the program. 

## Exercise 1
The goal of this exercise is to create a Client-Server application based on FIFO.
### Server
The server have to create a FIFO and wait for a message, that must contain an 
array of integers (`int vec[] = {a, b}`). 
When the message is received the server have to compare the two integers `a` and `b`.

If `a < b`, the output must be `A is less than B`, otherwise the output has to say `A is greater or equal to B`.

After the output has been printed, the Server must close the FIFO and terminate.
### Client
The client is tasked with the request of the input at the user, store the input in the array
and send it with the FIFO to the server. After the message is sent the client will terminate.

