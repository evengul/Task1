#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>

/**
 * Finn alle primtall mellom to gitte tall ved hjelp av et gitt
antall tråder.
Skriv til slutt ut en sortert liste av alle primtall som er funnet
Pass på at de ulike trådene får omtrent like mye arbeid

 */

using namespace std;


class PrimeChecker {
private:
    /**
     * From Wikipedia: Primality Test
     * @param number [int]: The number you want to check
     * @return true if the number is prime
     */
    bool isPrime(int number) {
        if (number <= 3) {
            return number > 1;
        }
        else if (number % 2 == 0 || number % 3 == 0) {
            return false;
        }

        int i = 5;

        while (i * i <= number) {
            if (number % i == 0 || number % (i + 2) == 0) {
                return false;
            }
            i = i + 6;
        }
        return true;
    }

    /*
        Get all primes between two points
    */
    vector<int> threadPrimes(int start, int end) {
        vector<int> primes = {};
        for (int i = start; i < end; i++) {
            if (isPrime(i)) {
                cout << i << " is a prime between " << start << " and " << end << endl;
                primes.push_back(i);
            }
        }
        return primes;
    }

    /*
        Split the work of a set of numbers between a set of primes. 
        Returns a vector of length threads, with vectors of length 2 (start of thread, end of thread)
    */
    vector<vector<int>> splitWork(int start, int end, int threads) {
        //If only one thread, that thread does all the work
        if (threads == 1) {
            return { {start, end} };
        }
        //Declare a work-vector, and a container for the start-end-points
        vector<vector<int>> work = {};
        vector<int> threadWork = { 0, 0 };
        //The first thread starts at start
        threadWork[0] = start;
        //The first thread starts between start and end
        int lastStep = (end - start) / 2;
        threadWork[1] = start + lastStep;
        //We add this set of work to the vector
        work.push_back(threadWork);

        //For all the rest of the threads, except for the last one
        for (int i = 1; i < threads - 1; i++) {
            //The start of the work for the thread is the end of the last one
            threadWork[0] = threadWork[0] + lastStep;
            //The work of the thread is half the size of the last one
            lastStep = lastStep / 2;
            threadWork[1] = threadWork[0] + lastStep;
            work.push_back(threadWork);
        }

        //The final thread takes the remainder of work
        threadWork[0] = threadWork[0] + lastStep;
        threadWork[1] = end;
        work.push_back(threadWork);
        return work;
    }

    /*
        Concatenates two vector<int>'s
    */
    vector<int> concat(vector<int> a, vector<int> b) {
        if (a.size() > b.size()) {
            a.insert(a.end(), b.begin(), b.end());
            return a;
        }
        else {
            b.insert(b.end(), a.begin(), a.end());
            return b;
        }
    }

public:
    /*
        Get all primes between two numbers split between a set of threads
    */
    vector<int> allPrimes(int start, int end, int threads) {
        //Define variables
        vector<thread> allThreads = {};
        mutex primeLock;
        vector<int> allPrimes = {};
        //Split the work between threads
        vector<vector<int>> work = splitWork(start, end, threads);
        //For every piece of work in the thread
        for (auto& i : work) {
            //The thread begins at it's given value
            int threadBegin = i[0];
            //The thread ends at it's given value
            int threadEnd = i[1];
            //Add a thread
            allThreads.push_back(thread([&primeLock, &allPrimes, threadBegin, threadEnd, this] {
                //Find the primes outside of the lock. This is the actual work.
                vector<int> threadPrimeVector = threadPrimes(threadBegin, threadEnd);
                //Lock the allPrimes variable
                primeLock.lock();
                //Add the primes found to the allPrimes vector
                allPrimes = concat(allPrimes, threadPrimeVector);
                //Unlock the allPrimes variable
                primeLock.unlock();
            }));
        }
        //Join all threads
        for (auto& thread : allThreads) {
            thread.join();
        }
        //Sort all primes
        sort(allPrimes.begin(), allPrimes.end());
        return allPrimes;
    }

    /*
        Print a vector in a line
    */
    void printPrimes(vector<int> primes) {
        for (auto& i : primes) {
            cout << i << ",";
        }
        cout << endl;
    }
};


int main() {
    PrimeChecker primeChecker;
    vector<int> somePrimes = primeChecker.allPrimes(100, 200, 5);
    cout << somePrimes.size() << " primes found: " << endl;
    primeChecker.printPrimes(somePrimes);
    return 0;
}
