// Just better version of array i think

// Change this to whatever type you want

#define ARRAY_TYPE float
#define MAX_ARRAY_LENGTH 64

struct Carray
{
    int length;
    ARRAY_TYPE items[MAX_ARRAY_LENGTH];

    void Clear()
    {
        for(int i = 0; i < length; i ++)
        {
            items[i] = NULL;
        }
    }
};