#include <iostream>

#include <openspell/lang_model.hpp>

using namespace NOpenSpell;

void PrintUsage(const char** argv) {
    std::cerr << "Usage: " << argv[0] << " mode args" << std::endl;
    std::cerr << "    train alphabet.txt dataset.txt resultModel.bin  - train model" << std::endl;
    std::cerr << "    interactive model.bin - input sentences and get score" << std::endl;
}

int Train(const std::string& alphabetFile,
          const std::string& datasetFile,
          const std::string& resultModelFile)
{
    TLangModel model;
    model.Train(datasetFile, alphabetFile);
    model.Save(resultModelFile);
    return 0;
}

int Interactive(const std::string& modelFile) {
    TLangModel model;
    model.Load(modelFile);
    while (true) {
        std::string text;
        std::cin >> text;
    }
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        PrintUsage(argv);
        return 42;
    }
    std::string mode = argv[1];
    if (mode == "train") {
        if (argc < 5) {
            PrintUsage(argv);
            return 42;
        }
        std::string alphabetFile = argv[2];
        std::string datasetFile = argv[3];
        std::string resultModelFile = argv[4];
        return Train(alphabetFile, datasetFile, resultModelFile);
    } else if (mode == "interactive") {
        if (argc < 3) {
            PrintUsage(argv);
            return 42;
        }
        std::string modelFile = argv[2];
        return Interactive(modelFile);
    }

    PrintUsage(argv);
    return 42;
}
