//#########################
//Pekka Kana 2
//by Janne Kivilahti from Piste Gamez (2003)
//#########################
#include "screens/screens.hpp"

#include "engine/PDraw.hpp"
#include "engine/PInput.hpp"
#include "engine/PSound.hpp"
#include "engine/PUtils.hpp"

#include <cstring>

#include "game/game.hpp"
#include "settings.hpp"
#include "gfx/text.hpp"
#include "language.hpp"
#include "gui.hpp"
#include "episode.hpp"
#include "sfx.hpp"
#include "save.hpp"
#include "system.hpp"

bool going_to_game = false;

int PK_Draw_Map_Button(int x, int y, int type){

	const int BORDER = 17;
	int ret = 0;
	
	if (mouse_x > x && mouse_x < x+ BORDER && mouse_y > y && mouse_y < y + BORDER){
		if (key_delay == 0 && (PisteInput_Hiiri_Vasen() || PisteInput_Keydown(PI_SPACE)
													    || PisteInput_Ohjain_Nappi(PI_PELIOHJAIN_1,PI_OHJAIN_NAPPI_1))){
			key_delay = 30;
			return 2;
		}

		if (type == 0)
			PDraw::image_cutcliptransparent(game_assets, 247, 1, 25, 25, x-4, y-4, 60, 32);
		if (type == 1)
			PDraw::image_cutcliptransparent(game_assets, 247, 1, 25, 25, x-2, y-2, 60, 96);
		if (type == 2)
			PDraw::image_cutcliptransparent(game_assets, 247, 1, 25, 25, x-4, y-4, 60, 64);

		ret = 1;
	}

	int flash = 50 + (int)(sin_table[degree%360]*2);

	if (flash < 0)
		flash = 0;
	
	if (type == 1)
		PDraw::image_cutcliptransparent(game_assets, 247, 1, 25, 25, x-2, y-2, flash, 96);

	if (((degree/45)+1)%4==0 || type==0)
		PDraw::image_cutclip(game_assets,x,y,1 + 25*type,58,23 + 25*type,80);

	return ret;
}

int PK_Draw_Map(){
	char luku[20];
	int vali = 20;

	PDraw::screen_fill(0);
	PDraw::image_clip(bg_screen, 0, 0);

	PDraw::font_write(fontti4,Episode->name,100+2,72+2);
	PDraw::font_write(fontti2,Episode->name,100,72);

	PDraw::font_write(fontti4,tekstit->Hae_Teksti(PK_txt.map_total_score),100+2,92+2);
	vali = PDraw::font_write(fontti2,tekstit->Hae_Teksti(PK_txt.map_total_score),100,92);//250,80
	ltoa(Episode->player_score,luku,10);
	PDraw::font_write(fontti4,luku,100+vali+2+15,92+2);
	PDraw::font_write(fontti2,luku,100+vali+15,92);

	if (Episode->scores.episode_top_score > 0) {
		vali = PDraw::font_write(fontti1,tekstit->Hae_Teksti(PK_txt.map_episode_best_player),360,72);
		PDraw::font_write(fontti1,Episode->scores.episode_top_player,360+vali+10,72);
		vali = PDraw::font_write(fontti1,tekstit->Hae_Teksti(PK_txt.map_episode_hiscore),360,92);
		ltoa(Episode->scores.episode_top_score,luku,10);
		PDraw::font_write(fontti2,luku,360+vali+15,92);
	}

	vali = PDraw::font_write(fontti1,tekstit->Hae_Teksti(PK_txt.map_next_level),100,120);
	ltoa(Episode->level, luku, 10);
	PDraw::font_write(fontti1,luku,100+vali+15,120);

	//PK_Particles_Draw();

	if (Episode->level_count == 0) {
		PDraw::font_write(fontti2,tekstit->Hae_Teksti(PK_txt.episodes_no_maps),180,290);
	}

	if (Draw_Menu_Text(true,tekstit->Hae_Teksti(PK_txt.mainmenu_return),100,430)){
		next_screen = SCREEN_MENU;
		degree_temp = degree;
	}

	int nuppi_x = 0, nuppi_y = 0;
	int tyyppi = 0;
	int paluu;
	int min = 0, sek = 0;
	int icon;
	int sinx = 0, cosy = 0;
	int pekkaframe = 0;

	int njakso = Episode->level_count;
	for (int i = 0; i < Episode->level_count; i++)
		if (!Episode->levels_list[i].cleared && Episode->levels_list[i].order < njakso)
			njakso = Episode->levels_list[i].order; // Find the first unclear level
	
	if(Episode->level < njakso)
		Episode->level = njakso;

	for (int i = 0; i < Episode->level_count; i++) {
		if (strcmp(Episode->levels_list[i].nimi,"")!=0 && Episode->levels_list[i].order > 0) {
			tyyppi = 0;							//0 harmaa
			if (Episode->levels_list[i].order == Episode->level)
				tyyppi = 1;						//1 vihre�
			if (Episode->levels_list[i].order > Episode->level)
				tyyppi = 2;						//2 oranssi
			if (Episode->levels_list[i].cleared)
				tyyppi = 0;

			if (Episode->levels_list[i].x == 0)
				Episode->levels_list[i].x = 172+i*30;

			if (Episode->levels_list[i].y == 0)
				Episode->levels_list[i].y = 270;

			icon = Episode->levels_list[i].icon;

			//PDraw::image_clip(game_assets,Episode->levels_list[i].x-4,Episode->levels_list[i].y-4-30,1+(icon*27),452,27+(icon*27),478);
			PDraw::image_cutclip(game_assets,Episode->levels_list[i].x-9,Episode->levels_list[i].y-14,1+(icon*28),452,28+(icon*28),479);

			if (tyyppi==1) {
				sinx = (int)(sin_table[degree%360]/2);
				cosy = (int)(cos_table[degree%360]/2);
				pekkaframe = 28*((degree%360)/120);
				PDraw::image_cutclip(game_assets,Episode->levels_list[i].x+sinx-12,Episode->levels_list[i].y-17+cosy,157+pekkaframe,46,181+pekkaframe,79);
			}

			paluu = PK_Draw_Map_Button(Episode->levels_list[i].x-5, Episode->levels_list[i].y-10, tyyppi);

			// if clicked
			if (paluu == 2) {
				if (tyyppi != 2 || dev_mode) {

					Game = new GameClass(i);
					
					going_to_game = true;
					PDraw::fade_out(PDraw::FADE_SLOW);
					PSound::set_musicvolume(0);
					Play_MenuSFX(doodle_sound,90);
				}
				else
					Play_MenuSFX(moo_sound,100);
			}

			itoa(Episode->levels_list[i].order,luku,10);
			PDraw::font_write(fontti1,luku,Episode->levels_list[i].x-12+2,Episode->levels_list[i].y-29+2);

			// if mouse hoover
			if (paluu > 0) {

				int info_x = 489+3, info_y = 341-26;

				PDraw::image_cutclip(game_assets,info_x-3,info_y+26,473,0,607,121);
				PDraw::font_write(fontti1,Episode->levels_list[i].nimi,info_x,info_y+30);

				if (Episode->scores.best_score[i] > 0) {
					PDraw::font_writealpha(fontti1,tekstit->Hae_Teksti(PK_txt.map_level_best_player),info_x,info_y+50,75);
					PDraw::font_write(fontti1,Episode->scores.top_player[i],info_x,info_y+62);
					vali = 8 + PDraw::font_writealpha(fontti1,tekstit->Hae_Teksti(PK_txt.map_level_hiscore),info_x,info_y+74,75);
					ltoa(Episode->scores.best_score[i],luku,10);
					PDraw::font_write(fontti1,luku,info_x+vali,info_y+75);
				}

				if (Episode->scores.best_time[i] > 0) {
					PDraw::font_writealpha(fontti1,tekstit->Hae_Teksti(PK_txt.map_level_fastest_player),info_x,info_y+98,75);
					PDraw::font_write(fontti1,Episode->scores.fastest_player[i],info_x,info_y+110);

					vali = 8 + PDraw::font_writealpha(fontti1,tekstit->Hae_Teksti(PK_txt.map_level_best_time),info_x,info_y+122,75);

					int time = (int)Episode->scores.best_time[i];
					if (time < 0) {
						time = -time;
						vali += PDraw::font_write(fontti1,"-",info_x+vali,info_y+122);
					}

					min = time / 60;
					sek = time % 60;

					itoa(min,luku,10);
					vali += PDraw::font_write(fontti1,luku,info_x+vali,info_y+122);
					vali += PDraw::font_write(fontti1,":",info_x+vali,info_y+122);
					itoa(sek,luku,10);
					PDraw::font_write(fontti1,luku,info_x+vali,info_y+122);
				}
			}
		}
	}

	PK_Draw_Cursor(mouse_x, mouse_y);

	return 0;
}

int Play_Music() {
	char mapmusa[PE_PATH_SIZE];
	
	strcpy(mapmusa, "map.mp3");
	Episode->Get_Dir(mapmusa);
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "map.ogg");
	Episode->Get_Dir(mapmusa);
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "map.xm");
	Episode->Get_Dir(mapmusa);
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "map.mod");
	Episode->Get_Dir(mapmusa);
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "map.it");
	Episode->Get_Dir(mapmusa);
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "map.s3m");
	Episode->Get_Dir(mapmusa);
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "music/map.mp3");
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "music/map.ogg");
	if (PisteUtils_Find(mapmusa))
		goto found;
	
	strcpy(mapmusa, "music/map.xm");
	
	found:

	PSound::start_music(mapmusa);
	PSound::set_musicvolume_now(Settings.music_max_volume);
}

int Screen_Map_Init() {

	if (!Episode) {
		PK2_Error("PK2    - ERROR - Episode not started\n");
		return 1;
	}

	GUI_Change(UI_CURSOR);
	if (Settings.isWide)
		PDraw::set_xoffset(80);
	else
		PDraw::set_xoffset(0);
	PDraw::screen_fill(0);

	degree = degree_temp;
	
	char mapkuva[PE_PATH_SIZE] = "map.bmp";
	Episode->Get_Dir(mapkuva);

	PDraw::image_load(bg_screen, mapkuva, true);
	if (bg_screen == -1)
		PDraw::image_load(bg_screen, "gfx/map.bmp", true);

	Play_Music();

	going_to_game = false;

	PDraw::fade_in(PDraw::FADE_SLOW);

	return 0;
}

int Screen_Map(){
	PK_Draw_Map();

	degree = 1 + degree % 360;

	if (going_to_game && !PDraw::is_fading()) {
		next_screen = SCREEN_GAME;
		
		//Draw "loading" text
		PDraw::set_xoffset(0);
		PDraw::screen_fill(0);
		PDraw::font_write(fontti2, tekstit->Hae_Teksti(PK_txt.game_loading), screen_width / 2 - 82, screen_height / 2 - 9);
		PDraw::fade_out(0);
	}

	if (!going_to_game && key_delay == 0){
		if (PisteInput_Keydown(PI_ESCAPE)) {
			next_screen = SCREEN_MENU;
			degree_temp = degree;
			key_delay = 20;
		}
	}

	return 0;
}