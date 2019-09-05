#include <iostream>
#include <regex>
#include <fstream>
int main()
{
    std::ifstream css_file("config/UI/design.css");
    std::string css_string((std::istreambuf_iterator<char>(css_file)), std::istreambuf_iterator<char>());

    std::ifstream html_file("config/UI/index.html");
    std::string html_string((std::istreambuf_iterator<char>(html_file)), std::istreambuf_iterator<char>());
    //std::string text("Hello World $CSS hfuweregepge gew");
    //std::regex e("\\$CSS");
    //std::string replace_with("WORLD !\nfrwfrve\n#1\.divvv\n");
    std::string hello = "hello World $asd";
    std::cout << "Content: " << hello << std::endl;
    hello = std::regex_replace(hello, std::regex("\\$asd"), std::string("hi")); 
    std::cout << "Hello: " << hello << std::endl;

    return 0;
}

