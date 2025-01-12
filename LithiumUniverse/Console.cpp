#include <iostream>
#include <string>

void Print(const char* message) {
	std::cout << message << std::endl;
}
void Print(std::string message) {
	Print(message.c_str());
}
void Print(double message) {
	Print(std::to_string(message));
}
void Print(bool message) {
	Print(message?"true" : "false");
}