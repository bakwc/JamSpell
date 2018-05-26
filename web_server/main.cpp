#include "jamspell/spell_corrector.hpp"
#include "contrib/httplib/httplib.h"
#include "contrib/nlohmann/json.hpp"


int main(int argc, const char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " model.bin 8080\n";
        return 42;
    }

    std::string modelFile = argv[1];
    int port = std::stoi(argv[2]);

    NJamSpell::TSpellCorrector corrector;
    std::cerr << "[info] loading model" << std::endl;
    if (!corrector.LoadLangModel(modelFile)) {
        std::cerr << "[error] failed to load model" << std::endl;
        return 42;
    }

    httplib::Server srv;
    srv.Get("/fix", [&corrector](const httplib::Request& req, httplib::Response& resp) {
        std::wstring input = NJamSpell::UTF8ToWide(req.get_param_value("text"));
        std::string result = NJamSpell::WideToUTF8(corrector.FixFragment(input));
        resp.set_content(result, "text/plain");
    });

    srv.Get("/fix_candidates", [&corrector](const httplib::Request& req, httplib::Response& resp) {
        std::wstring input = NJamSpell::UTF8ToWide(req.get_param_value("text"));
        std::transform(input.begin(), input.end(), input.begin(), std::towlower);
        NJamSpell::TSentences sentences = corrector.GetLangModel().Tokenize(input);

        nlohmann::json results;
        results["results"] = nlohmann::json::array();

        for (size_t i = 0; i < sentences.size(); ++i) {
            const NJamSpell::TWords& sentence = sentences[i];
            for (size_t j = 0; j < sentence.size(); ++j) {
                NJamSpell::TWord currWord = sentence[j];
                std::wstring wCurrWord(currWord.Ptr, currWord.Len);
                NJamSpell::TWords candidates = corrector.GetCandidatesRaw(sentence, j);
                if (candidates.empty()) {
                    continue;
                }
                std::wstring firstCandidate(candidates[0].Ptr, candidates[0].Len);
                if (wCurrWord == firstCandidate) {
                    continue;
                }
                nlohmann::json currentResult;
                currentResult["pos_from"] = currWord.Ptr - &input[0];
                currentResult["len"] = currWord.Len;
                currentResult["candidates"] = nlohmann::json::array();

                size_t candidatesSize = std::min(candidates.size(), size_t(7));
                for (size_t k = 0; k < candidatesSize; ++k) {
                    NJamSpell::TWord candidate = candidates[k];
                    std::string candidateStr = NJamSpell::WideToUTF8(std::wstring(candidate.Ptr, candidate.Len));
                    currentResult["candidates"].push_back(candidateStr);
                }

                results["results"].push_back(currentResult);
            }
        }

        resp.set_content(results.dump(4), "text/plain");
    });

    std::cerr << "[info] starting web server at localhost:" << port << std::endl;
    srv.listen("localhost", port);
    return 0;
}
