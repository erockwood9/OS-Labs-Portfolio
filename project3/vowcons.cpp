#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <sched.h>

using namespace std;

// Global variables to store words and synchronization
vector<string> words;
pthread_mutex_t mutex;
int index = 0; // Current index in the words vector

//***********************************************************************
//
// Vowel Check Helper Function
//
// This function checks whether a word starts with a vowel
// if the first character is a vowel, the return value is true.
// Otherwise, the return value is false.
//
//***********************************************************************
bool starts_with_vowel(const string& word) {
    char first_char = tolower(word[0]);
    return first_char == 'a' || first_char == 'e' || first_char == 'i' ||
           first_char == 'o' || first_char == 'u';
}

//***********************************************************************
//
// Word Parsing Function
//
// This function takes a string of input and parses it into a vector
// of individual words. Nothing is returned, since it modifies a global
// variable containing the words
//
//***********************************************************************
void parse_args(string line){
    // Initialize local variales
    stringstream liness(line);
    string token;

    // Parse the sstream into individual tokens, appending each one to 
    // the global vector 'words'
    while (getline(liness, token, ' ')) {
        words.push_back(token);
    }
}

//***********************************************************************
//
// Vowel-thread Function
//
// This thread function repeatedly selects the next unprocessed word from
// the shared `words` vector and prints it if it starts with a vowel.
// It uses `mutex` to synchronize access to the shared `index` and
// returns when all words have been processed.
//
//***********************************************************************
void* vow(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        if (index >= words.size()) {
            pthread_mutex_unlock(&mutex);
            return nullptr; // Exit if all words are processed
        }
        while (!starts_with_vowel(words[index])) {
            pthread_mutex_unlock(&mutex);
            sched_yield();  // Give CPU control to other thread
            pthread_mutex_lock(&mutex);
            if (index >= words.size()) {
                pthread_mutex_unlock(&mutex);
                return nullptr; // Exit if all words are processed
            }
        }

        if (starts_with_vowel(words[index])) {
            cout << "vow: " << words[index] << std::endl;
            ++index;
        } 
        pthread_mutex_unlock(&mutex);
        sched_yield();   
    }
}

//***********************************************************************
//
// Consonant-thread Function
//
// This thread function repeatedly selects the next unprocessed word from
// the shared `words` vector and prints it if it starts with a consonant.
// It uses `mutex` to synchronize access to the shared `index` and
// returns when all words have been processed.
//
//***********************************************************************
void* cons(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        if (index >= words.size()) {
            pthread_mutex_unlock(&mutex);
            return nullptr; // Exit if all words are processed
        }
        while (starts_with_vowel(words[index])) {
            pthread_mutex_unlock(&mutex);
            sched_yield();  // Give CPU control to other thread
            pthread_mutex_lock(&mutex);
            if (index >= words.size()) {
                pthread_mutex_unlock(&mutex);
                return nullptr; // Exit if all words are processed
            }
        }
        if (!starts_with_vowel(words[index])) {
            cout << "cons: " << words[index] << std::endl;
            ++index;
        }
    pthread_mutex_unlock(&mutex);
    sched_yield();      
    }
}

//***********************************************************************
//
// Main Function
//
// Reads a line of input from stdin, parses it into words, initializes
// synchronization primitives, creates the vowel and consonant threads,
// waits for them to finish, then cleans up and returns an exit code.
//
//***********************************************************************
int main(void) {
    // Read input line and parse into words
    cout << "Enter a line of text: ";
    string input_line;
    getline(cin, input_line);
    parse_args(input_line);

    // Initialize mutex and condition variables
    pthread_mutex_init(&mutex, nullptr);

    // Create threads for vowel and consonant word printing
    pthread_t thread_vow, thread_cons;
    pthread_create(&thread_vow, nullptr, vow, nullptr);
    pthread_create(&thread_cons, nullptr, cons, nullptr);

    // Wait for both threads to finish
    pthread_join(thread_vow, nullptr);
    pthread_join(thread_cons, nullptr);

    // Clean up mutex and condition variables
    pthread_mutex_destroy(&mutex);
    return 0;
}
