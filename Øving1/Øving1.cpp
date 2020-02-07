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

    vector<int> threadPrimes(int start, int end) {
        vector<int> primes = {};
        for (int i = start; i < end; i++) {
            if (isPrime(i)) {
                primes.push_back(i);
            }
        }
        return primes;
    }

    vector<vector<int>> splitWork(int start, int end, int threads) {
        if (threads == 1) {
            return { {start, end} };
        }
        vector<vector<int>> work = {};
        vector<int> threadWork = { 0, 0 };
        threadWork[0] = start;
        int lastStep = (end - start) / 2;
        threadWork[1] = start + lastStep;
        work.push_back(threadWork);

        for (int i = 1; i < threads - 1; i++) {
            threadWork[0] = threadWork[0] + lastStep;
            lastStep = lastStep / 2;
            threadWork[1] = threadWork[0] + lastStep;
            work.push_back(threadWork);
        }

        threadWork[0] = threadWork[0] + lastStep;
        threadWork[1] = end;
        work.push_back(threadWork);
        return work;
    }

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

    vector<int> allPrimes(int start, int end, int threads) {
        vector<thread> allThreads = {};
        mutex primeLock;
        vector<int> allPrimes = {};
        vector<vector<int>> work = splitWork(start, end, threads);
        int j = 0;
        for (auto& i : work) {
            int threadBegin = i[0];
            int threadEnd = i[1];
            allThreads.push_back(thread([&j, &primeLock, &allPrimes, threadBegin, threadEnd, this] {
                primeLock.lock();
                allPrimes = concat(allPrimes, threadPrimes(threadBegin, threadEnd));
                primeLock.unlock();
            }));
        }
        for (auto& thread : allThreads) {
            thread.join();
        }
        sort(allPrimes.begin(), allPrimes.end());
        return allPrimes;
    }

    void printPrimes(vector<int> primes) {
        for (auto& i : primes) {
            cout << i << ",";
        }
    }
};


int main() {
    PrimeChecker primeChecker;
    vector<int> somePrimes = primeChecker.allPrimes(200, 500, 5);
    cout << somePrimes.size() << " primes found: " << endl;
    primeChecker.printPrimes(somePrimes);
    return 0;
}