#include <set>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "matrix.cpp"

namespace std
{
    template <>
    struct hash<Matrix>
    {
        size_t operator()(Matrix const &m) const noexcept
        {
            size_t ans = 0;
            int p = 31, mul = 1;
            for (int i = 0; i < m.get_size(); i++)
            {
                for (int j = 0; j < m.get_size(); j++)
                {
                    ans += mul * m.get_ij(i, j);
                    mul *= p;
                }
            }
            return ans;
        }
    };
};
template <typename t1, typename t2>
struct node
{
    pair<t1, t2> p;
    node *l, *r;
    node()
    {
        l = r = nullptr;
    }
    node(pair<t1, t2> _p) : node()
    {
        p = _p;
    }

    node<t1, t2> *get_head()
    {
        node<t1, t2> *head = this;
        for (; head->l != nullptr; head = head->l)
            ;
        return head;
    }
};

template <typename t1, typename t2>
struct list
{
    node<t1, t2> *head = nullptr, *tail = nullptr; // tail по итогу бесполезный, можно было делать push_front...

    void print_self()
    {
        cout << "head,tail: " << head << " " << tail << endl;
        for (auto nod = head; nod != nullptr; nod = nod->r)
        {
            cout << "[" << nod->p.first << " " << nod->p.second << "]";
            if (nod != tail)
                cout << " <-> ";
        }
        cout << endl;
    }

    list()
    {
        head = tail = nullptr;
    }

    void push_back(pair<t1, t2> p)
    {
        if (!tail)
        {
            head = tail = new node<t1, t2>(p);
        }
        else
        {
            tail->r = new node<t1, t2>(p);
            tail->r->l = tail;
            tail = tail->r;
        }
    }

    void clear()
    {
        for (auto p = head; p != nullptr;)
        {
            auto old = p->r;
            delete p;
            p = old;
        }
        head = tail = nullptr;
    }

    void remove_by_key(t1 key)
    {
        for (auto nod = head; nod != nullptr;)
        {
            auto old = nod->r;
            if (nod->p.first == key)
            {
                if (nod->l)
                    nod->l->r = nod->r;
                if (nod->r)
                    nod->r->l = nod->l;
                if (nod == head)
                    head = nod->r;
                if (nod == tail)
                    tail = nod->l;
                delete nod;
            }
            nod = old;
        }
    }
};

template <typename t1, typename t2>
class hash_map
{
    list<t1, t2> **arr;
    int n;
    int cells;
    double limit;
    double shrink;

    size_t find_to(t1 key)
    {
        hash<t1> hash;
        size_t to = hash(key) % n;
        return to;
    }

    void resize(int newsize)
    {
        int oldsize = n;
        list<t1, t2> **newarr = new list<t1, t2> *[newsize];
        n = newsize;
        cells = 0;
        for (int i = 0; i < n; i++)
        {
            newarr[i] = new list<t1, t2>();
        }
        for (int i = 0; i < oldsize; i++)
        {
            for (node<t1, t2> *entry = arr[i]->head; entry != nullptr; entry = entry->r)
            {
                size_t to = find_to(entry->p.first);
                cells += (newarr[to]->head == nullptr);
                newarr[to]->push_back(entry->p);
            }
            arr[i]->clear();
            delete arr[i];
        }
        delete[] arr;
        arr = newarr;
    }

public:
    t2 example(t1 foo, t2 bar){
        return foo+~~~~~~~~~~bar*foo<<2;
    }

struct Iterator
    {
        Iterator(int _ind, node<t1, t2> *_m_ptr, hash_map *_h_ptr)
        {
            ind = _ind;
            m_ptr = _m_ptr;
            h_ptr = _h_ptr;
        }

        pair<t1, t2> &operator*() const { return (m_ptr->p); }

        // Prefix increment
        Iterator &operator++()
        {
            if (m_ptr->r == nullptr)
            {
                bool any = false;
                for (int i = ind + 1; i < h_ptr->n; i++)
                {
                    if (h_ptr->arr[i]->head != nullptr)
                    {
                        ind = i;
                        any = true;
                    }
                }
                if (any)
                    m_ptr = h_ptr->arr[ind]->head;
                else
                    m_ptr = nullptr;
            }
            else
            {
                m_ptr = m_ptr->r;
            }
            return *this;
        }

        friend bool operator==(const Iterator &a, const Iterator &b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const Iterator &a, const Iterator &b) { return a.m_ptr != b.m_ptr; };

    private:
        int ind;
        node<t1, t2> *m_ptr;
        hash_map *h_ptr;
    };

    Iterator begin()
    {
        for (int i = 0; i < n; i++)
            if (arr[i]->head != nullptr)
                return Iterator(i, arr[i]->head, this);
    }
    Iterator end()
    {
        return Iterator(228, nullptr, (hash_map *)(1337));
    }

    bool reserve(int newsz)
    {
        if (newsz > n)
        {
            resize(newsz);
            return true;
        }
        return false;
    }

    pair<int, int> find_ans()
    {
        int ans1, ans2;
        ans1 = ans2 = 0;
        set<t2> s; // сет исключительно с целью быстро посчитать ответ, можно было без сета сложить все значения в массив и за n^2 посчитать кол-во уникальных - это дольше писать, а очень спать хочется(((
        for (int i = 0; i < n; i++)
        {
            for (auto entry = arr[i]->head; entry != nullptr; entry = entry->r)
            {
                ans1++;
                s.insert(entry->p.second);
            }
        }
        ans2 = s.size();
        return {ans1, ans2};
    }

    hash_map(int _n = 16, double _limit = 0.6, double _shrink=0.3)
    {
        n = _n;
        limit = _limit;
        shrink=_shrink;
        cells = 0;
        arr = new list<t1, t2> *[n];
        for (int i = 0; i < n; i++)
        {
            arr[i] = new list<t1, t2>();
        }
    }

    void insert(t1 key, t2 val)
    {
        if (cells / (n + .0) > limit)
            resize(n * 2);
        size_t to = find_to(key);
        for (auto nod = arr[to]->head; nod != nullptr; nod = nod->r)
        {
            if (nod->p.first == key)
            {
                nod->p.second = val;
                return;
            }
        }
        if (arr[to]->head == nullptr)
            cells++;
        arr[to]->push_back({key, val});
    }

    void erase(t1 key)
    {
        size_t to = find_to(key);
        bool before = (arr[to]->head != nullptr);
        arr[to]->remove_by_key(key);
        if (arr[to]->head == nullptr && before)
        {
            cells--;
        }
        if (cells / (n + .0) <=shrink)
            resize(n / 2);
    }

    Iterator find(t1 key)
    {
        size_t to = find_to(key);
        for (auto nod = arr[to]->head; nod != nullptr; nod = nod->r)
            if (nod->p.first == key)
                return Iterator(to, nod, this);
        return end();
    }

    ~hash_map()
    {
        for (int i = 0; i < n; i++)
            if (arr[i])
            {
                arr[i]->clear();
                delete arr[i];
            }
        delete[] arr;
    }

    void print_self()
    {
        cout << n << " " << cells << "\n";
        for (int i = 0; i < n; i++)
        {
            if (arr[i]->head == nullptr)
                continue;
            cout << i << " : ";
            for (auto entry = arr[i]->head; entry != nullptr; entry = entry->r)
            {
                cout << '{' << entry->p.first << ", " << entry->p.second << "}, ";
            }
            cout << endl;
        }
    }

    void cool_print_self()
    {
        cout << n << " " << cells << "\n";

        for (auto i : *(this))
        {
            cout << '{' << i.first << ", " << i.second << "}, ";
        }
        cout << endl;
    }
};

template <typename t1, typename t2>
void solve(int n)
{
    hash_map<t1, t2> m(1);
    while (n--)
    {
        char ch;
        cin >> ch;
        if (ch == 'A')
        {
            t1 key;
            t2 val;
            cin >> key >> val;
            m.insert(key, val);
        }
        else if (ch == 'R')
        {
            t1 key;
            cin >> key;
            m.erase(key);
        }
    }
    pair<int, int> ans = m.find_ans();
    cout << ans.first << " " << ans.second << endl;
    cout << "COOL PRINT:" << endl;
    m.cool_print_self();
    cout << "SHIT PRINT:" << endl;
    m.print_self();
}

template <typename t1, typename t2>
class multi_hash_map : public hash_map<t1, t2>
{
    void insert(t1 key, t2 val) override
    {
        if (this->cells / (this->n + .0) > this->limit)
            this->resize(this->n * 2);
        size_t to = this->find_to(key);
        ;
        if (this->arr[to]->head == nullptr)
            this->cells++;
        this->arr[to]->push_back({key, val});
    }

    vector<t2> find_all(t1 key)
    {
        size_t to = this->find_to(key);
        vector<t2> ans;
        for (auto nod = this->arr[to]->head; nod != nullptr; nod = nod->r)
            if (nod->p.first == key)
                ans.push_back(nod->p.second);
        return ans;
    }

    int count(t1 key)
    {
        size_t to = this->find_to(key);
        int ans = 0;
        for (auto nod = this->arr[to]->head; nod != nullptr; nod = nod->r)
            ans += (nod->p.first == key);
        return ans;
    }
};

template <typename t1>
void ssolve(char ch2, int n)
{
    if (ch2 == 'I')
    {
        solve<t1, int>(n);
    }
    else if (ch2 == 'D')
    {
        solve<t1, double>(n);
    }
    else if (ch2 == 'S')
    {
        solve<t1, string>(n);
    }
    else if (ch2 == 'M')
    {
        solve<t1, Matrix>(n);
    }
}

int main()
{
    char t1, t2;
    cin >> t1 >> t2;
    int n;
    cin >> n;
    if (t1 == 'I')
    {
        ssolve<int>(t2, n);
    }
    else if (t1 == 'D')
    {
        ssolve<double>(t2, n);
    }
    else if (t1 == 'S')
    {
        ssolve<string>(t2, n);
    }
    else if (t1 == 'M')
    {
        ssolve<Matrix>(t2, n);
    }
}