#include "jamspell/spell_corrector.hpp"
#include "contrib/httplib/httplib.h"

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

    std::cerr << "[info] starting web server at localhost:" << port << std::endl;
    srv.listen("localhost", port);
    return 0;
}
