#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

using namespace std;

string down_folder = "./down/";
string dir = "./test_data/";

class Getter
{
private:
    string start;
    int n_threads;
    vector<thread> threads;
    queue<string> to_be_done;
    set<string> processed;
    mutex contLock;
    atomic<int> started_tasks;
    atomic<int> ended_tasks;

public:
    Getter(int count, string input)
    {
        n_threads = count;
        start = input;
        to_be_done.push(input);
        processed.insert(input);
    }

    void add_file(string href)
    {
        lock_guard<mutex> lockGuard(contLock);
        if (processed.find(href) == processed.end())
        {
            started_tasks++;
            to_be_done.push(href);
        }
    }

    static void copy_file(string filename)
    {
        ifstream in(dir + filename);
        string outputFile = down_folder + filename;
        ofstream out(outputFile);
        string input;
        out << in.rdbuf();
        in.close();
        out.close();
    }

    void add_finished(string filename)
    {
        lock_guard<mutex> lockGuard(contLock);
        processed.insert(filename);
    }

    pair<bool, string> get_file()
    {
        lock_guard<mutex> guard(contLock);
        if (!to_be_done.empty())
        {
            string nextFile = to_be_done.front();
            to_be_done.pop();
            return make_pair(true, nextFile);
        }
        return make_pair(false, "");
    }

    // Можно было конечно жахнуть мощный алгосик с полиномиальным хэшированием чтобы все вхождения за O(n) находить но сессия душит...
    void process_file(string file)
    {
        {
            lock_guard<mutex> lockGuard(contLock);
            if (processed.find(file) != processed.end() && file != start)
            {
                return;
            }
        }
        ifstream in(dir + file);
        copy_file(file);
        string line;
        char c;
        vector<string> hrefs;

        while (in)
        {
            line.clear();
            c = in.get();
            if (c == '<' && in.get() == 'a')
            {
                while (c != '\"')
                    c = in.get();
                c = in.get();
                while (c != '\"')
                {
                    line += c;
                    c = in.get();
                }
                hrefs.push_back(line.substr(7));
            }
        }
        for (auto i : hrefs)
            add_file(i);
        add_finished(file);
    }

    void do_work()
    {
        bool equal;
        do
        {
            equal = (ended_tasks.load() == started_tasks.load());
            auto file = get_file();
            if (file.first)
            {
                process_file(file.second);
                ended_tasks++;
                equal = false;
            }
            {
                lock_guard<mutex> lockGuard(contLock);
                // cout << processed.size() << " " << started_tasks.load() << " " << ended_tasks.load() << endl;
            }
        } while (!equal);
        // cout << "Thread id = " <<  this_thread::get_id() << " end!" << endl;
    }

    pair<int, int> get()
    {
        // cout << "Num of Threads = " << n_threads << endl;
        this_thread::sleep_for(chrono::milliseconds(2000));

        pair<int, int> result;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();

        started_tasks.store(1);
        ended_tasks.store(0);
        for (int i = 0; i < n_threads; ++i)
            threads.push_back(thread(&Getter::do_work, ref(*this)));
        for (int i = 0; i < n_threads; ++i)
        {
            threads[i].join();
        }

        end = chrono::system_clock::now();
        int time = chrono::duration_cast<chrono::microseconds>(end - start).count();
        result.second = time;
        result.first = processed.size();
        return result;
    }
};

int main()
{
    string s;
    int n;
    cin >> s >> n;
    Getter getter(n, s);
    pair<int, int> ans = getter.get();
    cout << ans.first << " " << (ans.second + .0) * (1e-6) << " s\n";
    return 0;
}