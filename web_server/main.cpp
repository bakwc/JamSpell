#include "jamspell/spell_corrector.hpp"
#include "contrib/httplib/httplib.h"
#include "contrib/nlohmann/json.hpp"
#include <cwctype>

std::string GetCandidates(const NJamSpell::TSpellCorrector& corrector,
                          const std::string& text)
{
    std::wstring input = NJamSpell::UTF8ToWide(text);
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

    return results.dump(4);
}

std::string FixText(const NJamSpell::TSpellCorrector& corrector,
                    const std::string& text)
{
    std::wstring input = NJamSpell::UTF8ToWide(text);
    return NJamSpell::WideToUTF8(corrector.FixFragment(input));
}

int main(int argc, const char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " model.bin localhost 8080\n";
        return 42;
    }

    std::string modelFile = argv[1];
    std::string hostname = argv[2];
    int port = std::stoi(argv[3]);

    NJamSpell::TSpellCorrector corrector;
    std::cerr << "[info] loading model" << std::endl;
    if (!corrector.LoadLangModel(modelFile)) {
        std::cerr << "[error] failed to load model" << std::endl;
        return 42;
    }

    httplib::Server srv;
    srv.Get("/fix", [&corrector](const httplib::Request& req, httplib::Response& resp) {
        resp.set_content(FixText(corrector, req.get_param_value("text")) + "\n", "text/plain");
    });

    srv.Post("/fix", [&corrector](const httplib::Request& req, httplib::Response& resp) {
        resp.set_content(FixText(corrector, req.body) + "\n", "text/plain");
    });

    srv.Get("/candidates", [&corrector](const httplib::Request& req, httplib::Response& resp) {
        resp.set_content(GetCandidates(corrector, req.get_param_value("text")) + "\n", "text/plain");
    });

    srv.Post("/candidates", [&corrector](const httplib::Request& req, httplib::Response& resp) {
        resp.set_content(GetCandidates(corrector, req.body) + "\n", "text/plain");
    });

    std::cerr << "[info] starting web server at " << hostname << ":" << port << std::endl;
    srv.listen(hostname.c_str(), port);
    return 0;
}
