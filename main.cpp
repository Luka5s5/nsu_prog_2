#include <iostream>

using namespace std;

class Matrix
{
private:
    int **a;
    int n;
    void init(int _n)
    {
        n = _n;
        a = new int *[n];
        for (int i = 0; i < n; i++)
            a[i] = new int[n]();
    }

    void copy(const Matrix &that)
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

public:
    int get_size() const { return n; }
    void set_size()
    {
        // Оказывается в C++ нет реаллока... А векторы юзать нельзя.....
        // https://www.stroustrup.com/bs_faq2.html#renew
        return;
    };

    Matrix()
    {
        n = 0;
        a = NULL; // Насколько корректно было бы написать = nullptr??
    }
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
        copy(orig);
    }
    Matrix operator()(int r, int c) const
    {
        r--;
        c--;
        Matrix ans(n - 1);
        for (int i = 0; i < n - 1; i++)
            for (int j = 0; j < n - 1; j++)
                ans[i][j] = get_ij(i + (i >= r), j + (j >= c)); // Круто? По моему круто...
        return ans;
    }
    Matrix operator+(const Matrix &b) const
    {
        Matrix ans(n);
        if (n != b.get_size())
            throw;
        for (int i = 0; i < n; i++)
        {
            ans.set_ij(i, i, 0);
            for (int j = 0; j < n; j++)
            {
                ans.set_ij(i, j, b.get_ij(i, j) + get_ij(i, j));
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
            for (int j = 0; j < n; j++)
            {
                ans.set_ij(i, j, b.get_ij(i, j) - get_ij(i, j) - (i == j));
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
        copy(that);
    }
    bool operator==(const Matrix &b)
    {
        bool ans = true;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                ans &= (get_ij(i, j) == b.get_ij(i, j));
        return ans;
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
                int tmp = ans.get_ij(i, j);
                ans.set_ij(i, j, ans.get_ij(j, i));
                ans.set_ij(j, i, tmp);
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
            ans.set_ij(r1, r1, 0);
            for (int r2 = 0; r2 < n; r2++)
            {
                for (int i = 0; i < n; i++)
                {
                    ans.set_ij(r1, r2, ans.get_ij(r1, r2) + tb.get_ij(r2, i) * get_ij(r1, i));
                }
            }
        }
        return ans;
    }
    int *operator[](const int &ind) const
    {
        if (ind >= n or ind < 0)
        {
            cout << "Shitty index in int *operator[](const int &ind) const" << endl;
            throw;
        }
        return a[ind];
    }
    int get_ij(int i, int j) const
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
    void read()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cin >> a[i][j];
            }
        }
        return;
    }
    void print()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cout << a[i][j] << " ";
            }
            cout << endl;
        }
        return;
    }
    ~Matrix()
    {
        for (int i = 0; i < n; i++)
        {
            delete[] a[i];
        }
        delete[] a;
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
    cin >> A >> B >> C >> D;
    // cout << A << B << A + B;
    cout << ((A + B * (~C) + K) * (~D));
}