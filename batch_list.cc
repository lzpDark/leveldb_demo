#include <iostream>
#include <deque>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <sstream>
#include <fstream>

using namespace std;

const int batch_size = 4;

struct Writer
{  
  string log;
  bool done;
  condition_variable cv_;
  

  void Wait(mutex& mute_)
  {
    unique_lock<mutex> lock(mute_, adopt_lock);
    cv_.wait(lock);
    lock.release();
  }

  void Signal()
  {
    cv_.notify_one();
  }
};

static mutex mute_;
static deque<Writer *> logList;
 
void func()
{

for(int l = 0;l<10000;l++)
  //while (true)
  {

    stringstream ss;
    ss << this_thread::get_id() << "_" << l ;
    string log = "data->" + ss.str();
    Writer w;
    w.log = log;
    w.done = false;

    cout << "start get lock " <<  this_thread::get_id() << endl;
    unique_lock<mutex> lock(mute_);
    logList.push_back(&w);
    { 
      ofstream file;
      file.open("./data.log", ios::app);
      file << log << endl;
      file.close();
    }
    while (!w.done && &w != logList.front())
    {
      cout << "wait " << this_thread::get_id() << endl;
      w.Wait(mute_);
    }
    if (w.done)
    {
      continue; // finished
    }

    // batch log & write 
    mute_.unlock();

    Writer *last_writer = &w;
    {
      unique_lock<mutex> lock(mute_);
      ofstream file;
      file.open("./log.log", ios::app);
      auto ite = logList.begin();
      for(int i = 0; ite != logList.end(); ite++, i++) {
        last_writer = *ite;
        if(i>=batch_size) {
          break;
        }
        cout << "#: " << this_thread::get_id() << " " << last_writer->log << endl;
        file << "#: " << this_thread::get_id() << " " << last_writer->log << endl;
      }
      
      file.close();
    }

    mute_.lock();

    // notify prev
    while (true)
    {
      Writer *head = logList.front();
      logList.pop_front();
      if (head != &w)
      {
        head->done = true;
        head->Signal();
      }
      if (head == last_writer)
      {
        break;
      }
    }

    // notify next
    if (!logList.empty())
    {
      { 
        ofstream file;
        file.open("./next.log", ios::app);
        file << logList.front()->log << endl;
        file.close();
      }
      logList.front()->Signal();
    }
    cout << "finished one loop " << this_thread::get_id() << endl;
  }
}

void func1() {
  while(true) { 
    
    sleep(2);
    unique_lock<mutex> lock(mute_);
    cout << "#: " << this_thread::get_id() << " " << "func1" << endl; 
  }
}

void func2() {
  while(true) { 
    
    sleep(2);
    unique_lock<mutex> lock(mute_);
    cout << "#: " << this_thread::get_id() << " " << "func2" << endl; 
  }
}
 
void func3() {
  while(true) { 
    
    sleep(2);
    unique_lock<mutex> lock(mute_);
    cout << "#: " << this_thread::get_id() << " " << "func3" << endl; 
  }
}

int main(int argc, char **argv)
{

  deque<Writer*> xx;
  auto ite = xx.begin();
Writer* p;
  for(; ite!= xx.end(); ite++) {
 
  }
  xx.front();
 
//  thread t1(func1);
//  thread t2(func2);
//  thread t3(func3);
    
//   t3.join();


thread t1(func);
thread t2(func);
thread t3(func);
thread t4(func);
thread t5(func);
thread t6(func);
thread t7(func);
thread t8(func);

t1.join();
t2.join();
t3.join();
t4.join();
t5.join();
t6.join();
t7.join();
t8.join();
  return 0;
}
