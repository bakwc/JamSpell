#include <iostream>

#include <jamspell/lang_model.hpp>
#include <jamspell/spell_corrector.hpp>

using namespace NJamSpell;

void PrintUsage(const char** argv) {
    std::cerr << "Usage: " << argv[0] << " mode args" << std::endl;
    std::cerr << "    train alphabet.txt dataset.txt resultModel.bin  - train model" << std::endl;
    std::cerr << "    train_ngrams alphabet.txt grams1.csv grams2.csv grams3.csv dataset.txt resultModel.bin"
                 "  - load model from 1-, 2-, and 3-grams and then train it on dataset.txt" << std::endl;
    std::cerr << "    score model.bin - input sentences and get score" << std::endl;
    std::cerr << "    correct model.bin - input sentences and get corrected one" << std::endl;
    std::cerr << "    fix model.bin input.txt output.txt - automatically fix txt file" << std::endl;
}

int Train(const std::string& alphabetFile,
          const std::string& datasetFile,
          const std::string& resultModelFile)
{
    TLangModel model;
    model.Train(datasetFile, alphabetFile);
    model.Dump(resultModelFile);
    return 0;
}

int TrainNGrams(const std::string& alphabetFile,
                const std::string& gram1File,
                const std::string& gram2File,
                const std::string& gram3File,
                const std::string& datasetFile,
          const std::string& resultModelFile)
{
    TLangModel model;
    model.TrainNGrams(gram1File, gram2File, gram3File, datasetFile, alphabetFile);
    model.Dump(resultModelFile);
    return 0;
}

int Score(const std::string& modelFile) {
    TLangModel model;
    std::cerr << "[info] loading model" << std::endl;
    if (!model.Load(modelFile)) {
        std::cerr << "[error] failed to load model" << std::endl;
        return 42;
    }
    std::cerr << "[info] loaded" << std::endl;
    std::cerr << ">> ";
    for (std::string line; std::getline(std::cin, line);) {
        std::wstring wtext = UTF8ToWide(line);
        std::cerr << model.Score(wtext) << "\n";
        std::cerr << ">> ";
    }
    return 0;
}

int Fix(const std::string& modelFile,
        const std::string& inputFile,
        const std::string& outFile)
{
    TSpellCorrector corrector;
    std::cerr << "[info] loading model" << std::endl;
    if (!corrector.LoadLangModel(modelFile)) {
        std::cerr << "[error] failed to load model" << std::endl;
        return 42;
    }
    std::cerr << "[info] loaded" << std::endl;
    std::wstring text = UTF8ToWide(LoadFile(inputFile));
    uint64_t startTime = GetCurrentTimeMs();
    std::wstring result = corrector.FixFragment(text);
    uint64_t finishTime = GetCurrentTimeMs();
    SaveFile(outFile, WideToUTF8(result));
    std::cerr << "[info] process time: " << finishTime - startTime << "ms" << std::endl;
    return 0;
}

int Correct(const std::string& modelFile) {
    TSpellCorrector corrector;
    std::cerr << "[info] loading model" << std::endl;
    if (!corrector.LoadLangModel(modelFile)) {
        std::cerr << "[error] failed to load model" << std::endl;
        return 42;
    }
    std::cerr << "[info] loaded" << std::endl;
    std::cerr << ">> ";
    for (std::string line; std::getline(std::cin, line);) {
        std::wstring wtext = UTF8ToWide(line);
        std::wstring result = corrector.FixFragment(wtext);
        std::cerr << WideToUTF8(result) << "\n";
        std::cerr << ">> ";
    }
    return 0;
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
    } else if (mode == "train_ngrams") {
        if (argc != 8) {
            PrintUsage(argv);
            return 42;
        }
        std::string alphabetFile = argv[2];
        std::string gram1File = argv[3];
        std::string gram2File = argv[4];
        std::string gram3File = argv[5];
        std::string datasetFile = argv[6];
        std::string resultModelFile = argv[7];
        return TrainNGrams(alphabetFile, gram1File, gram2File, gram3File, datasetFile, resultModelFile);
    } else if (mode == "score") {
        if (argc < 3) {
            PrintUsage(argv);
            return 42;
        }
        std::string modelFile = argv[2];
        return Score(modelFile);
    } else if (mode == "correct") {
        if (argc < 3) {
            PrintUsage(argv);
            return 42;
        }
        std::string modelFile = argv[2];
        return Correct(modelFile);
    } else if (mode == "fix") {
        if (argc < 5) {
            PrintUsage(argv);
            return 42;
        }
        std::string modelFile = argv[2];
        std::string inFile = argv[3];
        std::string outFile = argv[4];
        return Fix(modelFile, inFile, outFile);
    }

    PrintUsage(argv);
    return 42;
}
