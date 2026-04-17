#include "app/App.h"
#include "games/menu/MenuGame.h"
#include "platform/null/NullPlatform.h"
#include "ti_msp_dl_config.h"

int main(void) {
	SYSCFG_DL_init();

	handheld::NullPlatform platform;
	handheld::MenuGame menu;
	handheld::App app(platform, menu);
	app.run_forever();
}
