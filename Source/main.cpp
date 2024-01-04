#include "Application/application.h"

int main() {
	application_settings Settings;
	application App{Settings};
	while (App.RunOneFrame()) {
	}
	return 0;
}
