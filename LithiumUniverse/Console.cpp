#include <iostream>
#include <string>

void Print(const char* message) {
	std::cout << message << std::endl;
}
void Print(std::string message) {
	Print(message.c_str());
}