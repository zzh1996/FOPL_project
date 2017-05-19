#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
#include "Term.h"

#define debug 0

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
    map<std::string,Var> vars;
    Env *access_link;
    bool ret;
    Var retv;

    Env():access_link(NULL),ret(false),retv(Var(0)){}

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

Var run_expr(Term *t,Env *env);
bool run_boolexpr(Term *t,Env *env);
void run_block(Term *t,Env *envp,Env *fenv,int start_son=0,Env *env=NULL);

Var run_func(Var var,vector<Term*> &args,Env *env){
    assert(var.type==FUNC);
    Closure closure=var.func;
    Env *fenv=new Env();
    fenv->access_link=closure.env;
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

void run_block(Term *t,Env *envp,Env *fenv,int start_son,Env *env){
    assert(t->kind==Block);
    if(!env){
        env=new Env();
        env->access_link=envp;
    }
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
                break;
            case While:
                while(run_boolexpr(s->sons[0],env)){
                    run_block(s->sons[1],env,fenv);
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
        return run_func(env->getvar(t->sons[0]->name),t->sons,env);
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
    run_block(t,new Env(),NULL);
    output<<endl;
    return 0;
}
