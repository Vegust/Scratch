#include "Application/application.h"

int main() {
	application App{1000,800};
	while (App.RunOneFrame()) {
	}
	return 0;
}
