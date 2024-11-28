# first of all import the socket library 
import socket	
from _thread import *	
import threading	


shared_list=[]

# thread function
def skywalker(c):
    while True:
        
        # data received from client
        data = c.recv(1024)

        if not data:
            print('Bye')
             
            # lock released on exit
            #print_lock.release()
            break
 
        # add book to shared list!

        #change
        shared_list.append(data)

        #add a lock to prevent race condition

        print("id:",threading.get_ident(), "fuck you message:",data)
        # send back reversed string to client
        c.send(data)

    # connection closed
    c.close()

# creat 10 empty files, book1.txt ... book10.txt
def create_files():
    for i in range(1, 11):
        with open(f"book_{i:02}.txt", "w") as file:
            pass

if __name__ == "__main__":
    create_files()


# next create a socket object 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	 
print ("Socket successfully created")

# reserve a port on your computer in our 
# case it is 12345 but it can be anything 
port = 12345			

# Next bind to the port 
# we have not typed any ip in the ip field 
# instead we have inputted an empty string 
# this makes the server listen to requests 
# coming from other computers on the network 
s.bind(('', port))		 
print ("socket binded to %s" %(port)) 

# put the socket into listening mode 
s.listen(5)	 
print ("socket is listening")		 

# a forever loop until we interrupt it or 
# an error occurs 
while True: 

# Establish connection with client. 
 c, addr = s.accept()	 
#print ('Got connection from', addr )

# send a thank you message to the client. encoding to send byte type. 
#c.send('Thank you for connecting'.encode()) 

 start_new_thread(skywalker, (c,))

# Close the connection with the client 
#c.close()

# Breaking once connection closed
#break