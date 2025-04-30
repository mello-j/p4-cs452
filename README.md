# Makefile Project Template

Steps to configure, build, run, and test the project.

## Building

```bash
make
```

## Testing

```bash
make check
```

This runs a handful of tests through the command line tester provided by the instructor.

```bash
./test.sh
```

## Clean

```bash
make clean
```

## Install Dependencies

In order to use git send-mail you need to run the following command:

```bash
make install-deps
```


## Project Description
This is the final project for CS452 that implements a FIFO queue as a monitor for the bounded buffer problem. This project is used to develop and showcase programming techniques in threading, event loops, and inter-process communciation.

Unit tests and a testing script to run the main command line tester were written with AI to test the validity of the implemented QUEUE. 

Additionally I decided to run Lab.h through 3 different LLM's the see the results. They all struggled with setting the correct conditions in dequeue, enqueue, and is shutdown (or at least conditions that made sense), with each one failing the instructor provided test number 3. Interesting they all had the same issue. I found Gemini to be overexplanatory and verbose in coding, ChatGPT was much more succint, and Claude was the most "Accurate". I chose Claude to write my unit tests :) I plan to review these tests and review the lab.c again but submitting for now.


