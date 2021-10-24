#include <bits/stdc++.h>

using namespace std;
class anivector;
class Animal
{
private:
    inline pair<int, int> dec(int k)
    {
        if (k == 0)
        {
            return {0, -1};
        }
        if (k == 1)
        {
            return {1, 0};
        }
        if (k == 2)
        {
            return {0, 1};
        }
        return {-1, 0};
    }

protected:
    pair<int, int> xy;
    int d;
    int born;
    int eaten;
    int moves_made;
    int moves_need;
    int step;
    int lifespan;

public:
    Animal()
    {
        born = moves_made = eaten = 0;
    }
    Animal(int moves_need, pair<int, int> xy, int d) : Animal()
    {
        this->moves_need = moves_need;
        this->xy = xy;
        this->d = d;
        this->step = step;
    }
    Animal(Animal *that, int t)
    {
        (*this) = (*that);
        this->born = t;
    }
    void move(int n, int m)
    {
        pair<int, int> tempd = dec(d);
        xy = {(n + xy.first + step * tempd.first) % n, (m + xy.second + step * tempd.second) % m};
        moves_made++;
        d = (d + (moves_made == moves_need)) % 4;
        moves_made %= moves_need;
    }

    bool is_alive()
    {
        return (born != -1);
    }

    void kill()
    {
        born = -1;
    }

    pair<int, int> getxy()
    {
        return xy;
    }

    virtual char type() { return 'd'; };
    virtual void eat(anivector &a){};
    virtual Animal *breed(int t) { return nullptr; };
    void die(int t)
    {
        if ((t - born) == lifespan)
            kill();
    }
};
class anivector
{
    Animal **a;
    int n, now;

    void resize()
    {
        Animal **b = new Animal *[n *= 2];
        for (int i = 0; i < n; i++)
            b[i] = a[i];
        delete[] a;
        a = b;
    }

    void copy(const anivector &that)
    {
        n = that.size();
        now = that.len();
        a = new Animal *[n];
        for (int i = 0; i < now; i++)
        {
            a[i] = that.a[i];
        }
    }

public:
    int size() const
    {
        return n;
    }
    int len() const
    {
        return now;
    }

    anivector()
    {
        a = new Animal *[16];
        n = 16;
        now = 0;
    }

    void push_back(Animal *p)
    {
        if (now >= n)
        {
            resize();
        }
        a[now++] = p;
    }

    anivector(const anivector &orig)
    {
        copy(orig);
    }

    anivector &operator=(const anivector &that)
    {
        if (this == &that)
            return *this;
        delete[] a;
        copy(that);
    }

    Animal *&operator[](int i)
    {
        return a[i];
    }

    ~anivector()
    {
        delete[] a;
    }
};

class Wolf : public Animal
{
    using Animal::Animal;

    char type() override
    {
        return 'w';
    }
    void eat(anivector &a) override
    {
        for (int i = 0; i < a.len(); i++)
        {
            Animal *anim2 = a[i];
            if (!(anim2->type() == 'r' && anim2->is_alive() && xy == anim2->getxy()))
                continue;
            eaten++;
            anim2->kill();
        }
    }
    Animal *breed(int t) override
    {
        if (eaten >= 2)
        {
            eaten = 0;
            return new Wolf(this, t);
        }
        return nullptr;
    }

public:
    Wolf(int moves_need, pair<int, int> xy, int d) : Animal(moves_need, xy, d)
    {
        step = 2;
        lifespan = 15;
    }
};

class Rabbit : public Animal
{
    char type() override
    {
        return 'r';
    }
    using Animal::Animal;
    Animal *breed(int t)
    {
        if ((t - born) % 5 == 0 && t != born)
        {
            return new Rabbit(this, t);
        }
        return nullptr;
    }

public:
    Rabbit(int moves_need, pair<int, int> xy, int d) : Animal(moves_need, xy, d)
    {
        step = 1;
        lifespan = 10;
    }
};

class Hyena : public Wolf
{
    char type() override
    {
        return 'h';
    }
    using Wolf::Wolf;
    void eat(anivector &a) override
    {
        for (int i = 0; i < a.len(); i++)
        {
            Animal *anim2 = a[i];
            if (!(eaten < 2 && anim2->is_alive() && xy == anim2->getxy()))
                continue;
            eaten++;
            anim2->kill();
        }
    }
    Animal *breed(int t)
    {
        if (eaten >= 2)
        {
            eaten = 0;
            return new Hyena::Wolf::Animal(this, t);
        }
        return nullptr;
    }
};
class Simulation
{
    int now;
    anivector a;
    int n, m;

public:
    void print_ans()
    {
        vector<vector<int>> ans(n, vector<int>(m, 0));
        for (int i = 0; i < a.len(); i++)
        {
            Animal *anim = a[i];
            ans[anim->getxy().first][anim->getxy().second] += (anim->type() == 'r' ? 1 : -1);
        }
        for (auto i : ans)
        {
            for (auto j : i)
            {
                if (j == 0)
                {
                    cout << '#';
                }
                else
                {
                    cout << j;
                }
            }
            cout << endl;
        }
    }

    void add_animal(Animal *ex)
    {
        if (ex != nullptr)
            a.push_back(ex);
    }

    Simulation()
    {
        now = n = m = 0;
        a = {};
    }

    Simulation(int n, int m) : Simulation()
    {
        this->n = n;
        this->m = m;
    }

    void dobit_vyzhevshih()
    {
        anivector temp1;
        for (int i = 0; i < a.len(); i++)
        {
            Animal *anim = a[i];
            if (anim->is_alive())
                temp1.push_back(anim);
            else
                delete anim;
        }
        a = temp1;
    }

    void make_n_steps(int n)
    {
        for (int time = 1; time <= n; time++)
        {
            for (int i = 0; i < a.len(); i++)
            {
                Animal *anim = a[i];
                anim->move(this->n, m);
            }
            for (int i = 0; i < a.len(); i++)
            {
                Animal *anim1 = a[i];
                anim1->eat(a);
            }
            dobit_vyzhevshih();
            for (int i = 0; i < a.len(); i++)
            {
                add_animal(a[i]->breed(time));
            }
            for (int i = 0; i < a.len(); i++)
            {
                Animal *anim1 = a[i];
                anim1->die(time);
            }
            dobit_vyzhevshih();
        }
        now += n;
    };
    ~Simulation()
    {
        for (int i = 0; i < a.len(); i++)
            delete a[i];
    }
};

int main()
{
    int n, m, t;
    cin >> n >> m >> t;
    Simulation sim(n, m);
    int r, w, h;
    cin >> r >> w >> h;

    for (int i = 0; i < r + w + h; i++)
    {
        int tp = (i < r ? 1 : (i < r + w ? -1 : -2));
        int x, y, d, k;
        cin >> x >> y >> d >> k;
        if (tp == 1)
        {
            sim.add_animal(new Rabbit(k, {x, y}, d));
        }
        if (tp == -1)
        {
            sim.add_animal(new Wolf(k, {x, y}, d));
        }
        if (tp == -2)
        {
            sim.add_animal(new Hyena(k, {x, y}, d));
        }
    }

    sim.make_n_steps(t);
    sim.print_ans();
}