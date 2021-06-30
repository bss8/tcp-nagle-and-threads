# CS5341 Assignment 2

## Build the Code

Simply run `make` from the root directory and all code should be automatically built. 

### Manual build: 

To build manually, execute these commands: 
1. `g++ -o client client.cpp -g -std=c++11`     
2. `g++ -pthread -o server server.cpp -g -std=c++11`      
3. `g++ -pthread -o tclient readline_client.cpp -g -std=c++11`    
4. `g++ -pthread -o mclient modified_client.cpp -g -std=c++11`     
5. `g++ -pthread -o thread thread_test.cpp -g -std=c++11`     

## Run the code

First run the server, then run one of the 3 clients as needed. 

`./server`

`./client` or `./tclient` or `./mclient`

To run the small thread test code: `./thread`    


- client executable is for problem #1 and #2 and basic testing.    
- tclient is for readline_client.cpp, essentially for problem #3 part 1. Type some text and press Enter.     
- mclient is for modified_client.cpp, a modified version of readline_client.cpp for problem #3 part 2. Type some text and press Enter.   
- thread is for problem #2 part e. Once main exits normally, the thread terminates and nothing prints.     
    

## Helpful commands 

When terminating the client manually with Ctrl + C, for example, the port will be locked for the duration set by the operating system. Run the command below to see the status of the port (5001 by default)

`netstat --tcp --numeric | grep 5001`