#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
#include "Term.h"

#define debug 1;

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

    Env():access_link(NULL){}

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

int run_func(Closure &closure,vector<Term*> &args){
    //TODO
    return 0;
}

int run_block(Term *t,Env *envp){
    assert(t->kind==Block);
    Env *env=new Env(); //???
    env->access_link=envp;
    for(Term *s:t->sons){ //block -> Begin (function|command)* End
        assert(s->kind==Function||s->kind==Command);
        if(s->kind==Function){
            env->funcs[s->sons[0]->name]=Closure(s,env);
        }else if(s->kind==Command){
            switch(s->subtype){
            case Declaration:
                env->vars[s->sons[0]->name]=0;
                break;
            case Assign:
                env->setvar(s->sons[0]->name,run_expr(s->sons[1],env));
                break;
            case Call:
                run_func(env->getfunc(t->sons[0]->name),t->sons);
                break;
            case Read:
                int n;
                input>>n;
                env->setvar(s->sons[0]->name,n);
                break;
            case Print:
                output<<run_expr(s->sons[0],env)<<endl;
                break;
            case If:
                if(run_boolexpr(s->sons[0],env))
                    run_block(s->sons[1],env);
                else
                    run_block(s->sons[2],env);
                break;
            case While:
                while(run_boolexpr(s->sons[0],env))
                    run_block(s->sons[1],env);
                break;
            case Return:
                return run_expr(s->sons[0],env);
            default:
                assert(0);
            }
        }
    }
    return 0;
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
        return run_func(env->getfunc(t->sons[0]->name),t->sons);
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
    //t->print();
    run_block(t,new Env());
    return 0;
}