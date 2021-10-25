#include <iostream>
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
            ans.a[i][i]= 0;
            for (int j = 0; j < n; j++)
            {
                ans.a[i][j]=b.a[i][j] + a[i][j];
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
            ans.a[i][i]=0;
            for (int j = 0; j < n; j++)
            {
                ans.a[i][j]=b.a[i][j] - a[i][j];
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
                if (a[i][j] != b.a[i][ j])
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
                ans.a[i][j]=ans.a[j][i];
                ans.a[j][i]=tmp;
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
            ans.a[r1][r1]= 0;
            for (int r2 = 0; r2 < n; r2++)
            {
                for (int i = 0; i < n; i++)
                {
                    ans.a[r1][r2]=ans.a[r1][r2]+tb.a[r2][i]*a[r1][i];
                }
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

int main()
{
    int n;
    cin >> n;
    int k;
    cin >> k;
    int *arr = new int[n];
    for (int i = 0; i < n; i++)
        arr[i] = k;
    Matrix A(n), B(n), C(n), K(n, arr), D(n);
    cin >> A;
    cout << A * A;
    Part prt;
    for (int i = 1; i < 3; i++)
    {
        Matrix F = A * A;
        prt = F(1);
    }
    cout << prt[0];
}