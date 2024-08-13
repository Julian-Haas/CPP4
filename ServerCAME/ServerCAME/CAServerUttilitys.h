#include<string> 
struct ServerUttilitys
{
public:
	static std::string FloatToString(float val);
	static float StringToFloat(const std::string& txt);
	static char* StringToCharArray(const std::string& txt);

};