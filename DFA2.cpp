#include <fstream>
#include <string>
#include <set>


#define N 22
#define M 12
#define e -1
#define halt -2


using std::string;
using std::set;


struct context_t
{
    char c;
    int q;
    string type;
    string buffer;

    set<string> gloabals;
    set<string> locals;
};


typedef bool (*action_t)(context_t*);


struct edge_t
{
    int q{ e };
    action_t a{ nullptr };
};


// действия
bool a0(context_t* context);
bool a1(context_t* context);
bool a2(context_t* context);
bool a3(context_t* context);
bool a4(context_t* context);
bool a5(context_t* context);
bool a6(context_t* context);
bool a7(context_t* context);
bool a8(context_t* context);
bool a9(context_t* context);

// алфавит
std::string alphabet[M]={
    "\t\n\v\f\r ",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz",
    "0123456789",
    ";", ",", "[", "]",  "(", ")", "\0"
};


// функция переходов
edge_t delta[N][M]{
    // space    a-z      0-9      ;        ,        [        ]        (        )        \0
    {  {0,   }, {1, a1}, {     }, {7,   }, {     }, {     }, {     }, {     }, {     }, {halt } }, // q0
    {  {2, a2}, {1, a1}, {1, a1}, {7, a2}, {0, a2}, {3, a2}, {     }, {18,a2}, {     }, {     } }, // q1
    {  {2    }, {     }, {     }, {7,   }, {0    }, {3    }, {     }, {18,a2}, {     }, {     } }, // q2

    {  {3    }, {     }, {4    }, {     }, {     }, {     }, {6    }, {     }, {     }, {     } }, // q3
    {  {5    }, {     }, {4    }, {     }, {     }, {     }, {6,   }, {     }, {     }, {     } }, // q4
    {  {5    }, {     }, {6    }, {     }, {     }, {     }, {6,   }, {     }, {     }, {     } }, // q5
    {  {3    }, {     }, {     }, {7,   }, {0    }, {     }, {     }, {     }, {     }, {     } }, // q6

    {  {7,   }, {8, a1}, {     }, {7, a2}, {     }, {     }, {1,   }, {     }, {     }, {halt } }, // q7
    {  {8, a3}, {8, a1}, {     }, {7, a2}, {     }, {     }, {1,   }, {     }, {     }, {     } }, // q8
    {  {9,   }, {10,a1}, {     }, {7, a2}, {     }, {     }, {1,   }, {     }, {     }, {     } }, // q9
    {  {10,a4}, {10,a1}, {     }, {7, a2}, {     }, {     }, {1,   }, {     }, {     }, {     } }, // q10

    {  {11   }, {12,a1}, {     }, {     }, {     }, {     }, {     }, {     }, {2, a8}, {     } }, // q11
    {  {13,a7}, {12,a1}, {12,a1}, {     }, {18,a7}, {14,a7}, {     }, {     }, {2, a8}, {     } }, // q12
    {  {13   }, {     }, {     }, {     }, {18   }, {14,a7}, {     }, {     }, {2, a8}, {     } }, // q13

    {  {14   }, {     }, {15   }, {     }, {     }, {     }, {17   }, {     }, {     }, {     } }, // q14
    {  {16   }, {     }, {15   }, {     }, {     }, {     }, {17   }, {     }, {     }, {     } }, // q15
    {  {16   }, {     }, {17   }, {     }, {     }, {     }, {17   }, {     }, {     }, {     } }, // q16
    {  {14   }, {     }, {     }, {     }, {18   }, {     }, {     }, {     }, {2, a8}, {     } }, // q17

    {  {18   }, {19,a1}, {     }, {     }, {     }, {     }, {12   }, {     }, {2, a8}, {     } }, // q18
    {  {19,a5}, {19,a1}, {     }, {     }, {     }, {     }, {12   }, {     }, {2, a8}, {     } }, // q19
    {  {20   }, {21,a1}, {     }, {     }, {     }, {     }, {12   }, {     }, {2, a8}, {     } }, // q20
    {  {21,a6}, {21,a1}, {     }, {     }, {     }, {     }, {12   }, {     }, {2, a8}, {     } }, // q21
};


bool check(context_t* context)
{
    int i = 0;
    for (; i < M; i++)
        // костылина без которой никак
        if (alphabet[i].find(context->c) != std::string::npos || alphabet[i][0] == context->c)
            break;

    if (i == M)
        return true;
    
    int prev_q = context->q;
    int next_q = delta[prev_q][i].q;
    action_t a = delta[prev_q][i].a;
    context->q = next_q;

    if ((a != nullptr && !a(context)) || next_q == e)
        return true;
    return false;
}

                            
int main()                             
{            
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    context_t* context = new context_t{};
    int l{ 1 };
    int p{ 1 };

    context->q = 7;

    while (context->q != halt)
    {
        if (!fin.read(&context->c, 1))
            context->c = '\0';

        if (context->c == '\n')
        {
            l++;
            p = 1;
        }
        else if (check(context))
        {
            fout << l << " " << p;
            return 0;
        }

        p++;
    };

    fout << "OK";
}


bool istype(const string s)
{
    return (s == "void" || s == "bool" || s == "char" || s == "int" || s == "long" || s == "float" || s == "double");
}

bool istypespecifire(const string s)
{
    return (s == "long" || s == "short");
}

bool iscorrecttype(const string s)
{
    return true;
}


// очищает буфер 
bool a0(context_t* context)
{
    context->buffer.clear();
    return true;
}


// помещает символ в буфер
bool a1(context_t* context)
{
    context->buffer += context->c;
    return true;
}


// проверка уникальности имени глобальной переменной
bool a2(context_t* context)
{   
    if (!context->buffer.empty())
    {
        if (context->gloabals.find(context->buffer) != context->gloabals.end())
            return false;
        context->gloabals.insert(context->buffer);
    } 
    else if (context->c == ';')
    {
        context->type.clear();
    }

    return a0(context);
}


// проверка на тип или спецификатор типа
bool a3(context_t* context)
{
    if (istypespecifire(context->buffer))
    {
        context->type = context->buffer;
        context->q = 9;
        return a0(context);
    }

    else if (istype(context->buffer))
    {
        context->type = context->buffer;
        context->q = 0;
        return a0(context);
    }
    return false;
}

// проверка на тип
bool a4(context_t* context)
{
    if (istype(context->buffer))
    {
        context->type += " " + context->buffer;
        context->q = 0;
        return a0(context);
    }
    else
    {
        context->q = 2;
        return a2(context);
    }
}


// проверка на тип или спецификатор типа аргумента функции
bool a5(context_t* context)
{
    if (istypespecifire(context->buffer))
    {
        context->type = context->buffer;
        context->q = 18;
        return a0(context);
    }

    else if (istype(context->buffer))
    {
        context->type = context->buffer;
        context->q = 11;
        return a0(context);
    }
    return false;
}

// проверка на тип аргумента функции
bool a6(context_t* context)
{
    if (istype(context->buffer))
    {
        context->type += " " + context->buffer;
        context->q = 11;
        return a0(context);
    }
    else
    {
        context->q = 13;
        return a2(context);
    }
}

// проверка уникальности имени локальной переменной
bool a7(context_t* context)
{
    if (context->locals.find(context->buffer) != context->locals.end())
        return false;
    context->locals.insert(context->buffer);
    context->type.clear();
    return a0(context);
}


// стирает локальные переменные 
bool a8(context_t* context)
{
    if (a7(context)) {
        context->locals.clear();
        return a0(context);
    }
    return false;
}