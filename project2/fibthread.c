#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LENGTH 90

void *fib(void *);

// Global array used to store computed Fibonacci values.
long fib_numbers[MAX_LENGTH];

int main(void)
{
  //***********************************************************************
  //
  // Main Function
  //
  // Prompts the user for how many Fibonacci numbers to generate (bounded
  // by MAX_LENGTH), starts a worker thread to compute them, waits for the
  // worker, and then prints the results.
  //
  //***********************************************************************
  /* local variables */
  pthread_t thread;
  pthread_attr_t attr;
  int n;
  printf("Enter the number of Fibonacci numbers to generate (max %d): ", MAX_LENGTH);
  scanf("%d", &n);

  // Check for reasonable bounds
  if (n <= 0 || n >= MAX_LENGTH) {
    printf("Number out of bounds. Please enter a value between 1 and %d.\n", MAX_LENGTH);
    return 1;
  }
  
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&thread, &attr, fib, (void *)&n);
  pthread_join(thread, NULL);
  
  printf("Printing the first %d fibonacci numbers:\n", n); 
  for (int i=0;i<n;i++){
      printf("%ld\n",fib_numbers[i]);
  }
  
  return (0);
}

void *fib(void *s)
{
  //***********************************************************************
  //
  // Fibonacci worker thread
  //
  // The thread reads the requested count from the provided pointer and
  // fills the global `fib_numbers` array with the Fibonacci sequence up to
  // that count. It exits via pthread_exit when finished.
  //
  //***********************************************************************
  int n = *((int*)s);
  long a = 0, b = 1, c;
  fib_numbers[0] = a;
  fib_numbers[1] = b;

  if (n==1 || n==2) {
    pthread_exit(0);
  } else {
      for (int i=2;i<n;i++)
      {
          c = a + b;
          a = b;
          b = c;
          fib_numbers[i] = c;
      }
  }
  
  pthread_exit(0);
}

