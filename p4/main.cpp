#include <bits/stdc++.h>

#define EPS '`'

using namespace std;
int s;

struct node
{
    map<char, vector<node *>> to;
    bool is_final;
    node()
    {
        is_final = false;
    };
};

vector<node *> make_nfa()
{
    int n, nf;
    cin >> n >> s >> nf;
    vector<node *> nodes(n);
    for (int i = 0; i < n; i++)
        nodes[i] = new node();
    for (int i = 0; i < nf; i++)
    {
        int f;
        cin >> f;
        nodes[f]->is_final = true;
    }
    int np;
    cin >> np;
    for (int i = 0; i < np; i++)
    {
        int from, to;
        char ch;
        cin >> from >> to >> ch;
        nodes[from]->to[ch].push_back(nodes[to]);
    }
    return nodes;
}

void i_hate_memory_leaks(node *nod, int is_start=1){
    static map<node*,bool> used;
    used[nod]=1;
    for(auto i:nod->to){
        for(auto j:i.second){
            if(used.count(j)==0 or !used[j]){
                i_hate_memory_leaks(j,0);
            }
        }
    }
    if(is_start){
        for(auto p:used){
            if(p.second==1){
                // cout<<"Deleting "<<p.first<<endl;
                used[p.first]=0;
                delete p.first;
            }
        }
    }
}

struct kusok{
    node* start;
    // Should point to fin
    vector<node*> p_t_fin;

    kusok(char ch){
        start=new node();
        auto fin = new node();
        start->to[ch].push_back(fin);
        cout<<"!"<<start<<" "<<fin<<"!"<<endl;
        p_t_fin={fin};
    }

    kusok(){
        start=nullptr;
    }

    kusok operator+() {
        for(auto p:this->p_t_fin){
            p->to[EPS].push_back(start);
        }
        return *(this);
    }

    kusok operator*(){
        node* newstart = new node();
        newstart->to[EPS].push_back(this->start);
        this->p_t_fin.push_back(newstart);
        for(auto p:this->p_t_fin){
            p->to[EPS].push_back(start);
        }
        this->start=newstart;
        return *(this);
    }

    //concat .
    kusok operator+(kusok &a){
        for(auto i:this->p_t_fin){
            i->to[EPS].push_back(a.start);
        }
        a.start=this->start;
        return a;
    }

    //quest ?
    kusok operator!(){
        this->p_t_fin.push_back(start);
        return *(this);
    }

    kusok operator|(kusok &a){
        node* newstart = new node();
        newstart->to[EPS].push_back(this->start);
        newstart->to[EPS].push_back(a.start);
        for(auto i: this->p_t_fin)
            a.p_t_fin.push_back(i);
        a.start=newstart;
        return a;
    }
};

bool delim(char c)
{
    return false;
}

bool is_op(char c)
{
    return c == '|' || c == '*' || c == '+' || c == '?' || c == '.';
}

int priority(char op)
{
    return op == '+' || op == '*' || op == '?' ? 3 
                                                : op == '.' ? 2
                                                 : op == '|' ? 1
                                                               : -1;
}

void process_op(vector<kusok> &st, char op)
{
    if(op== '*'){
        kusok r=st.back();
        st.pop_back();
        st.push_back(*r);
    }
    if(op== '+'){
        kusok r=st.back();
        st.pop_back();
        st.push_back(+r);
    }
    if(op== '?'){
        kusok r=st.back();
        st.pop_back();
        st.push_back(!r);
    }
    if(op== '|'){
        kusok r = st.back();
        st.pop_back();
        kusok l = st.back();
        st.pop_back();
        st.push_back(l | r);
    }
    if(op== '.'){
        kusok r = st.back();
        st.pop_back();
        kusok l = st.back();
        st.pop_back();
        st.push_back(l + r);
    }
    
}

node* calc (string & s) {
	vector<kusok> st;
	vector<char> op;
	for (size_t i=0; i<s.length(); ++i)
		if (!delim (s[i]))
			if (s[i] == '(') {
				op.push_back ('(');
			}
			else if (s[i] == ')') {
				while (op.back() != '(')
					process_op (st, op.back()),  op.pop_back();
				op.pop_back();
			}
			else if (is_op (s[i])) {
				char curop = s[i];
				while (!op.empty() && priority(op.back()) >= priority(s[i]))
					process_op (st, op.back()),  op.pop_back();
				op.push_back (curop);
			}
			else {
                if((i+1>=s.length() || !is_op(s[i])) && i-1>=0 && isalpha(s[i-1])){
                    kusok tmp=st.back(), tmp1=kusok(s[i]);
                    st.pop_back();
                    st.push_back(tmp+tmp1);
                }
                else
                    st.push_back(kusok(s[i]));
			}
	while (!op.empty())
		process_op (st, op.back()),  op.pop_back();
	kusok ans=st.back();
    node* fin =new node();
    fin->is_final=1;
    for(auto i:ans.p_t_fin){
        i->to[EPS].push_back(fin);
    }
    return ans.start;

}

bool check(string s, node* start){
    set<node*> nowl={start};
    bool changes=true;
        while(changes){
            changes=false;
            for(auto i:nowl)
                if(i->to.count(EPS))
                    for(auto j:i->to[EPS])
                        if(!nowl.count(j)){
                            changes=true;
                            nowl.insert(j);
                        }
        }
    for(auto i:s){
        set<node*> newnow;
        for(auto n:nowl)
            if(n->to.count(i))
                for(auto j:n->to[i])
                    newnow.insert(j);
        bool changes1=true;
        while(changes1){
            changes1=false;
            for(auto i:newnow)
                if(i->to.count(EPS))
                    for(auto j:i->to[EPS])
                        if(!newnow.count(j)){
                            changes1=true;
                            newnow.insert(j);
                        }
        }
        nowl=newnow;
    }
    changes=true;
        while(changes){
            changes=false;
            for(auto i:nowl)
                if(i->to.count(EPS))
                    for(auto j:i->to[EPS])
                        if(!nowl.count(j)){
                            changes=true;
                            nowl.insert(j);
                        }
        }
    return any_of(nowl.begin(),nowl.end(),[](node* i){return i->is_final;});
}

void print(node* nod){
    static map<node*,bool> used;
    for(auto i:nod->to){
        cout<<nod<<" "<<(i.first==EPS?'`':i.first)<<" ";
        for(auto j:i.second)
            cout<<j<<" ";
        cout<<endl;
        for(auto j:i.second){
            if(used.count(j)==0 or !used[j]){
                used[j]=1;
                print(j);
            }
        }
    }
}

void dfs(node* v, map<node*,map<char,vector<node*>>> &ans, set<node*> &used){
    ans[v]=v->to;
    used.insert(v);
    for(auto p:v->to)
        for(auto to:p.second)
            if(used.count(to)==0)
                dfs(to,ans,used);
}

map<node*,map<char,vector<node*>>> make_g(node* st){
    map<node*,map<char,vector<node*>>> ans;
    set<node*> used;
    dfs(st,ans,used);
    return ans;
}

void dfs1(node* v, set<node*> &ret){
    ret.insert(v);
    for(auto p:v->to)
        for(auto to:p.second)
            if(ret.count(to)==0)
                dfs1(to,ret);
}

set<node*> make_one(node* nod){
    set<node*> ret;
    dfs1(nod,ret);
    return ret;
}

map<node*,set<node*>> make_eps_shit(set<node*> nodes){
    map<node*,set<node*>> ans;
    for(auto nod:nodes)
        ans[nod]=make_one(nod);
    return ans;
}

map<set<node*>,node*> make_powermap(set<node*> s, set<node*> &fins){
    map<set<node*>,node*> ret;
    vector<node*> v; for(auto i:s) v.push_back(i);
    for(unsigned long long mask=((1<<(s.size()))-1);mask!=-1;mask--){
        set<node*> ss;
        bool is_fin=false;
        for(int i=0;i<s.size();i++){
            if((1<<i)&mask){
                ss.insert(v[i]);
                if(fins.count(v[i])!=0)
                    is_fin=true;
            }
        }
        cout<<mask<<" ";
        // for(auto i:ss) cout<<i<<" ";
        cout<<endl;
        ret[ss]=new node();
        ret[ss]->is_final=is_fin;
    }
    return ret;
}

void dfs2(node* v, set<node*> &ans, set<node*> &used){
    used.insert(v);
    if(v->is_final) ans.insert(v);
    for(auto p:v->to)
        for(auto to:p.second)
            if(used.count(to)==0)
                dfs2(to,ans,used);
}

set<node*> find_finals(node* st){
    set<node*> used;
    set<node*> ans;
    dfs2(st,ans,used);
    return ans;
}

template<typename t1>
void setprint(set<t1> s){
    cout<<"{";
    for(auto i:s)
        cout<<i<<" ";
    cout<<"}";
}

node* determ(node* st){
    //я считаю раз тема задачи STL то я могу себе такое позволить, я давно хотел.............
    set<node*> finals=find_finals(st);
    
    map<node*,map<char,vector<node*>>> g=make_g(st);
    set<node*> s; for(auto p:g) s.insert(p.first);
    map<node*,set<node*>> eps_shit=make_eps_shit(s);
    map<set<node*>,node*> powermap=make_powermap(s,finals);
    
    vector<node*> v; for(auto i:s) v.push_back(i);
    for(unsigned long long mask=((1<<(s.size()))-1);mask!=-1;mask--){
        set<node*> ss;
        map<char,set<node*>> to;
        for(int i=0;i<s.size();i++){
            if((1<<i)&mask){
                ss.insert(v[i]);
            }
        }
        setprint(ss); cout<<endl;
        for(auto i:ss)
            for(auto j:i->to)
                for(auto k:j.second)
                    if(j.first!=EPS)
                        for(auto u:eps_shit[k])
                            to[j.first].insert(u);
        
        for(auto p:to){
            powermap[ss]->to[p.first].push_back(powermap[p.second]);
            cout<<p.first<<" : "<<"mapped"; setprint(ss); cout<<"to"; setprint(p.second); cout<<endl;
        }
    }
    return powermap[eps_shit[st]];
}

int main()
{
    string s1, s2;
    cin>>s1;
    node* aut=calc(s1);
    node* aut2=determ(aut);
    print(aut);
    print(aut2);
    while(cin>>s2){
        cout<<(check(s2,aut)?"YES\n":"NO\n");
        cout<<(check(s2,aut2)?"YES\n":"NO\n");
    }
    i_hate_memory_leaks(aut);
    i_hate_memory_leaks(aut2);
}