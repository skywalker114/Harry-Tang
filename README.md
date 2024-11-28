This assignment was locked 31 Oct 2023 at 23:59.
Due date: Fri, Oct 27, 23:59 (Adelaide time)   Extended to Monday 30th     and with no penalty applied for late submissions to Friday 3rd November 23:59pm which is the last day to submit 

 

Task: Multi-Threaded Network Server for Pattern Analysis

Objective:

Concurrency is among the most challenging parts of Operating Systems to understand.  Developing a full appreciation for the problem space is essential for your later professional career.  The ability to think in concurrent design architectures and to program multi-threaded with proper synchronisation is a very important skill to develop.  For this reason, this assignment aims to create a high-performance multi-threaded network server capable of managing incoming connections, processing text data, and analysing patterns within the data. This exercise will provide hands-on experience with concurrent programming, socket handling, and understanding the differences between blocking and non-blocking I/O.

1. Setup
You will require some large text files for this assignment. Consider using resources like the Gutenberg Project (https://www.gutenberg.org) to obtain such files. Download plain text format books (UTF-8) and save them locally for later use.
To send these text files to your program, consider utilising the netcat tool (nc).  For instance, to transmit a text file to your server, you may use the following command:

nc localhost 1234 -i <delay> < file.txt
Ensure that the first line of each text file contains the title of the respective book.   This makes your program later easier, as you can grasp a book identifier easily from the incoming data stream. 

2. Multi-Threaded Network Server
Write a multi-threaded network server in a programming language of your choice (C, Python, Java).  The server should listen for incoming connections on your chosen port.  It is easy to create a listen socket in any language, for example, CLinks to an external site., PythonLinks to an external site., or JavaLinks to an external site.. 

The server should listen to a networking port (> 1024).  Ensure that the server efficiently manages multiple simultaneous connections. The program should create a new thread for each incoming connection to handle client communication. This approach has to allow multiple clients to connect simultaneously.  In each thread, implement non-blocking readsLinks to an external site. from the sockets to efficiently receive and store data in one shared data structure -  a list.  Every line read is linked into that shared list that is the same across all threads. 

Part 1  - manage that shared list, which  involves multiple tasks:

Managing multiple readers:  for each incoming read or line you will create a new node and added to the shared list.  The purpose is to be able to keep track of the history of how data has arrived and been processed.  (Note this is similar to the list process we discussed in the lectures, but here we will be adding the node at the end of the list). 
Additionally, you need to keep track of each book by embedding a second pointer (let's call it  "book_next") into the shared list and having a head pointer for each book.  That means each thread needs to be able to update (additional) links to nodes that contain book lines in the correct order. 
Print a book: output each received book correctly: traversing the list from the book's header by "book_next" reproduces the complete book in the correct order.  
 

A diagram of the shared multi-link list is shown below 

Screen Shot 2023-10-04 at 4.25.16 pm.png

Note that the shared list has multiple links per node as described below: 

node->next - links to the next element in the shared list.   
node->book_next - links to the next item in the same book. 
node->next_frequent_search  (part2) links to the next item that had the search terms
 

Program outputs: 

As you add each node to the list you should print a line reporting the addition of that node to the screen (stdout). 

When a connection closes, you should write the received book; the filename is book_xx.txt where "xx" is  the number (order) in which the connection was accepted. For example, if you have three connections your program should write three files: book_01.txt, book_02.txt and book_03.txt.

 

Part 2 -  Multithreaded Analysis 

Implement two or more analysis threads that read from that shared data structure in a similar fashion that you have learned from the consumer/producer problem and are able to compute the frequency of an specific search pattern within the received data (e.g., maintain a linked list of notes that contain a particular search string).  The pattern would be given by the command line.  

In periodic configurable intervals (e.g., once every 2 or 5 seconds), one of the analysis threads should output the book titles sorted by the most frequent occurrences of the selected search pattern. Only the one thread that initiated the output first in that interval should be allowed to write to the screen and output the data within that timeframe. The output of the analysis should be to the screen (stdout). 

 

 

3. Testing
Develop tests that analyse your program to ensure a thread-safely to avoid data race conditions. You may initially test your program with a small number of input streams  but make sure  you also test your program with at least 10 simultaneous input streams to ensure robustness and scalability.  


Submission: 
Deliverables

Multi-threaded network server code in either C, Python or Java programming language. 
Makefile to produce assignment3 
The server should be started with: 

./assignment3 -l 12345  -p "happy"
where 12345 is a listen port  (it might be changed) and "happy" is the seach pattern: it will count how many times the string happy appears in the book and create a list to navigate the lines that contained it. 

A README document detailing the commands used to run your server and client programs (e.g., Client program using netcat) to send data to the server.
Academic Integrity:
OK to use any materials except original content from another team or generative AI-based code (e.g., ChatGPT or Co-Pilot). 
OK to use code for setting up listening socket templates from sources. 
It is Not OK to get someone outside your team to help you.
 


Grading Criteria

Part 1 is worth 80% and will be marked using an automatic script 

The network server can accept incoming connections from a listen socket  and is non-blocking  10%
Efficient handling of multiple connections using threads, ensuring the server remains responsive 10%
Correct log printed     15% 
Books are received and printed into files correctly   30%
Scales to 10 threads    15%
Part 2 is worth 20% and  will be assessed both  automatically and manually. 

Pattern analysis frequency is correct    10%
Results printed at intervals 5%
Book titles sorted by the most frequent occurrences of the selected pattern  5%
