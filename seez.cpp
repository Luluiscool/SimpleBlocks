#include <iostream>

int main()
{
    int t;

    std :: cin >> t;

    float multiple = 1;

    for(int i = 1; i < t + 1; i ++)
    {
        // std :: cout << i << " / " << i + 1 << '\n'; 
        multiple = multiple * ((float)(i) / (float)(i + 1));
    }

    std :: cout << multiple << '\n';

    system("pause");

    return 0;
}