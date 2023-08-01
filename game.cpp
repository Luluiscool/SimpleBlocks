#include <iostream>
#include <ctime>

int main()
{
    int ROUNDS = 100000;

    int total = 0;

    for(int i = 0; i < ROUNDS; i ++)
    {
        int round = 0;
        int balls = 2;

        while(true)
        {
            round ++;

            srand(time(0) * rand() * rand());
            balls ++;

            if(rand() % balls == 0)
            {
                break;
            }

        }

        total += round;
        // std :: cout << round << std :: endl;
    }

    std :: cout << "\n\nAverage : " << total / ROUNDS << std :: endl;
}