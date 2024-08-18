#include<string> 
struct ServerUttilitys
{
public:
	static std::string FloatToString(float val) {    return std::to_string(val); }
    static float StringToFloat(const std::string& txt) { return std::stof(txt); }
	static char* StringToCharArray(const std::string& txt)
    {
        char* charArray = new char[txt.length() + 1];
        std::copy(txt.c_str(), txt.c_str() + txt.length() + 1, charArray);
        return charArray;
    }

};



