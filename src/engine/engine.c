#include "engine.h"
#include "dos.h"
#include "keyb/keyb.h"
#include "types/types.h"
#include "video/video.h"
#include <string.h>

System engine;

/** ENGINE :: Gets DOS version and evaluates if it is below the minimum required
 */
static bool GetDosVersion(void) {
	word version;
	byte majorVersion;
	byte minorVersion;

	ScreenSetCursor(5, 6);
	version = _get_dos_version(0);
	majorVersion = (version >> 8) & 0xff;
	minorVersion = version & 0xff;

	//sleep(1);

	ScreenSetCursor(5, 20);
	printf(" REQ. %u.%u .....................................", DOS_MAJOR_VERSION, DOS_MINOR_VERSION);

	ScreenSetCursor(5, 67);
	printf("  %u.%u. \n", majorVersion, minorVersion);

	ScreenSetCursor(5, 6);
	if (majorVersion > DOS_MAJOR_VERSION) {
		printf("%u", 1);
		return true;
	} else if ((majorVersion == DOS_MAJOR_VERSION) & (minorVersion >= DOS_MINOR_VERSION)) {
		printf("%u", 1);
		return true;
	} else {
		printf("%u", 0);
		return false;
	}
}

/** ENINE :: Gets memory information and evaluates if it is enought RAM memory
 */
static bool GetMemInfo(void) {
	word totalMem;
	word availableMem;

	unsigned long dpmi_page_size;
	__dpmi_memory_info dpmi_mem;
	__dpmi_free_mem_info dpmi_free_mem;
	__dpmi_get_page_size(&dpmi_page_size);
	__dpmi_get_memory_information(&dpmi_mem);
	__dpmi_get_free_memory_information(&dpmi_free_mem);

	availableMem = (word) ((dpmi_free_mem.total_number_of_free_pages * dpmi_page_size) / 1024);
	totalMem = (word) ((dpmi_free_mem.total_number_of_physical_pages * dpmi_page_size) / 1024);

	//sleep(1);

	ScreenSetCursor(7, 19);
	printf(" REQ. %u kb ........... %u kb. free of %u kb. \n", MM_RAM_REQ, availableMem, totalMem);

	ScreenSetCursor(7, 6);
	if (availableMem >= MM_RAM_REQ) {
		printf("%u", 1);
		return true;

	} else {
		printf("%u", 0);
		ScreenSetCursor(7, 6);
		sleep(2);
		return false;
	}
}

/** ENGINE :: Gets video card information and evaluates its compatibility
 */
static bool GetAvailableVideo(void) {

	// Check VGA
	video.vga_present = VGA_CheckGraphicsCard();
	//sleep(1);
	if (video.vga_present) {
		ScreenSetCursor(9, 6);
		printf("%u", 1);
		ScreenSetCursor(9, 23);
		printf(" .......................................... Found. \n");
		return true;
	} else {
		ScreenSetCursor(9, 6);
		printf("%u", 0);
		ScreenSetCursor(9, 23);
		printf(" ...................................... Not found. \n");
		return false;
	}
}

/** ENGINE :: Checks if mouse is available
 */
static bool GetAvailableMouse(void) {
	mouse_present = MOUSE_CheckIfAvailable();
	//sleep(1);
	if (mouse_present) {
		ScreenSetCursor(13, 6);
		printf("%u", 1);
		ScreenSetCursor(13, 21);
		printf(" ........................................ Available. \n");
		return true;
	} else {
		ScreenSetCursor(13, 6);
		printf("%u", 0);
		ScreenSetCursor(13, 21);
		printf(" .................................... Not available. \n");
		return false;
	}
}

/** ENGINE :: Checks if there is any compatible sound card
 */
static bool GetAvailableAudio(void) {
	adlib_present = AUDIO_CheckAdlib();

	if (adlib_present) {
		ScreenSetCursor(14, 6);
		printf("%u", 1);
		ScreenSetCursor(14, 25);
		printf(" .................................... Available. \n");
	} else {
		ScreenSetCursor(14, 6);
		printf("%u", 0);
		ScreenSetCursor(14, 25);
		printf(" .................................Not available. \n");
	}

	sblaster_present = AUDIO_CheckSoundBlaster();

	if (sblaster_present) {
		ScreenSetCursor(15, 6);
		printf("%u", 1);
		ScreenSetCursor(15, 27);
		printf(" .................................. Available. \n");
	} else {
		ScreenSetCursor(15, 6);
		printf("%u", 0);
		ScreenSetCursor(15, 27);
		printf(" ...............................Not available. \n");
	}

	// PC Speaker is always here for you!
	return true;
}

/** ENGINE :: Awaits a key being pressed and released
 */
void AwaitInput(void) {
	while (ANY_KEY_PRESSED()) {
	}
	while (!ANY_KEY_PRESSED()) {
	}
}

/** ENGINE :: Shutdowns all systems
 */
static void ShutdownEngine(void) {

	VGA_SetMode(VIDEO_TEXT_MODE);
	VIDEO_Shutdown();
	MAP_Shutdown();
	GFX_Shutdown();
	MOUSE_Shutdown();
	AUDIO_Shutdown();
	MM_Shutdown();
	KEYB_Shutdown();
	TIMER_Shutdown();

	__djgpp_nearptr_disable();
}

/** ENGINE :: Exit to DOS
 */
void ExitDos(void) {

	// Shutdown all systems
	ShutdownEngine();

	// Show bye bye image
	VIDEO_BinaryImageToVRAM("BINARIES.DAT", "SHUTDOWN.BIN");// Show engine loading screen
	ScreenSetCursor(22, 1);
	exit(0);
}

/** ENGINE :: Error detected
 *  Prints error code and reason and calls the exit to dos function
 */
void Error(const char *quit_message1, const char *quit_message2, const char *quit_message3, int exit_code) {

	// Shutdown all systems
	ShutdownEngine();

	system("cls");
	switch (exit_code) {
		case ERROR_SYSTEM:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ERROR.BIN");// Show engine loading screen
			break;
		case ERROR_MEMORY:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "NOMEMORY.BIN");// Show engine loading screen
			break;
		case ERROR_VIDEO:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "NOVIDEO.BIN");// Show engine loading screen
			break;
		case ERROR_FILE:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ERROR.BIN");// Show engine loading screen
			break;
		case ERROR_SOUND:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ERROR.BIN");// Show engine loading screen
			break;
		case ERROR_GRAPHICS:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ERROR.BIN");// Show engine loading screen
			break;
		case ERROR_MOUSE:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ERROR.BIN");// Show engine loading screen
			break;
		case ERROR_BINARY_FILE:
			//GFX_ShowTextScreen("BIN\\ERROR.BIN");// Show engine loading screen
			break;
		default:
			VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ERROR.BIN");// Show engine loading screen
			//
			break;
	}

	ScreenSetCursor(2, 3);
	printf("[ERROR] :: Code %u", exit_code);
	ScreenSetCursor(3, 3);
	printf("[MSG1] :: %s", quit_message1);
	ScreenSetCursor(4, 3);
	printf("[MSG2] :: %s", quit_message2);
	ScreenSetCursor(5, 3);
	printf("[MSG3] :: %s", quit_message3);
	sleep(100);
	ScreenSetCursor(22, 1);
	sleep(100);
	exit(exit_code);
}

/* ENGINE :: Sets asyncronous delay time
 */
void SetDelayTime(int time) {
	engine.delay_time_ms = time;
}

// ENGINE :: Waits delay time is elapsed
bool AwaitDelayTime(void) {
	if (engine.delay_time_ms > 0) return false;
	return true;
}

/* ENGINE :: Initialize all systems
 * - Momory manager
 * - Mouse initialization
 * - Memory reservation
 */
static void InitSubsystems(void) {

	system("cls");

	MM_Init();   // Initialize memory manager
	MOUSE_Init();// Install custom mouse interrupt handler
	GFX_Init();  // Initialize graphics
	AUDIO_Init();// Initialize audio
	MAP_Init();  // Initialize map
	VIDEO_Init();// Initialize video
	CAM_Init();  // Initialize camera
	VGA_ClearPalette();
	VGA_SetMode(VIDEO_GRAPHICS_MODE);  // Set graphics mode
	MOUSE_RestrictZone(0, 320, 0, 199);// Restrict mouse zone

	// Set default keys
	kbBindingUp = SCANCODE_W;
	kbBindingDown = SCANCODE_S;
	kbBindingLeft = SCANCODE_A;
	kbBindingRight = SCANCODE_D;
	kbBindingJump = SCANCODE_SPACE;
	kbBindingFire = SCANCODE_0;
	kbBindingCombat = SCANCODE_C;
}

/* ENGINE :: Initialize engine
 * - Set evironment variables
 * - All system initialization
 */
void InitEngine(void) {
	bool dosVersionOK;
	bool memOK;
	bool videoOK;
	bool audioOK;
	bool mouseOK;

	system("cls");

	engine.fps_old_time = 0;
	engine.loading = false;

	VGA_HideCursor();// Hide cursor

	// Allow access to conventional mem
	if (__djgpp_nearptr_enable() == 0) {
		sprintf(engine.system_error_message1, "InitEngine function error");
		sprintf(engine.system_error_message2, "Impossible enable direct acess to real mem");
		sprintf(engine.system_error_message3, "_djgpp_nearptr_enable returns 0");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_SYSTEM);
	}

	// Set video pointers
	VGA = (byte *) (__djgpp_conventional_base + 0xA0000);
	EGA = (byte *) (__djgpp_conventional_base + 0xB8000);

	TIMER_Init();// Initialize timer interrupt
	KEYB_Init(); // Install custom keyboard interrupt handler

	// Load engine initialization screen
	VIDEO_BinaryImageToVRAM("BINARIES.DAT", "ENGINE.BIN");// Show engine loading screen

	ScreenSetCursor(21, 25);
	printf(" ...please wait...");

	dosVersionOK = GetDosVersion();// Get and check DOS version
	memOK = GetMemInfo();          // Get free memory
	videoOK = GetAvailableVideo(); // Check vga/ega/cga video available
	mouseOK = GetAvailableMouse(); // Check if mouse is available
	audioOK = GetAvailableAudio(); // Check adlib/soundblaster audio available

	ScreenSetCursor(21, 25);
	SetDelayTime(1000);
	while (!AwaitDelayTime()) {
		// just wait
	};
	printf(" ...process done...");

	if (!dosVersionOK) {
		sprintf(engine.system_error_message1, "DOS version not supported");
		sprintf(engine.system_error_message2, " ");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_SYSTEM);
	}

	if (!memOK) {
		sprintf(engine.system_error_message1, "Not enought RAM memory");
		sprintf(engine.system_error_message2, " ");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_MEMORY);
	}

	if (!videoOK) {
		sprintf(engine.system_error_message1, "Video device not supported");
		sprintf(engine.system_error_message2, " ");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
	}

	if (!audioOK) {
		sprintf(engine.system_error_message1, "Audio device not supported");
		sprintf(engine.system_error_message2, " ");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_SOUND);
	}

	if (!mouseOK) {
		sprintf(engine.system_error_message1, "Mouse not available");
		sprintf(engine.system_error_message2, " ");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_MOUSE);
	}

	// Initialize everything else
	InitSubsystems();
}

void LimitFPS(int time) {
	dword limit_new_time;

	// Just for the first time
	if (engine.fps_old_time == 0) {
		engine.limit_old_time = TIMER_GetMilliseconds();
	}

	limit_new_time = TIMER_GetMilliseconds();

	while ((limit_new_time - engine.limit_old_time) < time) {
		limit_new_time = TIMER_GetMilliseconds();
	}
	engine.limit_old_time = limit_new_time;
}

void FPS(void) {
	dword fps_new_time;

	// Increase frame counter
	engine.frame_counter++;

	// Just for the first time
	if (engine.fps_old_time == 0) {
		engine.fps_old_time = TIMER_GetMilliseconds();
	}

	fps_new_time = TIMER_GetMilliseconds();

	//One second elpsed
	if (fps_new_time - engine.fps_old_time >= 1000) {
		engine.fps = engine.frame_counter;// Update frames per second
		engine.frame_counter = 0;         // Reset frame counter
		engine.fps_old_time = fps_new_time;
	}
}

void UpdateStatusPannel(void) {
	char int_string[6];
	char dword_string[10];

	sprintf(dword_string, "%08ld\n", engine.sample_time);
	VIDEO_StringToScreenBuffer(8, 0, dword_string, FONT_SLIM_WHITE);
	sprintf(int_string, "%04d\n", engine.fps);
	VIDEO_StringToScreenBuffer(8, 8, int_string, FONT_SLIM_WHITE);

	sprintf(int_string, "%04d\n", engine.debug1_INT);
	VIDEO_StringToScreenBuffer(8, 16, int_string, FONT_SLIM_WHITE);
	sprintf(int_string, "%04d\n", engine.debug2_INT);
	VIDEO_StringToScreenBuffer(8, 24, int_string, FONT_SLIM_WHITE);
	sprintf(int_string, "%04d\n", engine.debug3_INT);
	VIDEO_StringToScreenBuffer(8, 32, int_string, FONT_SLIM_WHITE);
	sprintf(int_string, "%04d\n", engine.debug4_INT);
	VIDEO_StringToScreenBuffer(8, 40, int_string, FONT_SLIM_WHITE);
}

void Update(int player_follow) {
	dword start_time;

	////// MAIN VGA DRAWING FUNCTION //////
	VIDEO_VSync();// Wait Vsync

	start_time = TIMER_GetMilliseconds();

	// Draws screen buffer to video ram
	VIDEO_ScreenBufferToVRAM();

	FPS();                                                                       // Calculate fps
	if (!ui.freeze) CAM_MainPositionControl(player_follow, &map.update_required);// Place new cam position
	MAP_Update();
	VIDEO_MapBufferToScreenBuffer();// Fills a second buffer with the area shown

	// Update
	MOUSE_Update(actor.mode_combat);
	if (!ui.freeze) ACTOR_Update();
	if (!ui.freeze) OBJECT_UpdateObjects();
	if (!ui.freeze) EFFECT_UpdateEffects();
	if (!ui.freeze) PARTICLE_UpdateParticles();
	if (!ui.freeze) ENEMY_Update();

	// Draw all sprites and effects
	if (!ui.freeze) GFX_UpdateSprites();
	GFX_DrawSprites();

	UI_UpdateUI(actor.mode_combat);
	UpdateStatusPannel();// Draws status pannel on the screen buffer

	LimitFPS(28);
	engine.sample_time = TIMER_GetMilliseconds() - start_time;

	//ENEMY_DrawHitPixels(enemy[0]);
	//ENEMY_DrawColissionPixels(enemy[1]);
	//ENEMY_DrawColissionPixels(enemy[2]);

	//ACTOR_DrawColissionPixels();
	//ACTOR_DrawHitPixels();


	// Update map with the new cam position


	////////// GRAPHICS UPDATE ////////////
	//GFX_AsyncRotatePalette();
	//GFX_AsyncFadeInFromPalette();
	//GFX_AsyncFadeOutFromPalette();

	// Update video


	//if (waitCyclesTarget > 0) { waitCyclesCounter++; }
}