// A neural network that can learn

#include <iostream>
#include <ctime>
#include <cmath>
#define MAX_LAYERS 10
#define MAX_HEIGHT 10

struct NeuralSignal
{
    unsigned int length;
    float values[MAX_HEIGHT];
};

class Layer
{
    public:
        unsigned int nodeCount;
        NeuralSignal nodes;

        Layer(unsigned int _nodeCount)
        {
            nodeCount = _nodeCount;
            nodes.length = _nodeCount;

            for(unsigned int i = 0; i < nodeCount; i ++)
            {
                srand(time(NULL) * rand() * rand());
                nodes.values[i] = (float(rand() % 2000) - 1000.0f) * 0.001f;
            }
        }

        NeuralSignal Multiply(NeuralSignal inputs)
        {
            NeuralSignal output;
            output.length = inputs.length;

            for(int i = 0; i < inputs.length; i ++)
            {
                for(int j = 0; j < nodes.length; j ++)
                {
                    output.values[i] = sinf(nodes.values[j] * inputs.values[i] * 1000.0f);
                }
            }

            return output;
        }
};

int main()
{
    Layer layer = Layer(5);

    NeuralSignal signal;


    std :: cout << "Single layer neural network\n"<< std :: endl;

    std :: cout << "Number of input nodes : ";

    signal.length = 2;

    std :: cin >> signal.length;

    std :: cout << "\nEnter neuron values\n";

    for(int i = 0; i < signal.length; i ++)
    {
        std :: cout << 'N' << i + 1 << " : ";
        std :: cin >> signal.values[i];
    }

    NeuralSignal ext;
    ext.length = signal.length;

    std :: cout << "\nExpected Output\n";

    for(int i = 0; i < signal.length; i ++)
    {
        std :: cout << 'E' << i + 1 << " : ";
        std :: cin >> ext.values[i];
    }

    int EPOC;

    std :: cout << "\n Number of EPOCH(s) to train : ";
    std :: cin >> EPOC;
    std :: cout << std :: endl;

    std :: cout << "TRAINING\n...\n\n";

    NeuralSignal best;
    best.length = signal.length;

    for(int k = 0; k < EPOC; k ++)
    {
        NeuralSignal output = layer.Multiply(signal);
        NeuralSignal sub;
        sub.length = output.length;

        for(int i = 0; i < signal.length; i ++)
        {
            // std :: cout << 'O' << i + 1 << " : " << output.values[i] << std :: endl;
            
            
            sub.values[i] = best;
        }
    }

    std :: cout << "\nNeuron Output: \n";

    


    // std :: cout << output.values[0] << ' ' << output.values[1];
    
    return 0;
}