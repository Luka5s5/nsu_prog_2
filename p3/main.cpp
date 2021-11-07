#include <set>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Matrix;
class Part;
struct PartNode;

class Part
{
private:
    friend class Matrix;
    bool type, dead;
    Matrix *m;
    int t;

public:
    Part(bool type, int ind, Matrix *m);
    int &operator[](int _x);
    ~Part();
    Part(){};
    Part &operator=(const Part &that);
};

struct PartNode
{
    PartNode *next, *prev;
    Part *p;
    PartNode(Part *p)
    {
        this->p = p;
        next = prev = nullptr;
    }
};

class Matrix
{
private:
    friend class Part;
    int **a;
    int n;

    PartNode *list;

    void init(int _n)
    {
        list = nullptr;

        n = _n;
        a = new int *[n];
        for (int i = 0; i < n; i++)
            a[i] = new int[n]();
    }

    void copy_init(const Matrix &that)
    {
        n = that.get_size();
        init(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                set_ij(i, j, that.get_ij(i, j));
            }
        }
    }

    void add_part(Part *p)
    {
        if (list == nullptr)
        {
            list = new PartNode(p);
            return;
        }
        list->prev = new PartNode(p);
        list->prev->next = list;
        list = list->prev;
    }
    void remove_part(Part *p)
    {
        for (PartNode *now = list; now != nullptr; now = now->next)
        {
            if (now->p == p)
            {
                if (now->next)
                    now->next->prev = now->prev;
                if (now->prev)
                    now->prev->next = now->next;
                if (list == now)
                    list = now->next;
                delete now;
                break;
            }
        }
    }

public:
    void print_self_address()
    {
        cout << a << endl;
    }

    int get_size() const { return n; }
    Matrix() : n(0), a(nullptr) {}
    Matrix(int _n)
    {
        init(_n);
        for (int i = 0; i < n; i++)
            a[i][i] = 1;
    }
    Matrix(int _n, int *_arr)
    {
        init(_n);
        for (int i = 0; i < n; i++)
            a[i][i] = _arr[i];
    }
    Matrix(const Matrix &orig)
    {
        copy_init(orig);
    }
    Matrix operator()(int r, int c) const
    {
        r--;
        c--;
        Matrix ans(n - 1);
        for (int i = 0; i < n - 1; i++)
            for (int j = 0; j < n - 1; j++)
                ans.a[i][j] = a[i + (i >= r)][j + (j >= c)];
        return ans;
    }
    Matrix operator+(const Matrix &b) const
    {
        Matrix ans(n);
        if (n != b.n)
            throw;
        for (int i = 0; i < n; i++)
        {
            ans.a[i][i] = 0;
            for (int j = 0; j < n; j++)
            {
                ans.a[i][j] = b.a[i][j] + a[i][j];
            }
        }
        return ans;
    }
    Matrix operator-(const Matrix &b) const
    {
        Matrix ans(n);
        if (n != b.get_size())
            throw;
        for (int i = 0; i < n; i++)
        {
            ans.a[i][i] = 0;
            for (int j = 0; j < n; j++)
            {
                ans.a[i][j] = b.a[i][j] - a[i][j];
            }
        }
        return ans;
    }
    Matrix &operator=(const Matrix &that)
    {
        if (this == &that)
            return *this;
        for (int i = 0; i < n; i++)
            delete[] a[i];
        delete[] a;
        copy_init(that);
    }
    bool operator==(const Matrix &b)
    {
        bool ans = true;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (a[i][j] != b.a[i][j])
                    return false;
        return true;
    }
    bool operator!=(const Matrix &b)
    {
        return !operator==(b);
    }
    Matrix operator~() const
    {
        Matrix ans = (*this);
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
            {
                int tmp = ans.a[i][j];
                ans.a[i][j] = ans.a[j][i];
                ans.a[j][i] = tmp;
            }
        return ans;
    }
    Matrix operator*(const Matrix &b) const
    {
        if (n != b.get_size())
            throw;
        Matrix ans(n), tb = ~b;
        for (int r1 = 0; r1 < n; r1++)
        {
            ans.a[r1][r1] = 0;
            for (int r2 = 0; r2 < n; r2++)
            {
                for (int i = 0; i < n; i++)
                {
                    ans.a[r1][r2] = ans.a[r1][r2] + tb.a[r2][i] * a[r1][i];
                }
            }
        }
        return ans;
    }
    operator size_t() const
    {
        size_t ans = 0;
        int p = 31, mul = 1;
        for (int i = 0; i < get_size(); i++)
        {
            for (int j = 0; j < get_size(); j++)
            {
                ans += mul * get_ij(i, j);
                mul *= p;
            }
        }
        return ans;
    }

    Part operator[](const int &ind)
    {
        if (ind >= n or ind < 0)
        {
            cout << "Shitty index in int *operator[](const int &ind) const" << endl;
            throw;
        }
        return Part(0, ind, this);
    }
    Part operator()(const int &ind)
    {
        if (ind >= n or ind < 0)
        {
            cout << "Shitty index in int *operator[](const int &ind) const" << endl;
            throw;
        }
        return Part(true, ind, this);
    }

    int &get_ij(int i, int j) const
    {
        if (i < n && j < n && i >= 0 && j >= 0)
            return a[i][j];
        else
            throw;
    }
    int set_ij(int i, int j, int val)
    {
        if (i < n && j < n && i >= 0 && j >= 0)
            a[i][j] = val;
        else
            throw;
        return a[i][j];
    }
    ~Matrix()
    {
        for (int i = 0; i < n; i++)
        {
            delete[] a[i];
        }
        delete[] a;

        PartNode *now = list;

        while (now != nullptr)
        {
            now->p->dead = true;
            PartNode *nxt = now->next;
            delete now;
            now = nxt;
        }
    }

    friend ostream &operator<<(ostream &os, const Matrix &m)
    {
        int n = m.get_size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                os << m.get_ij(i, j) << " ";
            os << endl;
        }
        return os;
    }
    friend istream &operator>>(istream &is, Matrix &m)
    {
        int n = m.get_size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                int tmp;
                is >> tmp;
                m.set_ij(i, j, tmp);
            }
        }
        return is;
    }
};

int &Part::operator[](int _x)
{
    int x = t, y = _x;
    if (type == true)
        swap(x, y);
    if (dead)
    {
        throw std::runtime_error("Матрица умерла :(((");
        //        return x;
    }
    else
    {
        return m->get_ij(x, y);
    }
}

Part::~Part()
{
    if (!dead)
    {
        this->m->remove_part(this);
    }
}

Part::Part(bool type, int ind, Matrix *m)
{
    this->type = type;
    this->m = m;
    this->t = ind;
    this->dead = 0;
    this->m->add_part(this);
}

Part &Part::operator=(const Part &that)
{
    if (this == &that)
        return *this;
    type = that.type;
    dead = that.dead;
    m = that.m;
    t = that.t;
    that.m->add_part(this);
    return *this;
}
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
                hash<t1> hash;
                size_t to = hash(entry->p.first) % n;
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

    hash_map(int _n = 16, double _limit = 0.6)
    {
        n = _n;
        limit = _limit;
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
        hash<t1> hash;
        size_t to = hash(key) % n;
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
        hash<t1> hash;
        size_t to = hash(key) % n;
        bool before = (arr[to]->head != nullptr);
        arr[to]->remove_by_key(key);
        if (arr[to]->head == nullptr && before)
        {
            cells--;
        }
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
}

template <typename t1, typename t2>
class multi_hash_map : public hash_map<t1, t2>
{
    void insert(t1 key, t2 val) override
    {
        if (this->cells / (this->n + .0) > this->limit)
            this->resize(this->n * 2);
        hash<t1> hash;
        size_t to = hash(key) % this->n;
        if (this->arr[to]->head == nullptr)
            this->cells++;
        this->arr[to]->push_back({key, val});
    }

    vector<t2> find_all(t1 key)
    {
        hash<t1> hash;
        size_t to = hash(key) % this->n;
        vector<t2> ans;
        for (auto nod = this->arr[to]->head; nod != nullptr; nod = nod->r)
            if (nod->p.first == key)
                ans.push_back(nod->p.second);
        return ans;
    }

    int count(t1 key)
    {
        hash<t1> hash;
        size_t to = hash(key) % this->n;
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