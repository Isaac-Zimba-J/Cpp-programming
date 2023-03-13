#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <vector>
#include <condition_variable>

using namespace std;

int numberOfElf = 6;
int numberAllowed = 3;
int elfNumberCounter = 0;
mutex elfMutex;
condition_variable elfCV;

int reindeerCounter = 0;
int numberOfReindeer = 0;
mutex reindeerMutex;
condition_variable reindeerCV;

mutex santaMutex;
condition_variable santaCV;
int santaCounter = 0;

void prepareSleigh() {
    cout << "\nSanta is preparing his sleigh for christmas\n";
    cout << "\nSanta has gone for christmas\n";
    cout << "\nSanta is back from christmas and is continuing with the remaining elves\n";
}

void getHitched() {
    cout << "\nReindeer are getting hitched\n";
}

void getMoreElves() {
    lock_guard<mutex> lock(elfMutex);
    cout << "\nSanta has gotten another set of elves\n";
    cout << "\nReducing number of elves at the door\n";

    numberOfElf -= 3;
    numberAllowed = 3;
    elfNumberCounter += 1;
}

void getHelp() {
    cout << "\nSanta is assisting the elf\n";
    this_thread::sleep_for(chrono::milliseconds(rand() % 3000 + 1000));
}

void elfCodeChecker(int number) {
    unique_lock<mutex> lock(elfMutex);
    while (numberAllowed > 0) {
        elfCV.wait(lock, []{ return (numberAllowed > 0); });
        cout << "\nElf " << number << " is opening Santa's door\n";
        getHelp();
        numberAllowed--;
        cout << "\nElf number " << number << " is getting help from Santa\n";
        number++;
        cout << "\nSanta has helped, so is going back to sleep\n";
    }
    lock.unlock();
    elfCV.notify_all();
    cout << "\n------ Santa is done ------\n";
}

void reindeerArrived() {
    unique_lock<mutex> lock(reindeerMutex);
    while (numberOfReindeer != 9) {
        reindeerCV.wait(lock, []{ return (numberOfReindeer == 9); });
    }
    cout << "\nNumber of reindeer is " << numberOfReindeer << "\n";
    cout << "---all reindeer are ready!---\n";
    cout << "\nReindeer calling get hitched\n";
    getHitched();
    reindeerCounter++;
    lock.unlock();
    reindeerCV.notify_all();
}
void remindcomeback(){
    auto now=system_ clock::now();
    auto vacation =now;
    auto comebackTime = now + months(12);
    time_t comebackTimeT= system_clock::to_time_t(comebackTime);
    cout<<"Reminder :comeback!its been twelve months since you went for vacation. Time is:"<<ctime(&comebackTimeT)<<Endl;
}


void Santa(int number) {
    cout << "\nSanta is sleeping\n";
    while (santaCounter < number) {
        unique_lock<mutex> lock(santaMutex);
        reindeerArrived();
        prepareSleigh();
        santaCounter++;
        lock.unlock();
        santaCV.notify_one();
    }
}

int main() {
    thread santaThread(Santa, 1);
    santaThread.detach();
    //call the remindcomeback function to remind some one to come back after twelve months
    remindcomeback();

    vector<thread> elfThreads;
    int numb = numberOfElf / 3;
    for (int i = 0; i < numb; i++) {
        elfThreads.emplace_back(elfCodeChecker, i+1);
    }
    for (auto& t : elfThreads) {
        t.join();
    }
    return 0;
}
