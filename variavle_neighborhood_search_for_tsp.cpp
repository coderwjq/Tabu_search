////////////////////////
//TSP问题 变邻域搜索求解代码
//基于Berlin52例子求解
//作者：infinitor
//时间：2018-04-12
////////////////////////


#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <windows.h>
#include <memory.h>
#include <string.h>
#include <iomanip>

#define DEBUG

using namespace std;

#define CITY_SIZE 52 //城市数量


//城市坐标
typedef struct candidate
{
    int x;
    int y;
}city, CITIES;

//解决方案
typedef struct Solution
{
    int permutation[CITY_SIZE]; //城市排列
    int cost;                        //该排列对应的总路线长度
}SOLUTION;

//城市排列
int permutation[CITY_SIZE];
//城市坐标数组
CITIES cities[CITY_SIZE];


//berlin52城市坐标，最优解7542好像
CITIES berlin52[CITY_SIZE] =
{
{ 565,575 },{ 25,185 },{ 345,750 },{ 945,685 },{ 845,655 },
{ 880,660 },{ 25,230 },{ 525,1000 },{ 580,1175 },{ 650,1130 },{ 1605,620 },
{ 1220,580 },{ 1465,200 },{ 1530,5 },{ 845,680 },{ 725,370 },{ 145,665 },
{ 415,635 },{ 510,875 },{ 560,365 },{ 300,465 },{ 520,585 },{ 480,415 },
{ 835,625 },{ 975,580 },{ 1215,245 },{ 1320,315 },{ 1250,400 },{ 660,180 },
{ 410,250 },{ 420,555 },{ 575,665 },{ 1150,1160 },{ 700,580 },{ 685,595 },
{ 685,610 },{ 770,610 },{ 795,645 },{ 720,635 },{ 760,650 },{ 475,960 },
{ 95,260 },{ 875,920 },{ 700,500 },{ 555,815 },{ 830,485 },{ 1170,65 },
{ 830,610 },{ 605,625 },{ 595,360 },{ 1340,725 },{ 1740,245 }
};


//随机数产生函数，利用系统时间，精确到微妙，再加一个变量i组合产生随机数。
//单单用srand(time(NULL)) + rand()达不到效果，因为函数执行太快。时间间隔太短
int randEx(int i)
{
    LARGE_INTEGER seed;
    QueryPerformanceFrequency(&seed);
    QueryPerformanceCounter(&seed);
    srand((unsigned int)seed.QuadPart + i);

    return rand();
}


//计算两个城市间距离
int distance_2city(city c1, city c2)
{
    int distance = 0;
    distance = sqrt((double)((c1.x - c2.x)*(c1.x - c2.x) + (c1.y - c2.y)*(c1.y - c2.y)));

    return distance;
}

//根据产生的城市序列，计算旅游总距离
//所谓城市序列，就是城市先后访问的顺序，比如可以先访问ABC，也可以先访问BAC等等
//访问顺序不同，那么总路线长度也是不同的
//p_perm 城市序列参数
int cost_total(int * cities_permutation, CITIES * cities)
{
    int total_distance = 0;
    int c1, c2;
    //逛一圈，看看最后的总距离是多少
    for (int i = 0; i < CITY_SIZE; i++)
    {
        c1 = cities_permutation[i];
        if (i == CITY_SIZE - 1) //最后一个城市和第一个城市计算距离
        {
            c2 = cities_permutation[0];
        }
        else
        {
            c2 = cities_permutation[i + 1];
        }
        total_distance += distance_2city(cities[c1], cities[c2]);
    }

    return total_distance;
}

//获取随机城市排列
void random_permutation(int * cities_permutation)
{
    int i, r, temp;
    for (i = 0; i < CITY_SIZE; i++)
    {
        cities_permutation[i] = i; //初始化城市排列，初始按顺序排
    }


    for (i = 0; i < CITY_SIZE; i++)
    {
        //城市排列顺序随机打乱
        srand((unsigned int)time(NULL));
        int j = rand();
        r = randEx(++j) % (CITY_SIZE - i) + i;
        temp = cities_permutation[i];
        cities_permutation[i] = cities_permutation[r];
        cities_permutation[r] = temp;
    }
}


//颠倒数组中下标begin到end的元素位置
void swap_element(int *p, int begin, int end)
{
    int temp;
    while (begin < end)
    {
        temp = p[begin];
        p[begin] = p[end];
        p[end] = temp;
        begin++;
        end--;
    }
}


//邻域结构0 利用swap_element算子搜索
void neighborhood_zero(SOLUTION & solution, CITIES * cities)
{
    SOLUTION current_solution = solution;

    int count = 0;
    int max_no_improve = 10;

    do
    {
        count++;
        for (int i = 0; i < CITY_SIZE - 1; i++)
        {
            for (int k = i + 1; k < CITY_SIZE; k++)
            {
                current_solution = solution;
                swap_element(current_solution.permutation, i, k);

                current_solution.cost = cost_total(current_solution.permutation, cities);
                if (current_solution.cost < solution.cost)
                {
                    solution = current_solution;
                    count = 0; //count复位
                }

            }
         }

    } while (count <= max_no_improve);



}




// two_opt_swap算子
void two_opt_swap(int *cities_permutation, int b, int c)
{
    vector<int> v;
    for (int i = 0; i < b; i++)
    {
        v.push_back(cities_permutation[i]);
    }
    for (int i = c; i >= b; i--)
    {
        v.push_back(cities_permutation[i]);
    }
    for (int i = c + 1; i < CITY_SIZE; i++)
    {
        v.push_back(cities_permutation[i]);
    }

    for (int i = 0; i < CITY_SIZE; i++)
    {
        cities_permutation[i] = v[i];
    }

}

//邻域结构1 使用two_opt_swap算子
void neighborhood_one(SOLUTION & solution, CITIES *cities)
{
    int i, k, count = 0;
    int max_no_improve = 10;
    SOLUTION current_solution = solution;
    do
    {
        count++;
        for (i = 0; i < CITY_SIZE - 1; i++)
        {
            for (k = i + 1; k < CITY_SIZE; k++)
            {
                current_solution = solution;
                two_opt_swap(current_solution.permutation, i, k);

                current_solution.cost = cost_total(current_solution.permutation, cities);
                if (current_solution.cost < solution.cost)
                {
                    solution = current_solution;

                    count = 0; //count复位
                }

             }
          }
    }while (count <= max_no_improve);

}
//two_h_opt_swap算子
void two_h_opt_swap(int *cities_permutation, int a, int d)
{
    int n = CITY_SIZE;
    vector<int> v;
    v.push_back(cities_permutation[a]);
    v.push_back(cities_permutation[d]);
    // i = 1 to account for a already added
    for (int i = 1; i < n; i++)
    {
        int idx = (a + i) % n;
        // Ignore d which has been added already
        if (idx != d)
        {
            v.push_back(cities_permutation[idx]);
        }
    }

    for (int i = 0; i < v.size(); i++)
    {
        cities_permutation[i] = v[i];
    }

}


//邻域结构2 使用two_h_opt_swap算子
void neighborhood_two(SOLUTION & solution, CITIES *cities)
{
    int i, k, count = 0;
    int max_no_improve = 10;
    SOLUTION current_solution = solution;
    do
    {
        count++;
        for (i = 0; i < CITY_SIZE - 1; i++)
        {
            for (k = i + 1; k < CITY_SIZE; k++)
            {
                current_solution = solution;
                two_h_opt_swap(current_solution.permutation, i, k);

                current_solution.cost = cost_total(current_solution.permutation, cities);

                if (current_solution.cost < solution.cost)
                {
                    solution = current_solution;
                    count = 0; //count复位
                }

            }
        }
    } while (count <= max_no_improve);
}


//VND
//best_solution最优解
//current_solution当前解
void variable_neighborhood_descent(SOLUTION & solution, CITIES * cities)
{

    SOLUTION current_solution = solution;
    int l = 0;
    cout  <<"=====================VariableNeighborhoodDescent=====================" << endl;
    while(true)
    {
        switch (l)
        {
        case 0:
            neighborhood_zero(current_solution, cities);
            cout << setw(45) << setiosflags(ios::left) << "Now in neighborhood_zero, current_solution = " << current_solution.cost << setw(10) << setiosflags(ios::left) << "  solution = " << solution.cost << endl;
            if (current_solution.cost < solution.cost)
            {
                solution = current_solution;
                l = -1;
            }
            break;
        case 1:
            neighborhood_one(current_solution, cities);
            cout << setw(45) << setiosflags(ios::left)  <<"Now in neighborhood_one , current_solution = " << current_solution.cost << setw(10) << setiosflags(ios::left) << "  solution = " << solution.cost << endl;
            if (current_solution.cost < solution.cost)
            {
                solution = current_solution;
                l = -1;
            }
            break;
        case 2:
            neighborhood_two(current_solution, cities);
            cout << setw(45) << setiosflags(ios::left) << "Now in neighborhood_two , current_solution = " << current_solution.cost << setw(10) << setiosflags(ios::left) << "  solution = " << solution.cost << endl;
            if (current_solution.cost < solution.cost)
            {
                solution = current_solution;
                l = -1;
            }
            break;

        default:
            return;
        }
        l++;

    }

}

//将城市序列分成4块，然后按块重新打乱顺序。
//用于扰动函数
void double_bridge_move(int * cities_permutation)
{
    srand((unsigned int)time(NULL));
    int j = rand();
    int pos1 = 1 + randEx(++j) % (CITY_SIZE / 4);
    int pos2 = pos1 + 1 + randEx(++j) % (CITY_SIZE / 4);
    int pos3 = pos2 + 1 + randEx(++j) % (CITY_SIZE / 4);

    int i;
    vector<int> v;
    //第一块
    for (i = 0; i < pos1; i++)
    {
        v.push_back(cities_permutation[i]);
    }

    //第二块
    for (i = pos3; i < CITY_SIZE; i++)
    {
        v.push_back(cities_permutation[i]);
    }
    //第三块
    for (i = pos2; i < pos3; i++)
    {
        v.push_back(cities_permutation[i]);
    }

    //第四块
    for (i = pos1; i < pos2; i++)
    {
        v.push_back(cities_permutation[i]);
    }


    for (i = 0; i < (int)v.size(); i++)
    {
        cities_permutation[i] = v[i];
    }


}

//抖动
void shaking(SOLUTION &solution, CITIES *cities)
{
    double_bridge_move(solution.permutation);
    solution.cost = cost_total(solution.permutation, cities);
}


void variable_neighborhood_search(SOLUTION & best_solution, CITIES * cities)
{

    int max_iterations = 5;

    int count = 0, it = 0;

    SOLUTION current_solution = best_solution;

    //算法开始
    do
    {
        cout << endl << "\t\tAlgorithm VNS iterated  " << it << "  times" << endl;
        count++;
        it++;
        shaking(current_solution, cities);

        variable_neighborhood_descent(current_solution, cities);

        if (current_solution.cost < best_solution.cost)
        {
            best_solution = current_solution;
            count = 0;
        }

        cout << "\t\t全局best_solution = " << best_solution.cost << endl;

    } while (count <= max_iterations);


}


int main()
{

    SOLUTION best_solution;

    random_permutation(best_solution.permutation);
    best_solution.cost = cost_total(best_solution.permutation, berlin52);

    cout << "初始总路线长度 = " << best_solution.cost << endl;

    variable_neighborhood_search(best_solution, berlin52);

    cout << endl << endl << "搜索完成！ 最优路线总长度 = " << best_solution.cost << endl;
    cout << "最优访问城市序列如下：" << endl;
    for (int i = 0; i < CITY_SIZE; i++)
    {
        cout << setw(4) << setiosflags(ios::left) << best_solution.permutation[i];
    }

    cout << endl << endl;

    return 0;
}

