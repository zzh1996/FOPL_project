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

class Env{
public:
    map<std::string,int> vars;
    map<std::string,Closure> funcs;
    Env *access_link;
    bool ret;
    int retv;

    Env():access_link(NULL),ret(false),retv(0){}

    void setvar(string &name,int value){
        if(vars.find(name)!=vars.end())
            vars[name]=value;
        else if(access_link)
            access_link->setvar(name,value);
        else
            cout<<"name not found: "<<name<<endl;
    }

    int getvar(string &name){
        if(vars.find(name)!=vars.end())
            return vars[name];
        else if(access_link)
            return access_link->getvar(name);
        else{
            cout<<"name not found: "<<name<<endl;
            assert(0);
        }
    }

    Closure &getfunc(string &name){
        if(funcs.find(name)!=funcs.end())
            return funcs[name];
        else if(access_link)
            return access_link->getfunc(name);
        else{
            cout<<"name not found: "<<name<<endl;
            assert(0);
        }
    }
};

int run_expr(Term *t,Env *env);
bool run_boolexpr(Term *t,Env *env);
void run_block(Term *t,Env *envp,Env *fenv,Env *env=NULL);

int run_func(Closure &closure,vector<Term*> &args,Env *env){
    Env *fenv=new Env();
    fenv->access_link=closure.env;
    if(debug)cout<<closure.code->sons[0]->name<<" is called"<<endl;
    for(int i=1;i<args.size();i++){
        int arg_value=run_expr(args[i],env);
        string &arg_name=closure.code->sons[i]->name;
        if(debug)cout<<arg_name<<" = "<<arg_value<<endl;
        fenv->vars[arg_name]=arg_value;
    }
    run_block(closure.code->sons[closure.code->sons.size()-1],NULL,fenv,fenv);
    return fenv->retv;
}

void run_block(Term *t,Env *envp,Env *fenv,Env *env){
    assert(t->kind==Block);
    if(!env){
        env=new Env();
        env->access_link=envp;
    }
    for(Term *s:t->sons){ //block -> Begin (function|command)* End
        assert(s->kind==Function||s->kind==Command);
        if(s->kind==Function){
            env->funcs[s->sons[0]->name]=Closure(s,env);
        }else if(s->kind==Command){
            switch(s->subtype){
            case Declaration:
                //if(debug)cout<<"var "<<s->sons[0]->name<<endl;
                env->vars[s->sons[0]->name]=0;
                break;
            case Assign:
                env->setvar(s->sons[0]->name,run_expr(s->sons[1],env));
                break;
            case Call:
                run_func(env->getfunc(s->sons[0]->name),s->sons,env);
                break;
            case Read:
                int n;
                input>>n;
                env->setvar(s->sons[0]->name,n);
                break;
            case Print:
                int r;
                r=run_expr(s->sons[0],env);
                static bool first=true;
                if(!first)output<<" ";
                first=false;
                output<<r;
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
    }
}

int run_expr(Term *t,Env *env){
    assert(t->kind==Expr);
    switch(t->subtype){
    case Number:
        return t->number;
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
        return run_func(env->getfunc(t->sons[0]->name),t->sons,env);
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
