
#include <iostream>
#include <string>
// #include <cpprest/http_client.h>
// #include <cpprest/json.h>

// using namespace web;
// using namespace web::http;
// using namespace web::http::client;

// // Function to translate text using Microsoft Translator API
// std::string TranslateText(const std::string& text, const std::string& target_lang, const std::string& api_key) {
//     utility::string_t url = U("https://api.cognitive.microsofttranslator.com/translate?api-version=3.0&to=") + utility::conversions::to_string_t(target_lang);
//     http_client client(url);

//     // Prepare request headers
//     http_request request(methods::POST);
//     request.headers().add(U("Ocp-Apim-Subscription-Key"), utility::conversions::to_string_t(api_key));
//     request.headers().add(U("Content-Type"), U("application/json"));

//     // Prepare request body
//     json::value body;
//     body[U("text")] = json::value::string(utility::conversions::to_string_t(text));
//     request.set_body(body);

//     // Send request
//     http_response response = client.request(request).get();

//     // Read response
//     json::value json_response = response.extract_json().get();
//     return utility::conversions::to_utf8string(json_response[0][U("translations")][0][U("text")].as_string());
// }

int main() {
    // std::string text_to_translate = "Hello, how are you?";
    // std::string target_language = "fr"; // French
    // std::string api_key = "YOUR_API_KEY"; // Replace with your actual API key

    // std::string translated_text = TranslateText(text_to_translate, target_language, api_key);
    // std::cout << "Translated text: " << translated_text << std::endl;

    std::string s = ":copper.libera.chat 353 lalala = #c :lalala nero1086 lentement varioust Fingel ntwk harrykar X-Scale SystemError Brainium gnomesort pragma- divine byteskeptical kentucky44448 seninha Leonarbro bitbinge oneeyedalien_ Thedarkb Inline wbooze korg815 kitsunenokenja jjakob Feuermagier hammond_ Coop Cindy DPA famine__ ic56 sumpwa mountaingoat uso jargon syscall-42 neutroniak nostar emanuele6 dustinm gokkun Biolunar raym foul_owl mechap elastic_dog phongjaa AmR mima gr33n7007h Everything rtypo asarandi AbleBacon";

    std::cout << s.size() << std::endl;
    return 0;
}

