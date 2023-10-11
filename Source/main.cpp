#include "Application/application.h"

int main() {
	application App{};
	while (App.RunOneFrame()) {
	}
	return 0;
}
