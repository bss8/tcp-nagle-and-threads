#include <iostream>
#include <string>
#include <thread>
#include <chrono>

/**
 * A simple function that will be executed in a spearate thread. 
 * Simply sleep for 2 seconds and then try to print a string. 
 * If a detached thread can survive once main terminates normally, 
 * we should be able to see the string. Otherwise nothing will print. 
 * The latter is what we observe. 
 */
void some_thread()
{
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "Do you see me?" << std::endl;
}

/**
 * Detach the thread and allow main to exit normally. 
 * We do not observe any output, so once main() terminates the thread terminates.
 */
int main()
{
  std::thread t1(some_thread);
  t1.detach();

  return 0;
}
