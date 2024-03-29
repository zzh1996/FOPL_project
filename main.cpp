#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
#include <set>
#include <ctime>
#include "Term.h"

#define debug 0 //输出调试信息开关
#define GC 1 //垃圾回收开关
#define TIME_GC 1 //两次GC的间隔时间（秒）
#define MONITOR 0 //输出GC的监视信息开关

using namespace std;

ifstream input("input.txt");
ifstream program("program.txt");
ofstream output("output.txt");

class Env;

class Closure{
public:
    Term *code;
    Env *env;
    Closure(){}
    Closure(Term *code,Env *env):code(code),env(env){}
};

enum VarType{NUM,FUNC,UNDEF};

class Var{
public:
    VarType type;
    int num;
    Closure func;

    Var():type(UNDEF){}
    Var(int n):type(NUM),num(n){}
    Var(Closure c):type(FUNC),func(c){}

    Var operator+(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return Var(num+rhs.num);
    }

    Var operator-(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return Var(num-rhs.num);
    }

    Var operator*(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return Var(num*rhs.num);
    }

    Var operator/(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return Var(num/rhs.num);
    }

    Var operator%(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return Var(num%rhs.num);
    }

    bool operator<(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return num<rhs.num;
    }
    bool operator>(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return num>rhs.num;
    }
    bool operator==(Var rhs){
        assert(type==NUM&&rhs.type==NUM);
        return num==rhs.num;
    }
};

std::ostream &operator<<(std::ostream &os,const Var &v) {
    switch(v.type){
    case NUM:
        os<<v.num;
        break;
    case FUNC:
        os<<v.func.code->sons[0]->name;
        break;
    case UNDEF:
        os<<"Undefined";
        break;
    }
    return os;
}

class Env{
public:
    static set<Env*> envs;
    map<std::string,Var> vars;
    Env *control_link;
    Env *access_link;
    bool ret;
    Var retv;

    Env(Env *control_link,Env* access_link):control_link(control_link),access_link(access_link),ret(false),retv(Var(0)){
        envs.insert(this);
    }

    ~Env(){
        envs.erase(this);
    }

    void setvar(string &name,Var var){
        if(vars.find(name)!=vars.end())
            vars[name]=var;
        else if(access_link)
            access_link->setvar(name,var);
        else
            cout<<"name not found: "<<name<<endl;
    }

    Var getvar(string &name){
        if(vars.find(name)!=vars.end())
            return vars[name];
        else if(access_link)
            return access_link->getvar(name);
        else{
            cout<<"name not found: "<<name<<endl;
            assert(0);
        }
    }
};

set<Env*> Env::envs;

void remove_env(Env *env,set<Env*> &s){ //从集合s中删除从env可达的所有活动记录
    if(env&&s.find(env)!=s.end()){
        s.erase(env);
        remove_env(env->control_link,s);
        remove_env(env->access_link,s);
        if(env->retv.type==FUNC)
            remove_env(env->retv.func.env,s);
        for(auto &i:env->vars){
            Var &v=i.second;
            if(v.type==FUNC)
                remove_env(v.func.env,s);
        }
    }
}

void gc(Env *env,Var *temp=NULL){ //回收从env和temp不可达的所有活动记录
#ifdef TIME_GC
    static time_t last_time=time(0);
    if(time(0)>=last_time+TIME_GC){
#endif
        if(GC){
            set<Env*> envs=Env::envs;
            remove_env(env,envs);
            if(temp&&temp->type==FUNC)
                remove_env(temp->func.env,envs);
            for(Env *e:envs)
                delete e;
        }
        if(MONITOR){
            cout<<time(0)<<"\t"<<Env::envs.size()<<endl;
        }
#ifdef TIME_GC
        last_time=time(0);
    }
#endif
}

Var run_expr(Term *t,Env *env);
bool run_boolexpr(Term *t,Env *env);
void run_block(Term *t,Env *envp,Env *fenv,int start_son=0,Env *env=NULL);

//var:函数 args:实参列表 env:当前活动记录
Var run_func(Var var,vector<Term*> &args,Env *env){
    assert(var.type==FUNC);
    Closure closure=var.func;
    Env *fenv=new Env(env,closure.env);
    if(debug)cout<<closure.code->sons[0]->name<<" is called"<<endl;
    for(int i=1;i<args.size();i++){
        Var arg_value=run_expr(args[i],env);
        string &arg_name=closure.code->sons[i]->name;
        if(debug)cout<<arg_name<<" = "<<arg_value<<endl;
        fenv->vars[arg_name]=arg_value;
    }
    run_block(closure.code->sons[closure.code->sons.size()-1],NULL,fenv,0,fenv);
    return fenv->retv;
}

//t:block的Term envp:外层活动记录 fenv:当前函数活动记录
//start_son:当前位置（sons中的第几个） env:新的活动记录
void run_block(Term *t,Env *envp,Env *fenv,int start_son,Env *env){
    assert(t->kind==Block);
    if(!env)
        env=new Env(envp,envp);
    if(start_son<t->sons.size()){
        Term *s=t->sons[start_son];
        assert(s->kind==Function||s->kind==Command);
        if(s->kind==Function){
            env->vars[s->sons[0]->name]=Var(Closure(s,env));
        }else if(s->kind==Command){
            switch(s->subtype){
            case Declaration:
                //if(debug)cout<<"var "<<s->sons[0]->name<<endl;
                for(Term *v:s->sons)
                    env->vars[v->name]=Var();
                break;
            case Assign:
                env->setvar(s->sons[0]->name,run_expr(s->sons[1],env));
                break;
            case Call:
                run_func(env->getvar(s->sons[0]->name),s->sons,env);
                gc(env);
                break;
            case Read:
                int n;
                input>>n;
                env->setvar(s->sons[0]->name,Var(n));
                break;
            case Print:
                {
                    Var r;
                    r=run_expr(s->sons[0],env);
                    if(r.type==NUM){
                        static bool first=true;
                        if(!first)output<<" ";
                        first=false;
                        output<<r.num;
                    }
                }
                break;
            case If:
                if(run_boolexpr(s->sons[0],env))
                    run_block(s->sons[1],env,fenv);
                else
                    run_block(s->sons[2],env,fenv);
                gc(env);
                break;
            case While:
                while(run_boolexpr(s->sons[0],env)){
                    run_block(s->sons[1],env,fenv);
                    gc(env);
                    if(fenv&&fenv->ret)return;
                }
                break;
            case Return:
                assert(fenv);
                fenv->retv=run_expr(s->sons[0],env);
                fenv->ret=true;
                break;
            default:
                if(debug)cout<<s->subtype<<endl;
                assert(0);
            }
        }
        if(fenv&&fenv->ret)return;
        run_block(t,env,fenv,start_son+1);
    }
}

Var run_expr(Term *t,Env *env){
    assert(t->kind==Expr);
    Var retv;
    switch(t->subtype){
    case Number:
        return Var(t->number);
    case VarName:
        return env->getvar(t->name);
    case Plus:
        return run_expr(t->sons[0],env)+run_expr(t->sons[1],env);
    case Minus:
        return run_expr(t->sons[0],env)-run_expr(t->sons[1],env);
    case Mult:
        return run_expr(t->sons[0],env)*run_expr(t->sons[1],env);
    case Div:
        return run_expr(t->sons[0],env)/run_expr(t->sons[1],env);
    case Mod:
        return run_expr(t->sons[0],env)%run_expr(t->sons[1],env);
    case Apply:
        retv=run_func(env->getvar(t->sons[0]->name),t->sons,env);
        gc(env,&retv);
        return retv;
    default:
        assert(0);
    }
}

bool run_boolexpr(Term *t,Env *env){
    assert(t->kind==BoolExpr);
    switch(t->subtype){
    case Lt:
        return run_expr(t->sons[0],env)<run_expr(t->sons[1],env);
    case Gt:
        return run_expr(t->sons[0],env)>run_expr(t->sons[1],env);
    case Eq:
        return run_expr(t->sons[0],env)==run_expr(t->sons[1],env);
    case And:
        return run_boolexpr(t->sons[0],env)&&run_boolexpr(t->sons[1],env);
    case Or:
        return run_boolexpr(t->sons[0],env)||run_boolexpr(t->sons[1],env);
    case Negb:
        return !run_boolexpr(t->sons[0],env);
    default:
        assert(0);
    }
}

int main()
{
    Term *t=parse(program);
    if(debug)t->print();
    run_block(t,new Env(NULL,NULL),NULL);
    output<<endl;
    return 0;
}
