//#########################
//Pekka Kana 2
//Copyright (c) 2003 Janne Kivilahti
//#########################
#include "game/physics.hpp"

#include "game/game.hpp"
#include "game/sprites.hpp"
#include "game/blocks.hpp"
#include "game/gifts.hpp"
#include "settings.hpp"
#include "gfx/particles.hpp"
#include "gfx/effect.hpp"
#include "gfx/text.hpp"
#include "language.hpp"
#include "sfx.hpp"
#include "system.hpp"
#include "episode/episodeclass.hpp"
#include "gui.hpp"

#include "engine/types.hpp"
#include "engine/PInput.hpp"
#include "engine/PSound.hpp"
#include "engine/PDraw.hpp"

#include <cstring>

double sprite_x;
double sprite_y;
double sprite_a;
double sprite_b;

double sprite_vasen;
double sprite_oikea;
double sprite_yla;
double sprite_ala;

int sprite_leveys;
int sprite_korkeus;

int map_vasen;
int map_yla;
int x = 0;
int y = 0;

bool oikealle;
bool vasemmalle;
bool ylos;
bool alas;

bool vedessa;

u8 max_nopeus;
void Check_Blocks2(SpriteClass &sprite, PK2BLOCK &palikka) {

	//left and right
	if (sprite_yla < palikka.ala && sprite_ala-1 > palikka.yla){
		if (sprite_oikea+sprite_a-1 > palikka.vasen && sprite_vasen+sprite_a < palikka.oikea){
			// Tutkitaan onko sprite menossa oikeanpuoleisen palikan sis��n.
			if (sprite_oikea+sprite_a < palikka.oikea){
				// Onko palikka sein�?
				if (palikka.oikealle == BLOCK_WALL){
					oikealle = false;
					if (palikka.koodi == BLOCK_HISSI_HORI)
						sprite_x = palikka.vasen - sprite_leveys/2;
				}
			}

			if (sprite_vasen+sprite_a > palikka.vasen){
				if (palikka.vasemmalle == BLOCK_WALL){
					vasemmalle = false;

					if (palikka.koodi == BLOCK_HISSI_HORI)
						sprite_x = palikka.oikea + sprite_leveys/2;

				}
			}
		}
	}

	sprite_vasen = sprite_x-sprite_leveys/2;
	sprite_oikea = sprite_x+sprite_leveys/2;

	//ceil and floor

	if (sprite_vasen < palikka.oikea && sprite_oikea-1 > palikka.vasen){
		if (sprite_ala+sprite_b-1 >= palikka.yla && sprite_yla+sprite_b <= palikka.ala){
			if (sprite_ala+sprite_b-1 <= palikka.ala){
				if (palikka.alas == BLOCK_WALL){
					alas = false;

					if (palikka.koodi == BLOCK_HISSI_VERT)
						sprite_y = palikka.yla - sprite_korkeus /2;

					if (sprite_ala-1 >= palikka.yla && sprite_b >= 0)
						if (palikka.koodi != BLOCK_HISSI_HORI)
							sprite_y = palikka.yla - sprite_korkeus /2;
				}
			}

			if (sprite_yla+sprite_b > palikka.yla){
				if (palikka.ylos == BLOCK_WALL){
					ylos = false;

					if (sprite_yla < palikka.ala)
						if (palikka.koodi != BLOCK_HISSI_HORI)
							sprite.kyykky = true;
				}
			}
		}
	}
}

void Check_Blocks(SpriteClass &sprite, PK2BLOCK &palikka) {
	int mask_index;

	//If sprite is in the block
	if (sprite_x <= palikka.oikea && sprite_x >= palikka.vasen && sprite_y <= palikka.ala && sprite_y >= palikka.yla){

		/**********************************************************************/
		/* Examine if block is water background                               */
		/**********************************************************************/
		if (palikka.water)
			sprite.vedessa = true;
		else
			sprite.vedessa = false;

		/**********************************************************************/
		/* Examine if it touches the fire                                     */
		/**********************************************************************/
		if (palikka.koodi == BLOCK_TULI && Game->button1 == 0 && sprite.isku == 0){
			sprite.saatu_vahinko = 2;
			sprite.saatu_vahinko_tyyppi = DAMAGE_FIRE;
		}

		/**********************************************************************/
		/* Examine if bloc is hideway (unnused)                               */
		/**********************************************************************/
		if (palikka.koodi == BLOCK_PIILO)
			sprite.piilossa = true;
		else
			sprite.piilossa = false;
		

		/**********************************************************************/
		/* Examine if block is the exit                                       */
		/**********************************************************************/
		if (palikka.koodi == BLOCK_LOPETUS && sprite.pelaaja != 0)
			if (!Game->level_clear)
				Game->Finnish();
	}

	//If sprite is thouching the block
	if (sprite_vasen <= palikka.oikea-4 && sprite_oikea >= palikka.vasen+4 && sprite_yla <= palikka.ala && sprite_ala >= palikka.yla+16){
		/**********************************************************************/
		/* Examine if it touches the fire                                     */
		/**********************************************************************/
		if (palikka.koodi == BLOCK_TULI && Game->button1 == 0 && sprite.isku == 0){
			sprite.saatu_vahinko = 2;
			sprite.saatu_vahinko_tyyppi = DAMAGE_FIRE;
		}
	}

	//Examine if there is a block on bottom
	if ((palikka.koodi<80 || palikka.koodi>139) && palikka.koodi != BLOCK_ESTO_ALAS && palikka.koodi < 150){
		mask_index = (int)(sprite_x+sprite_a) - palikka.vasen;

		if (mask_index < 0)
			mask_index = 0;

		if (mask_index > 31)
			mask_index = 31;

		palikka.yla += Game->palikkamaskit[palikka.koodi].alas[mask_index];

		if (palikka.yla >= palikka.ala-2)
			palikka.alas = BLOCK_BACKGROUND;

		palikka.ala -= Game->palikkamaskit[palikka.koodi].ylos[mask_index];
	}

	//If sprite is thouching the block (again?)
	if (sprite_vasen <= palikka.oikea+2 && sprite_oikea >= palikka.vasen-2 && sprite_yla <= palikka.ala && sprite_ala >= palikka.yla){
		/**********************************************************************/
		/* Examine if it is a key and touches lock wall                       */
		/**********************************************************************/
		if (palikka.koodi == BLOCK_LUKKO && sprite.tyyppi->avain){
			Game->map->seinat[palikka.vasen/32+(palikka.yla/32)*PK2KARTTA_KARTTA_LEVEYS] = 255;
			Game->map->Calculate_Edges();

			sprite.piilota = true;

			if (sprite.tyyppi->tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU) {
				Game->keys--;
				if (Game->keys < 1)
					Game->map->Open_Locks();
			}

			Effect_Explosion(palikka.vasen+16, palikka.yla+10, 0);
			Play_GameSFX(open_locks_sound,100, (int)sprite_x, (int)sprite_y, SOUND_SAMPLERATE, false);
		}

		/**********************************************************************/
		/* Make wind effects                                                  */
		/**********************************************************************/
		if (palikka.koodi == BLOCK_VIRTA_VASEMMALLE && vasemmalle)
			sprite_a -= 0.02;

		if (palikka.koodi == BLOCK_VIRTA_OIKEALLE && oikealle)
			sprite_a += 0.02;	//0.05

		/*********************************************************************/
		/* Examine if sprite is on the border to fall                        */
		/*********************************************************************/
		if (palikka.border && sprite.hyppy_ajastin <= 0 && sprite_y < palikka.ala && sprite_y > palikka.yla){
			/* && sprite_ala <= palikka.ala+2)*/ // onko sprite tullut borderlle
			if (sprite_vasen > palikka.vasen)
				sprite.reuna_vasemmalla = true;

			if (sprite_oikea < palikka.oikea)
				sprite.reuna_oikealla = true;
		}
	}

	//Examine walls on left and right

	if (sprite_yla < palikka.ala && sprite_ala-1 > palikka.yla) {
		if (sprite_oikea+sprite_a-1 > palikka.vasen && sprite_vasen+sprite_a < palikka.oikea) {
			// Examine whether the sprite going in the right side of the block.
			if (sprite_oikea+sprite_a < palikka.oikea) {
				// Onko palikka sein�?
				if (palikka.oikealle == BLOCK_WALL) {
					oikealle = false;

					if (palikka.koodi == BLOCK_HISSI_HORI)
						sprite_x = palikka.vasen - sprite_leveys/2;
				}
			}
			// Examine whether the sprite going in the left side of the block.
			if (sprite_vasen+sprite_a > palikka.vasen) {
				if (palikka.vasemmalle == BLOCK_WALL) {
					vasemmalle = false;

					if (palikka.koodi == BLOCK_HISSI_HORI)
						sprite_x = palikka.oikea + sprite_leveys/2;

				}
			}
		}
	}

	sprite_vasen = sprite_x - sprite_leveys/2;
	sprite_oikea = sprite_x + sprite_leveys/2;

	//Examine walls on up and down

	if (sprite_vasen < palikka.oikea && sprite_oikea-1 > palikka.vasen) { //Remove the left and right blocks
		if (sprite_ala+sprite_b-1 >= palikka.yla && sprite_yla+sprite_b <= palikka.ala) { //Get the up and down blocks
			if (sprite_ala+sprite_b-1 <= palikka.ala) { //Just in the sprite's foot
				if (palikka.alas == BLOCK_WALL) { //If it is a wall
					alas = false;
					if (palikka.koodi == BLOCK_HISSI_VERT)
						sprite_y = palikka.yla - sprite_korkeus /2;

					if (sprite_ala-1 >= palikka.yla && sprite_b >= 0) {
						//sprite_y -= sprite_ala - palikka.yla;
						if (palikka.koodi != BLOCK_HISSI_HORI) {
							sprite_y = palikka.yla - sprite_korkeus /2;
						}
					}

					if (sprite.kytkinpaino >= 1) { // Sprite can press the buttons
						if (palikka.koodi == BLOCK_KYTKIN1 && Game->button1 == 0) {
							Game->button1 = KYTKIN_ALOITUSARVO;
							Game->button_moving = 64;
							Play_GameSFX(switch_sound, 100, (int)sprite_x, (int)sprite_y, SOUND_SAMPLERATE, false);
							PInput::Vibrate(1000);
						}

						if (palikka.koodi == BLOCK_KYTKIN2 && Game->button2 == 0) {
							Game->button2 = KYTKIN_ALOITUSARVO;
							Game->button_moving = 64;
							Play_GameSFX(switch_sound, 100, (int)sprite_x, (int)sprite_y, SOUND_SAMPLERATE, false);
							PInput::Vibrate(1000);
						}

						if (palikka.koodi == BLOCK_KYTKIN3 && Game->button3 == 0) {
							Game->button3 = KYTKIN_ALOITUSARVO;
							Game->button_moving = 64;
							Play_GameSFX(switch_sound, 100, (int)sprite_x, (int)sprite_y, SOUND_SAMPLERATE, false);
							PInput::Vibrate(1000);
						}
					}

				}
			}

			if (sprite_yla+sprite_b > palikka.yla) {
				if (palikka.ylos == BLOCK_WALL) {
					ylos = false;

					if (sprite_yla < palikka.ala) {
						if (palikka.koodi == BLOCK_HISSI_VERT && sprite.kyykky) {
							sprite.saatu_vahinko = 2;
							sprite.saatu_vahinko_tyyppi = DAMAGE_IMPACT;
						}

						if (palikka.koodi != BLOCK_HISSI_HORI) {
							//if (sprite.kyykky)
							//	sprite_y = palikka.ala + sprite_korkeus /2;

							sprite.kyykky = true;
						}
					}
				}
			}
		}
	}
}
int Sprite_Movement(int i){
	if (i >= MAX_SPRITEJA || i < 0)
		return -1;

	SpriteClass &sprite = Sprites_List[i]; //address of sprite = address of spritet[i] (if change sprite, change spritet[i])

	if (!sprite.tyyppi)
		return -1;

	// Save values
	sprite_x = sprite.x;
	sprite_y = sprite.y;
	sprite_a = sprite.a;
	sprite_b = sprite.b;

	sprite_leveys  = sprite.tyyppi->leveys;
	sprite_korkeus = sprite.tyyppi->korkeus;

	sprite_vasen = sprite_x-sprite_leveys/2;
	sprite_oikea = sprite_x+sprite_leveys/2;
	sprite_yla	 = sprite_y-sprite_korkeus/2;
	sprite_ala	 = sprite_y+sprite_korkeus/2;

	max_nopeus = (u8)sprite.tyyppi->max_nopeus;

	vedessa = sprite.vedessa;

	x = 0;
	y = 0;

	oikealle	 = true,
	vasemmalle	 = true,
	ylos		 = true,
	alas		 = true;

	map_vasen = 0;
	map_yla   = 0;

	sprite.kyykky = false;

	sprite.reuna_vasemmalla = false;
	sprite.reuna_oikealla = false;


	/* Pistet��n vauhtia tainnutettuihin spriteihin */
	if (sprite.energia < 1)
		max_nopeus = 3;

	// Calculate the remainder of the sprite towards

	if (sprite.hyokkays1 > 0)
		sprite.hyokkays1--;

	if (sprite.hyokkays2 > 0)
		sprite.hyokkays2--;

	if (sprite.lataus > 0)	// aika kahden ampumisen (munimisen) v�lill�
		sprite.lataus --;

	if (sprite.muutos_ajastin > 0)	// aika muutokseen
		sprite.muutos_ajastin --;

	/*****************************************************************************************/
	/* Player-sprite and its controls                                                        */
	/*****************************************************************************************/

	bool add_speed = true;
	bool gliding = false;

	if (sprite.pelaaja != 0 && sprite.energia > 0){
		/* SLOW WALK */
		if (PInput::Keydown(Settings.control_walk_slow))
			add_speed = false;

		/* ATTACK 1 */
		if ((PInput::Keydown(Settings.control_attack1) || Gui_egg) && sprite.lataus == 0 && sprite.ammus1 != -1)
			sprite.hyokkays1 = sprite.tyyppi->hyokkays1_aika;
		/* ATTACK 2 */
		else if ((PInput::Keydown(Settings.control_attack2) || Gui_doodle) && sprite.lataus == 0 && sprite.ammus2 != -1)
				sprite.hyokkays2 = sprite.tyyppi->hyokkays2_aika;

		/* CROUCH */
		sprite.kyykky = false;
		if ((PInput::Keydown(Settings.control_down) || Gui_down) && !sprite.alas) {
			sprite.kyykky = true;
			sprite_yla += sprite_korkeus/1.5;
		}

		/* NAVIGATING*/
		int navigation = Gui_pad;

		if (PInput::Keydown(Settings.control_right))
			navigation = 100;
		
		if (PInput::Keydown(Settings.control_left))
			navigation = -100;

		double a_lisays = 0.04;//0.08;

		if (add_speed) {
			if (rand()%20 == 1 && sprite.animaatio_index == ANIMATION_WALKING) // Draw dust
				Particles_New(PARTICLE_DUST_CLOUDS,sprite_x-8,sprite_ala-8,0.25,-0.25,40,0,0);

			a_lisays += 0.09;//0.05
		}

		if (sprite.alas)
			a_lisays /= 1.5;//2.0

		a_lisays *= double(navigation) / 100;

		if (navigation > 0)
			sprite.flip_x = false;
		else if (navigation < 0)
			sprite.flip_x = true;

		if (sprite.kyykky)	// Slow when couch
			a_lisays /= 10;

		sprite_a += a_lisays;

		/* JUMPING */
		if (sprite.tyyppi->paino > 0) {
			if (PInput::Keydown(Settings.control_jump) || Gui_up) {
				if (!sprite.kyykky) {
					if (sprite.hyppy_ajastin == 0)
						Play_GameSFX(jump_sound, 100, (int)sprite_x, (int)sprite_y,
									  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

					if (sprite.hyppy_ajastin <= 0)
						sprite.hyppy_ajastin = 1; //10;
				}
			} else {
				if (sprite.hyppy_ajastin > 0 && sprite.hyppy_ajastin < 45)
					sprite.hyppy_ajastin = 55;
			}

			/* tippuminen hiljaa alasp�in */
			if ((PInput::Keydown(Settings.control_jump) || Gui_up) && sprite.hyppy_ajastin >= 150/*90+20*/ &&
				sprite.tyyppi->liitokyky)
				gliding = true;
		}
		/* MOVING UP AND DOWN */
		else { // if the player sprite-weight is 0 - like birds

			if (PInput::Keydown(Settings.control_jump) || Gui_up)
				sprite_b -= 0.15;

			if (PInput::Keydown(Settings.control_down) || Gui_down)
				sprite_b += 0.15;

			sprite.hyppy_ajastin = 0;
		}

		/* AI */
		for (int ai=0;ai < SPRITE_MAX_AI;ai++)
			switch (sprite.tyyppi->AI[ai]){
			
			case AI_CHANGE_WHEN_ENERGY_UNDER_2:
				if (sprite.tyyppi->muutos > -1)
					sprite.AI_Change_When_Energy_Under_2(Prototypes_List[sprite.tyyppi->muutos]);
			break;
			
			case AI_CHANGE_WHEN_ENERGY_OVER_1:
			if (sprite.tyyppi->muutos > -1)
				if (sprite.AI_Change_When_Energy_Over_1(Prototypes_List[sprite.tyyppi->muutos])==1)
					Effect_Destruction(FX_DESTRUCT_SAVU_HARMAA, (u32)sprite.x, (u32)sprite.y);
			break;
			
			case AI_MUUTOS_AJASTIN:
				if (sprite.tyyppi->muutos > -1)
					sprite.AI_Muutos_Ajastin(Prototypes_List[sprite.tyyppi->muutos]);
			break;
			
			case AI_VAHINGOITTUU_VEDESTA:
				sprite.AI_Vahingoittuu_Vedesta();
			break;
			
			case AI_MUUTOS_JOS_OSUTTU:
				if (sprite.tyyppi->muutos > -1)
					sprite.AI_Muutos_Jos_Osuttu(Prototypes_List[sprite.tyyppi->muutos]);
			break;

			default: break;
			}

		/* It is not acceptable that a player is anything other than the game character */
		if (sprite.tyyppi->tyyppi != TYPE_GAME_CHARACTER)
			sprite.energia = 0;
	}

	/*****************************************************************************************/
	/* Jump                                                                                  */
	/*****************************************************************************************/

	bool hyppy_maximissa = sprite.hyppy_ajastin >= 90;

	// Jos ollaan hyp�tty / ilmassa:
	if (sprite.hyppy_ajastin > 0) {
		if (sprite.hyppy_ajastin < 50-sprite.tyyppi->max_hyppy)
			sprite.hyppy_ajastin = 50-sprite.tyyppi->max_hyppy;

		if (sprite.hyppy_ajastin < 10)
			sprite.hyppy_ajastin = 10;

		if (!hyppy_maximissa) {
		// sprite_b = (sprite.tyyppi->max_hyppy/2 - sprite.hyppy_ajastin/2)/-2.0;//-4
		   sprite_b = -sin_table[sprite.hyppy_ajastin]/8;//(sprite.tyyppi->max_hyppy/2 - sprite.hyppy_ajastin/2)/-2.5;
			if (sprite_b > sprite.tyyppi->max_hyppy){
				sprite_b = sprite.tyyppi->max_hyppy/10.0;
				sprite.hyppy_ajastin = 90 - sprite.hyppy_ajastin;
			}

		}

		if (sprite.hyppy_ajastin < 180)
			sprite.hyppy_ajastin += 2;
	}

	if (sprite.hyppy_ajastin < 0)
		sprite.hyppy_ajastin++;

	if (sprite_b > 0 && !hyppy_maximissa)
		sprite.hyppy_ajastin = 90;//sprite.tyyppi->max_hyppy*2;

	/*****************************************************************************************/
	/* Hit recovering                                                                        */
	/*****************************************************************************************/

	if (sprite.isku > 0)
		sprite.isku--;

	/*****************************************************************************************/
	/* Invisibility                                                                          */
	/*****************************************************************************************/

	if (sprite.invisible > 0)
		sprite.invisible--;

	/*****************************************************************************************/
	/* Gravity effect                                                                        */
	/*****************************************************************************************/

	if (sprite.paino != 0 && (sprite.hyppy_ajastin <= 0 || sprite.hyppy_ajastin >= 45))
		sprite_b += sprite.paino/1.25;// + sprite_b/1.5;

	if (gliding && sprite_b > 0) // If gliding
		sprite_b /= 1.3;//1.5;//3

	/*****************************************************************************************/
	/* Speed limits                                                                          */
	/*****************************************************************************************/

	if (sprite_b > 4.0)//4
		sprite_b = 4.0;//4

	if (sprite_b < -4.0)
		sprite_b = -4.0;

	if (sprite_a > max_nopeus)
		sprite_a = max_nopeus;

	if (sprite_a < -max_nopeus)
		sprite_a = -max_nopeus;

	/*****************************************************************************************/
	/* Blocks colision -                                                                     */
	/*****************************************************************************************/

	int palikat_x_lkm = -1,
	    palikat_y_lkm = -1;
	    //palikat_lkm;
	u32 p;

	if (sprite.tyyppi->tiletarkistus){ //Find the tiles that the sprite occupies

		palikat_x_lkm = (int)((sprite_leveys) /32)+4; //Number of blocks
		palikat_y_lkm = (int)((sprite_korkeus)/32)+4;

		map_vasen = (int)(sprite_vasen)/32;	//Position in tile map
		map_yla	 = (int)(sprite_yla)/32;

		for ( y = 0; y < palikat_y_lkm; y++)
			for ( x = 0; x < palikat_x_lkm; x++) //For each block, create a array of blocks around the sprite
				Game->palikat[x+(y*palikat_x_lkm)] = Block_Get(map_vasen+x-1,map_yla+y-1); //x = 0, y = 0

		/*****************************************************************************************/
		/* Going through the blocks around the sprite.                                           */
		/*****************************************************************************************/

		//palikat_lkm = palikat_y_lkm*palikat_x_lkm;
		for (y = 0; y < palikat_y_lkm; y++){
			for (x = 0; x < palikat_x_lkm; x++) {
				p = x + y*palikat_x_lkm;
				if ( p < 300 )
					Check_Blocks(sprite, Game->palikat[p]);
			}
		}
	}
	/*****************************************************************************************/
	/* If the sprite is under water                                                          */
	/*****************************************************************************************/

	if (sprite.vedessa) {

		if (!sprite.tyyppi->osaa_uida || sprite.energia < 1) {
			/*
			if (sprite_b > 0)
				sprite_b /= 2.0;

			sprite_b -= (1.5-sprite.paino)/10;*/
			sprite_b /= 2.0;
			sprite_a /= 1.05;

			if (sprite.hyppy_ajastin > 0 && sprite.hyppy_ajastin < 90)
				sprite.hyppy_ajastin--;
		}

		if (rand()%80 == 1)
			Particles_New(PARTICLE_SPARK,sprite_x-4,sprite_y,0,-0.5-rand()%2,rand()%30+30,0,32);
	}

	if (vedessa != sprite.vedessa) { // Sprite comes in or out from water
		Effect_Splash((int)sprite_x,(int)sprite_y,32);
		Play_GameSFX(splash_sound, 100, (int)sprite_x, (int)sprite_y, SOUND_SAMPLERATE, true);
	}

	/*****************************************************************************************/
	/* Sprite weight                                                                         */
	/*****************************************************************************************/

	sprite.paino = sprite.alkupaino;
	sprite.kytkinpaino = sprite.paino;

	if (sprite.energia < 1 && sprite.paino == 0) // Fall when is death
		sprite.paino = 1;

	/*****************************************************************************************/
	/* Sprite collision with other sprites                                                   */
	/*****************************************************************************************/

	int tuhoutuminen;
	double sprite2_yla; // kyykistymiseen liittyv�
	PK2BLOCK spritepalikka;

	SpriteClass *sprite2;

	//Compare this sprite with every sprite in the game
	for (int sprite_index = 0; sprite_index < MAX_SPRITEJA; sprite_index++) {
		sprite2 = &Sprites_List[sprite_index];

		if (sprite_index != i && /*!sprite2->piilota*/sprite2->aktiivinen) {
			if (sprite2->kyykky)
				sprite2_yla = sprite2->tyyppi->korkeus / 3;//1.5;
			else
				sprite2_yla = 0;

			if (sprite2->tyyppi->este && sprite.tyyppi->tiletarkistus) { //If there is a block sprite active

				if (sprite_x-sprite_leveys/2 +sprite_a  <= sprite2->x + sprite2->tyyppi->leveys /2 &&
					sprite_x+sprite_leveys/2 +sprite_a  >= sprite2->x - sprite2->tyyppi->leveys /2 &&
					sprite_y-sprite_korkeus/2+sprite_b <= sprite2->y + sprite2->tyyppi->korkeus/2 &&
					sprite_y+sprite_korkeus/2+sprite_b >= sprite2->y - sprite2->tyyppi->korkeus/2)
				{
					spritepalikka.koodi = 0;
					spritepalikka.ala   = (int)sprite2->y + sprite2->tyyppi->korkeus/2;
					spritepalikka.oikea = (int)sprite2->x + sprite2->tyyppi->leveys/2;
					spritepalikka.vasen = (int)sprite2->x - sprite2->tyyppi->leveys/2;
					spritepalikka.yla   = (int)sprite2->y - sprite2->tyyppi->korkeus/2;

					spritepalikka.water  = false;

					//spritepalikka.koodi = BLOCK_HISSI_VERT;
					/*
					Block_Set_Barriers(spritepalikka);

					if (!sprite.tyyppi->este)
					{
						if (!sprite2->tyyppi->este_alas)
							spritepalikka.alas		 = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_ylos)
							spritepalikka.ylos		 = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_oikealle)
							spritepalikka.oikealle   = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_vasemmalle)
							spritepalikka.vasemmalle = BLOCK_BACKGROUND;
					}
					*/

					Block_Set_Barriers(spritepalikka);

					if (!sprite.tyyppi->este){
						if (!sprite2->tyyppi->este_alas)
							spritepalikka.alas = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_ylos)
							spritepalikka.ylos = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_oikealle)
							spritepalikka.oikealle = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_vasemmalle)
							spritepalikka.vasemmalle = BLOCK_BACKGROUND;
					}

					if (sprite2->a > 0)
						spritepalikka.koodi = BLOCK_HISSI_HORI;

					if (sprite2->b > 0)
						spritepalikka.koodi = BLOCK_HISSI_VERT;

					Check_Blocks2(sprite, spritepalikka); //Colision sprite and sprite block
				}
			}

			if (sprite_x <= sprite2->x + sprite2->tyyppi->leveys /2 &&
				sprite_x >= sprite2->x - sprite2->tyyppi->leveys /2 &&
				sprite_y/*yla*/ <= sprite2->y + sprite2->tyyppi->korkeus/2 &&
				sprite_y/*ala*/ >= sprite2->y - sprite2->tyyppi->korkeus/2 + sprite2_yla)
			{
				// sprites with same index change directions when touch
				if (sprite.tyyppi->indeksi == sprite2->tyyppi->indeksi &&
					sprite2->energia > 0/* && sprite.pelaaja == 0*/)
				{
					if (sprite.x < sprite2->x)
						oikealle = false;
					if (sprite.x > sprite2->x)
						vasemmalle = false;
					if (sprite.y < sprite2->y)
						alas = false;
					if (sprite.y > sprite2->y)
						ylos = false;
				}

				if (sprite.tyyppi->Onko_AI(AI_NUOLET_VAIKUTTAVAT)) {

					if (sprite2->tyyppi->Onko_AI(AI_NUOLI_OIKEALLE)) {
						sprite_a = sprite.tyyppi->max_nopeus / 3.5;
						sprite_b = 0;
					}
					else if (sprite2->tyyppi->Onko_AI(AI_NUOLI_VASEMMALLE)) {
						sprite_a = sprite.tyyppi->max_nopeus / -3.5;
						sprite_b = 0;
					}

					if (sprite2->tyyppi->Onko_AI(AI_NUOLI_YLOS)) {
						sprite_b = sprite.tyyppi->max_nopeus / -3.5;
						sprite_a = 0;
					}
					else if (sprite2->tyyppi->Onko_AI(AI_NUOLI_ALAS)) {
						sprite_b = sprite.tyyppi->max_nopeus / 3.5;
						sprite_a = 0;
					}
				}

				/* spritet voivat vaihtaa tietoa pelaajan olinpaikasta */
	/*			if (sprite.pelaaja_x != -1 && sprite2->pelaaja_x == -1)
				{
					sprite2->pelaaja_x = sprite.pelaaja_x + rand()%30 - rand()%30;
					sprite.pelaaja_x = -1;
				} */


				if (sprite.vihollinen != sprite2->vihollinen && sprite.emosprite != sprite_index) {
					if (sprite2->tyyppi->tyyppi != TYPE_BACKGROUND &&
						sprite.tyyppi->tyyppi   != TYPE_BACKGROUND &&
						sprite2->tyyppi->tyyppi != TYPE_TELEPORT &&
						sprite2->isku == 0 &&
						sprite.isku == 0 &&
						sprite2->energia > 0 &&
						sprite.energia > 0 &&
						sprite2->saatu_vahinko < 1)
					{

						// Tippuuko toinen sprite p��lle?

						if (sprite2->b > 2 && sprite2->paino >= 0.5 &&
							sprite2->y < sprite_y && !sprite.tyyppi->este &&
							sprite.tyyppi->tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU)
						{
							//sprite.saatu_vahinko = (int)sprite2->paino;//1;
							sprite.saatu_vahinko = (int)(sprite2->paino+sprite2->b/4);
							sprite.saatu_vahinko_tyyppi = DAMAGE_DROP;
							sprite2->hyppy_ajastin = 1;
						}

						// If there is another sprite damaging
						if (sprite.tyyppi->vahinko > 0 && sprite2->tyyppi->tyyppi != TYPE_BONUS) {
							
							sprite2->saatu_vahinko        = sprite.tyyppi->vahinko;
							sprite2->saatu_vahinko_tyyppi = sprite.tyyppi->vahinko_tyyppi;
							
							if ( !(sprite2->pelaaja && sprite2->invisible) ) //If sprite2 isn't a invisible player
								sprite.hyokkays1 = sprite.tyyppi->hyokkays1_aika; //Then sprite attack

							// The projectiles are shattered by shock
							if (sprite2->tyyppi->tyyppi == TYPE_PROJECTILE) {
								sprite.saatu_vahinko = 1;//sprite2->tyyppi->vahinko;
								sprite.saatu_vahinko_tyyppi = sprite2->tyyppi->vahinko_tyyppi;
							}

							if (sprite.tyyppi->tyyppi == TYPE_PROJECTILE) {
								sprite.saatu_vahinko = 1;//sprite2->tyyppi->vahinko;
								sprite.saatu_vahinko_tyyppi = sprite2->tyyppi->vahinko_tyyppi;
							}
						}
					}
				}

				// lis�t��n spriten painoon sit� koskettavan toisen spriten paino
				if (sprite.paino > 0)
					sprite.kytkinpaino += sprite2->tyyppi->paino;

			}
		}
	}

	/*****************************************************************************************/
	/* If the sprite has suffered damage                                                     */
	/*****************************************************************************************/
	
	// If it is invisible, just these damages can injury it
	if (sprite.saatu_vahinko != 0 && sprite.invisible != 0 && 
		sprite.saatu_vahinko_tyyppi != DAMAGE_FIRE &&
		sprite.saatu_vahinko_tyyppi != DAMAGE_COMPRESSION &&
		sprite.saatu_vahinko_tyyppi != DAMAGE_DROP &&
		sprite.saatu_vahinko_tyyppi != DAMAGE_ALL) {
		
		sprite.saatu_vahinko = 0;
		sprite.saatu_vahinko_tyyppi = DAMAGE_NONE;
	}

	if (sprite.saatu_vahinko != 0 && sprite.energia > 0 && sprite.tyyppi->tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU){
		if (sprite.tyyppi->suojaus != sprite.saatu_vahinko_tyyppi || sprite.tyyppi->suojaus == DAMAGE_NONE){
			sprite.energia -= sprite.saatu_vahinko;
			sprite.isku = DAMAGE_TIME;

			if (sprite.saatu_vahinko_tyyppi == DAMAGE_ELECTRIC)
				sprite.isku *= 6;

			Play_GameSFX(sprite.tyyppi->aanet[SOUND_DAMAGE], 100, (int)sprite.x, (int)sprite.y,
						  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

			if (sprite.tyyppi->tuhoutuminen%100 == FX_DESTRUCT_HOYHENET)
				Effect_Destruction(FX_DESTRUCT_HOYHENET, (u32)sprite.x, (u32)sprite.y);

			if (sprite.tyyppi->tyyppi != TYPE_PROJECTILE){
				Particles_New(PARTICLE_STAR,sprite_x,sprite_y,-1,-1,60,0.01,128);
				Particles_New(PARTICLE_STAR,sprite_x,sprite_y, 0,-1,60,0.01,128);
				Particles_New(PARTICLE_STAR,sprite_x,sprite_y, 1,-1,60,0.01,128);
			}

			if (sprite.Onko_AI(AI_VAIHDA_KALLOT_JOS_OSUTTU))
				Game->map->Change_SkullBlocks();

			if (sprite.Onko_AI(AI_HYOKKAYS_1_JOS_OSUTTU)){
				sprite.hyokkays1 = sprite.tyyppi->hyokkays1_aika;
				sprite.lataus = 0;
			}

			if (sprite.Onko_AI(AI_HYOKKAYS_2_JOS_OSUTTU)){
				sprite.hyokkays2 = sprite.tyyppi->hyokkays2_aika;
				sprite.lataus = 0;
			}

		}

		sprite.saatu_vahinko = 0;
		sprite.saatu_vahinko_tyyppi = DAMAGE_NONE;


		/*****************************************************************************************/
		/* If the sprite is destroyed                                                            */
		/*****************************************************************************************/

		if (sprite.energia < 1){
			tuhoutuminen = sprite.tyyppi->tuhoutuminen;

			if (tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU){
				if (sprite.tyyppi->bonus > -1 && sprite.tyyppi->bonusten_lkm > 0)
					if (sprite.tyyppi->bonus_aina || rand()%4 == 1)
						for (int bi=0; bi<sprite.tyyppi->bonusten_lkm; bi++)
							Sprites_add(sprite.tyyppi->bonus,0,sprite_x-11+(10-rand()%20),
											  sprite_ala-16-(10+rand()%20), i, true);

				if (sprite.Onko_AI(AI_VAIHDA_KALLOT_JOS_TYRMATTY) && !sprite.Onko_AI(AI_VAIHDA_KALLOT_JOS_OSUTTU))
					Game->map->Change_SkullBlocks();

				if (tuhoutuminen >= FX_DESTRUCT_ANIMAATIO)
					tuhoutuminen -= FX_DESTRUCT_ANIMAATIO;
				else
					sprite.piilota = true;

				Effect_Destruction(tuhoutuminen, (u32)sprite.x, (u32)sprite.y);
				Play_GameSFX(sprite.tyyppi->aanet[SOUND_DESTRUCTION],100, (int)sprite.x, (int)sprite.y,
							  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

				if (sprite.Onko_AI(AI_UUSI_JOS_TUHOUTUU)) {
					Sprites_add(sprite.tyyppi->indeksi,0,sprite.alku_x-sprite.tyyppi->leveys, sprite.alku_y-sprite.tyyppi->korkeus,i, false);
				} //TODO - does sprite.tyyppi->indeksi work

				if (sprite.tyyppi->tyyppi == TYPE_GAME_CHARACTER && sprite.tyyppi->pisteet != 0){
					char luku[10];
					sprintf(luku, "%i", sprite.tyyppi->pisteet);
					Fadetext_New(fontti2,luku,(int)Sprites_List[i].x-8,(int)Sprites_List[i].y-8,80);
					Game->score_increment += sprite.tyyppi->pisteet;
				}
			} else
				sprite.energia = 1;
		}
	}

	if (sprite.isku == 0)
		sprite.saatu_vahinko_tyyppi = DAMAGE_NONE;


	/*****************************************************************************************/
	/* Revisions                                                                             */
	/*****************************************************************************************/

	if (!oikealle)
		if (sprite_a > 0)
			sprite_a = 0;

	if (!vasemmalle)
		if (sprite_a < 0)
			sprite_a = 0;

	if (!ylos){
		if (sprite_b < 0)
			sprite_b = 0;

		if (!hyppy_maximissa)
			sprite.hyppy_ajastin = 95;//sprite.tyyppi->max_hyppy * 2;
	}

	if (!alas)
		if (sprite_b >= 0){ //If sprite is falling
			if (sprite.hyppy_ajastin > 0){
				if (sprite.hyppy_ajastin >= 90+10){
					Play_GameSFX(pump_sound,30,(int)sprite_x, (int)sprite_y,
				                  int(25050-sprite.paino*3000),true);

					//Particles_New(	PARTICLE_DUST_CLOUDS,sprite_x+rand()%5-rand()%5-10,sprite_ala+rand()%3-rand()%3,
					//			  0,-0.2,rand()%50+20,0,0);

					if (rand()%7 == 1) {
						Particles_New(PARTICLE_SMOKE,sprite_x+rand()%5-rand()%5-10,sprite_ala+rand()%3-rand()%3,
									  	   0.3,-0.1,450,0,0);
						Particles_New(PARTICLE_SMOKE,sprite_x+rand()%5-rand()%5-10,sprite_ala+rand()%3-rand()%3,
									  	   -0.3,-0.1,450,0,0);
					}

					if (sprite.paino > 1)
						Game->vibration = 34 + int(sprite.paino * 20);
				}

				sprite.hyppy_ajastin = 0;
			}

			sprite_b = 0;
		}

	/*****************************************************************************************/
	/* Set correct values                                                                    */
	/*****************************************************************************************/

	if (sprite_b > 4.0)
		sprite_b = 4.0;

	if (sprite_b < -4.0)
		sprite_b = -4.0;

	if (sprite_a > max_nopeus)
		sprite_a = max_nopeus;

	if (sprite_a < -max_nopeus)
		sprite_a = -max_nopeus;

	if (sprite.energia < 0)
		sprite.energia = 0;

	if (sprite.energia > sprite.tyyppi->energia)
		sprite.energia = sprite.tyyppi->energia;

	if (sprite.isku == 0 || sprite.pelaaja == 1) {
		sprite_x += sprite_a;
		sprite_y += sprite_b;
	}

	if (&sprite == Player_Sprite || sprite.energia < 1) {
		double kitka = 1.04;

		if (Game->map->ilma == ILMA_SADE || Game->map->ilma == ILMA_SADEMETSA)
			kitka = 1.03; // Slippery ground in the rain

		if (Game->map->ilma == ILMA_LUMISADE)
			kitka = 1.01; // And even more on snow

		if (!alas)
			sprite_a /= kitka;
		else
			sprite_a /= 1.03;//1.02//1.05

		sprite_b /= 1.25;
	}

	sprite.x = sprite_x;
	sprite.y = sprite_y;
	sprite.a = sprite_a;
	sprite.b = sprite_b;

	sprite.oikealle = oikealle;
	sprite.vasemmalle = vasemmalle;
	sprite.alas = alas;
	sprite.ylos = ylos;

	/*
	sprite.paino = sprite.tyyppi->paino;

	if (sprite.energia < 1 && sprite.paino == 0)
		sprite.paino = 1;*/

	if (sprite.hyppy_ajastin < 0)
		sprite.alas = false;

	//sprite.kyykky   = false;

	/*****************************************************************************************/
	/* AI                                                                                    */
	/*****************************************************************************************/

	//TODO run sprite lua script
	
	if (sprite.pelaaja == 0) {
		for (int ai=0;ai < SPRITE_MAX_AI; ai++)
			switch (sprite.tyyppi->AI[ai]) {
				case AI_NONE:							ai = SPRITE_MAX_AI; // lopetetaan
													break;
				case AI_KANA:						sprite.AI_Kana();
													break;
				case AI_LITTLE_CHICKEN:					sprite.AI_Kana();
													break;
				case AI_SAMMAKKO1:					sprite.AI_Sammakko1();
													break;
				case AI_SAMMAKKO2:					sprite.AI_Sammakko2();
													break;
				case AI_BONUS:						sprite.AI_Bonus();
													break;
				case AI_EGG:						sprite.AI_Egg();
													break;
				case AI_AMMUS:						sprite.AI_Ammus();
													break;
				case AI_HYPPIJA:					sprite.AI_Hyppija();
													break;
				case AI_BASIC:						sprite.AI_Perus();
													break;
				case AI_NONSTOP:					sprite.AI_NonStop();
													break;
				case AI_KAANTYY_ESTEESTA_HORI:		sprite.AI_Kaantyy_Esteesta_Hori();
													break;
				case AI_KAANTYY_ESTEESTA_VERT:		sprite.AI_Kaantyy_Esteesta_Vert();
													break;
				case AI_VAROO_KUOPPAA:				sprite.AI_Varoo_Kuoppaa();
													break;
				case AI_RANDOM_SUUNNANVAIHTO_HORI:	sprite.AI_Random_Suunnanvaihto_Hori();
													break;
				case AI_RANDOM_KAANTYMINEN:			sprite.AI_Random_Kaantyminen();
													break;
				case AI_RANDOM_JUMP:				sprite.AI_Random_Hyppy();
													break;
				case AI_FOLLOW_PLAYER:			if (Player_Sprite->invisible == 0)
														sprite.AI_Seuraa_Pelaajaa(*Player_Sprite);
													break;
				case AI_SEURAA_PELAAJAA_JOS_NAKEE:	if (Player_Sprite->invisible == 0)
														sprite.AI_Seuraa_Pelaajaa_Jos_Nakee(*Player_Sprite);
													break;
				case AI_SEURAA_PELAAJAA_VERT_HORI:	if (Player_Sprite->invisible == 0)
														sprite.AI_Seuraa_Pelaajaa_Vert_Hori(*Player_Sprite);
													break;
				case AI_SEURAA_PELAAJAA_JOS_NAKEE_VERT_HORI:
													if (Player_Sprite->invisible == 0)
														sprite.AI_Seuraa_Pelaajaa_Jos_Nakee_Vert_Hori(*Player_Sprite);
													break;
				case AI_PAKENEE_PELAAJAA_JOS_NAKEE:	if (Player_Sprite->invisible == 0)
														sprite.AI_Pakenee_Pelaajaa_Jos_Nakee(*Player_Sprite);
													break;
				case AI_POMMI:						sprite.AI_Pommi();
													break;
				case AI_HYOKKAYS_1_JOS_OSUTTU:		sprite.AI_Hyokkays_1_Jos_Osuttu();
													break;
				case AI_HYOKKAYS_2_JOS_OSUTTU:		sprite.AI_Hyokkays_2_Jos_Osuttu();
													break;
				case AI_HYOKKAYS_1_NONSTOP:			sprite.AI_Hyokkays_1_Nonstop();
													break;
				case AI_HYOKKAYS_2_NONSTOP:			sprite.AI_Hyokkays_2_Nonstop();
													break;
				case AI_HYOKKAYS_1_JOS_PELAAJA_EDESSA:
													if (Player_Sprite->invisible == 0)
														sprite.AI_Hyokkays_1_Jos_Pelaaja_Edessa(*Player_Sprite);
													break;
				case AI_HYOKKAYS_2_JOS_PELAAJA_EDESSA:
													if (Player_Sprite->invisible == 0)
														sprite.AI_Hyokkays_2_Jos_Pelaaja_Edessa(*Player_Sprite);
													break;
				case AI_HYOKKAYS_1_JOS_PELAAJA_ALAPUOLELLA:
													if (Player_Sprite->invisible == 0)
														sprite.AI_Hyokkays_1_Jos_Pelaaja_Alapuolella(*Player_Sprite);
													break;
				case AI_HYPPY_JOS_PELAAJA_YLAPUOLELLA:
													if (Player_Sprite->invisible == 0)
														sprite.AI_Hyppy_Jos_Pelaaja_Ylapuolella(*Player_Sprite);
													break;
				case AI_VAHINGOITTUU_VEDESTA:		sprite.AI_Vahingoittuu_Vedesta();
													break;
				case AI_TAPA_KAIKKI:				sprite.AI_Tapa_Kaikki();
													break;
				case AI_KITKA_VAIKUTTAA:			sprite.AI_Kitka_Vaikuttaa();
													break;
				case AI_PIILOUTUU:					sprite.AI_Piiloutuu();
													break;
				case AI_PALAA_ALKUUN_X:				sprite.AI_Palaa_Alkuun_X();
													break;
				case AI_PALAA_ALKUUN_Y:				sprite.AI_Palaa_Alkuun_Y();
													break;
				case AI_LIIKKUU_X_COS:				sprite.AI_Liikkuu_X(cos_table[degree%360]);
													break;
				case AI_LIIKKUU_Y_COS:				sprite.AI_Liikkuu_Y(cos_table[degree%360]);
													break;
				case AI_LIIKKUU_X_SIN:				sprite.AI_Liikkuu_X(sin_table[degree%360]);
													break;
				case AI_LIIKKUU_Y_SIN:				sprite.AI_Liikkuu_Y(sin_table[degree%360]);
													break;
				case AI_LIIKKUU_X_COS_NOPEA:		sprite.AI_Liikkuu_X(cos_table[(degree*2)%360]);
													break;
				case AI_LIIKKUU_Y_SIN_NOPEA:		sprite.AI_Liikkuu_Y(sin_table[(degree*2)%360]);
													break;
				case AI_LIIKKUU_X_COS_HIDAS:		sprite.AI_Liikkuu_X(cos_table[(degree/2)%360]);
													break;
				case AI_LIIKKUU_Y_SIN_HIDAS:		sprite.AI_Liikkuu_Y(sin_table[(degree/2)%360]);
													break;
				case AI_LIIKKUU_Y_SIN_VAPAA:		sprite.AI_Liikkuu_Y(sin_table[(sprite.ajastin/2)%360]);
													break;
				case AI_CHANGE_WHEN_ENERGY_UNDER_2:	if (sprite.tyyppi->muutos > -1)
														sprite.AI_Change_When_Energy_Under_2(Prototypes_List[sprite.tyyppi->muutos]);
													break;
				case AI_CHANGE_WHEN_ENERGY_OVER_1:	if (sprite.tyyppi->muutos > -1)
														if (sprite.AI_Change_When_Energy_Over_1(Prototypes_List[sprite.tyyppi->muutos])==1)
															Effect_Destruction(FX_DESTRUCT_SAVU_HARMAA, (u32)sprite.x, (u32)sprite.y);
													break;
				case AI_MUUTOS_AJASTIN:				if (sprite.tyyppi->muutos > -1) {
														sprite.AI_Muutos_Ajastin(Prototypes_List[sprite.tyyppi->muutos]);
													}
													break;
				case AI_MUUTOS_JOS_OSUTTU:			if (sprite.tyyppi->muutos > -1) {
														sprite.AI_Muutos_Jos_Osuttu(Prototypes_List[sprite.tyyppi->muutos]);
													}
													break;
				case AI_TELEPORT:					if (sprite.AI_Teleportti(i, Sprites_List, MAX_SPRITEJA, *Player_Sprite)==1)
													{

														Game->camera_x = (int)Player_Sprite->x;
														Game->camera_y = (int)Player_Sprite->y;
														Game->dcamera_x = Game->camera_x-screen_width/2;
														Game->dcamera_y = Game->camera_y-screen_height/2;
														PDraw::fade_in(PDraw::FADE_NORMAL);
														if (sprite.tyyppi->aanet[SOUND_ATTACK2] != -1)
															Play_MenuSFX(sprite.tyyppi->aanet[SOUND_ATTACK2], 100);
															//Play_GameSFX(, 100, Game->camera_x, Game->camera_y, SOUND_SAMPLERATE, false);


													}
													break;
				case AI_KIIPEILIJA:					sprite.AI_Kiipeilija();
													break;
				case AI_KIIPEILIJA2:				sprite.AI_Kiipeilija2();
													break;
				case AI_TUHOUTUU_JOS_EMO_TUHOUTUU:	sprite.AI_Tuhoutuu_Jos_Emo_Tuhoutuu(Sprites_List);
													break;

				case AI_TIPPUU_TARINASTA:			sprite.AI_Tippuu_Tarinasta(Game->vibration + Game->button_moving);
													break;
				case AI_LIIKKUU_ALAS_JOS_KYTKIN1_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button1,0,1);
													break;
				case AI_LIIKKUU_YLOS_JOS_KYTKIN1_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button1,0,-1);
													break;
				case AI_LIIKKUU_VASEMMALLE_JOS_KYTKIN1_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button1,-1,0);
													break;
				case AI_LIIKKUU_OIKEALLE_JOS_KYTKIN1_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button1,1,0);
													break;
				case AI_LIIKKUU_ALAS_JOS_KYTKIN2_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button2,0,1);
													break;
				case AI_LIIKKUU_YLOS_JOS_KYTKIN2_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button2,0,-1);
													break;
				case AI_LIIKKUU_VASEMMALLE_JOS_KYTKIN2_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button2,-1,0);
													break;
				case AI_LIIKKUU_OIKEALLE_JOS_KYTKIN2_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button2,1,0);
													break;
				case AI_LIIKKUU_ALAS_JOS_KYTKIN3_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button3,0,1);
													break;
				case AI_LIIKKUU_YLOS_JOS_KYTKIN3_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button3,0,-1);
													break;
				case AI_LIIKKUU_VASEMMALLE_JOS_KYTKIN3_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button3,-1,0);
													break;
				case AI_LIIKKUU_OIKEALLE_JOS_KYTKIN3_PAINETTU: sprite.AI_Liikkuu_Jos_Kytkin_Painettu(Game->button3,1,0);
													break;
				case AI_TIPPUU_JOS_KYTKIN1_PAINETTU: sprite.AI_Tippuu_Jos_Kytkin_Painettu(Game->button1);
													break;
				case AI_TIPPUU_JOS_KYTKIN2_PAINETTU: sprite.AI_Tippuu_Jos_Kytkin_Painettu(Game->button2);
													break;
				case AI_TIPPUU_JOS_KYTKIN3_PAINETTU: sprite.AI_Tippuu_Jos_Kytkin_Painettu(Game->button3);
													break;
				case AI_RANDOM_LIIKAHDUS_VERT_HORI:	sprite.AI_Random_Liikahdus_Vert_Hori();
													break;
				case AI_KAANTYY_JOS_OSUTTU:			sprite.AI_Kaantyy_Jos_Osuttu();
													break;
				case AI_EVIL_ONE:					if (sprite.energia < 1) PSound::set_musicvolume(0);
													break;

				case AI_INFO1:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info01));break;
				case AI_INFO2:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info02));break;
				case AI_INFO3:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info03));break;
				case AI_INFO4:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info04));break;
				case AI_INFO5:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info05));break;
				case AI_INFO6:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info06));break;
				case AI_INFO7:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info07));break;
				case AI_INFO8:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info08));break;
				case AI_INFO9:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info09));break;
				case AI_INFO10:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info10));break;
				case AI_INFO11:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info11));break;
				case AI_INFO12:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info12));break;
				case AI_INFO13:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info13));break;
				case AI_INFO14:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info14));break;
				case AI_INFO15:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info15));break;
				case AI_INFO16:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info16));break;
				case AI_INFO17:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info17));break;
				case AI_INFO18:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info18));break;
				case AI_INFO19:						if (sprite.AI_Info(*Player_Sprite))	Game->Show_Info(tekstit->Get_Text(PK_txt.info19));break;

				default:							break;
			}
	}

	//if (kaiku == 1 && sprite.tyyppi->tyyppi == TYPE_PROJECTILE && sprite.tyyppi->vahinko_tyyppi == DAMAGE_NOISE &&
	//	sprite.tyyppi->aanet[SOUND_ATTACK1] > -1)
	//	Play_GameSFX(sprite.tyyppi->aanet[SOUND_ATTACK1],20, (int)sprite_x, (int)sprite_y,
	//				  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);


	/*****************************************************************************************/
	/* Set game area to sprite                                                               */
	/*****************************************************************************************/

	if (sprite.x < 0)
		sprite.x = 0;

	if (sprite.y < -sprite_korkeus)
		sprite.y = -sprite_korkeus;

	if (sprite.x > PK2KARTTA_KARTTA_LEVEYS*32)
		sprite.x = PK2KARTTA_KARTTA_LEVEYS*32;

	// If the sprite falls under the lower edge of the map
	if (sprite.y > PK2KARTTA_KARTTA_KORKEUS*32 + sprite_korkeus) {

		sprite.y = PK2KARTTA_KARTTA_KORKEUS*32 + sprite_korkeus;
		sprite.energia = 0;
		if (&sprite != Player_Sprite)
			sprite.piilota = true;

		if (sprite.kytkinpaino >= 1)
			Game->vibration = 50;
	}

	if (sprite.a > max_nopeus)
		sprite.a = max_nopeus;

	if (sprite.a < -max_nopeus)
		sprite.a = -max_nopeus;


	/*****************************************************************************************/
	/* Attacks 1 and 2                                                                       */
	/*****************************************************************************************/

	// If the sprite is ready and isn't crouching
	if (sprite.lataus == 0 && !sprite.kyykky) {
		// hy�kk�ysaika on "tapissa" mik� tarkoittaa sit�, ett� aloitetaan hy�kk�ys
		if (sprite.hyokkays1 == sprite.tyyppi->hyokkays1_aika) {
			// provides recovery time, after which the sprite can attack again
			sprite.lataus = sprite.tyyppi->latausaika;
			if(sprite.lataus == 0) sprite.lataus = 5;
			// jos spritelle ei ole m��ritelty omaa latausaikaa ...
			if (sprite.ammus1 > -1 && sprite.tyyppi->latausaika == 0)
			// ... ja ammukseen on, otetaan latausaika ammuksesta
				if (Prototypes_List[sprite.ammus1].tulitauko > 0)
					sprite.lataus = Prototypes_List[sprite.ammus1].tulitauko;

			// soitetaan hy�kk�ys��ni
			Play_GameSFX(sprite.tyyppi->aanet[SOUND_ATTACK1],100, (int)sprite_x, (int)sprite_y,
						  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

			if (sprite.ammus1 > -1) {
				Sprites_add_ammo(sprite.ammus1,0,sprite_x, sprite_y, i);

		//		if (Prototypes_List[sprite.ammus1].aanet[SOUND_ATTACK1] > -1)
		//			Play_GameSFX(Prototypes_List[sprite.ammus1].aanet[SOUND_ATTACK1],100, (int)sprite_x, (int)sprite_y,
		//						  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);
			}
		}

		// Sama kuin hy�kk�ys 1:ss�
		if (sprite.hyokkays2 == sprite.tyyppi->hyokkays2_aika) {
			sprite.lataus = sprite.tyyppi->latausaika;
			if(sprite.lataus == 0) sprite.lataus = 5;
			if (sprite.ammus2 > -1  && sprite.tyyppi->latausaika == 0)
				if (Prototypes_List[sprite.ammus2].tulitauko > 0)
					sprite.lataus = Prototypes_List[sprite.ammus2].tulitauko;

			Play_GameSFX(sprite.tyyppi->aanet[SOUND_ATTACK2],100,(int)sprite_x, (int)sprite_y,
						  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

			if (sprite.ammus2 > -1) {
				Sprites_add_ammo(sprite.ammus2,0,sprite_x, sprite_y, i);

		//		if (Prototypes_List[sprite.ammus2].aanet[SOUND_ATTACK1] > -1)
		//			Play_GameSFX(Prototypes_List[sprite.ammus2].aanet[SOUND_ATTACK1],100, (int)sprite_x, (int)sprite_y,
		//						  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

			}
		}
	}

	// Random sounds
	if (sprite.tyyppi->aanet[SOUND_RANDOM] != -1 && rand()%200 == 1 && sprite.energia > 0)
		Play_GameSFX(sprite.tyyppi->aanet[SOUND_RANDOM],80,(int)sprite_x, (int)sprite_y,
					  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);


	// KEHITYSVAIHEEN APUTOIMINTOJA

	u8 color;
	u32 plk;

	if (PInput::Keydown(PInput::B) && dev_mode) { // Draw bounding box
		
		if (i == 0/*pelaaja_index*/) {

			char lukua[50];
			sprintf(lukua, "%i", Game->palikat[1].yla);
			//gcvt(sprite_a,7,lukua);
			PDraw::font_write(fontti1,lukua,310,50);

		}

		if (sprite.tyyppi->tiletarkistus)
			for (x=0;x<palikat_x_lkm;x++) {
				for (y=0;y<palikat_y_lkm;y++) {
					color = 50-x*2-y*2;
					plk = x+y*palikat_x_lkm;

					if (plk > 299)
						plk = 299;

					//if (plk < 0)
					//	plk = 0;

					if (!Game->palikat[plk].tausta)
						color += 32;

					PDraw::screen_fill(
											Game->palikat[plk].vasen-Game->camera_x,
											Game->palikat[plk].yla-Game->camera_y,
											Game->palikat[plk].oikea-Game->camera_x,
											Game->palikat[plk].ala-Game->camera_y,
											color);
				}
			}

		PDraw::screen_fill(
								(int)(sprite_vasen-Game->camera_x),
								(int)(sprite_yla-Game->camera_y),
								(int)(sprite_oikea-Game->camera_x),
								(int)(sprite_ala-Game->camera_y),
								30);

	}



	return 0;
}
int BonusSprite_Movement(int i){
	sprite_x = 0;
	sprite_y = 0;
	sprite_a = 0;
	sprite_b = 0;

	sprite_vasen = 0;
	sprite_oikea = 0;
	sprite_yla = 0;
	sprite_ala = 0;

	sprite_leveys  = 0;
	sprite_korkeus = 0;

	map_vasen = 0;
	map_yla   = 0;

	SpriteClass &sprite = Sprites_List[i];

	sprite_x = sprite.x;
	sprite_y = sprite.y;
	sprite_a = sprite.a;
	sprite_b = sprite.b;

	sprite_leveys  = sprite.tyyppi->leveys;
	sprite_korkeus = sprite.tyyppi->korkeus;

	x = 0;
	y = 0;
	oikealle	= true,
	vasemmalle	= true,
	ylos		= true,
	alas		= true;

	vedessa = sprite.vedessa;

	max_nopeus = (int)sprite.tyyppi->max_nopeus;

	// Siirret��n varsinaiset muuttujat apumuuttujiin.

	sprite_vasen = sprite_x-sprite_leveys/2;
	sprite_oikea = sprite_x+sprite_leveys/2;
	sprite_yla	 = sprite_y-sprite_korkeus/2;
	sprite_ala	 = sprite_y+sprite_korkeus/2;


	if (sprite.isku > 0)
		sprite.isku--;

	if (sprite.lataus > 0)
		sprite.lataus--;

	if (sprite.muutos_ajastin > 0)	// aika muutokseen
		sprite.muutos_ajastin --;

	// Hyppyyn liittyv�t seikat

	if (Game->button_moving + Game->vibration > 0 && sprite.hyppy_ajastin == 0)
		sprite.hyppy_ajastin = sprite.tyyppi->max_hyppy / 2;

	if (sprite.hyppy_ajastin > 0 && sprite.hyppy_ajastin < sprite.tyyppi->max_hyppy)
	{
		sprite.hyppy_ajastin ++;
		sprite_b = (sprite.tyyppi->max_hyppy - sprite.hyppy_ajastin)/-4.0;//-2
	}

	if (sprite_b > 0)
		sprite.hyppy_ajastin = sprite.tyyppi->max_hyppy;



	if (sprite.paino != 0)	// jos bonuksella on paino, tutkitaan ymp�rist�
	{
		// o
		//
		// |  Gravity
		// V

		sprite_b += sprite.paino + sprite_b/1.25;

		if (sprite.vedessa)
		{
			if (sprite_b > 0)
				sprite_b /= 2.0;

			if (rand()%80 == 1)
				Particles_New(PARTICLE_SPARK,sprite_x-4,sprite_y,0,-0.5-rand()%2,rand()%30+30,0,32);
		}

		sprite.vedessa = false;

		sprite.kytkinpaino = sprite.paino;

		/* TOISET SPRITET */

		PK2BLOCK spritepalikka; 

		for (int sprite_index = 0; sprite_index < MAX_SPRITEJA; sprite_index++) {

			SpriteClass* sprite2 = &Sprites_List[sprite_index];
			if (sprite_index != i && !sprite2->piilota) {
				if (sprite2->tyyppi->este && sprite.tyyppi->tiletarkistus) {
					if (sprite_x-sprite_leveys/2 +sprite_a <= sprite2->x + sprite2->tyyppi->leveys /2 &&
						sprite_x+sprite_leveys/2 +sprite_a >= sprite2->x - sprite2->tyyppi->leveys /2 &&
						sprite_y-sprite_korkeus/2+sprite_b <= sprite2->y + sprite2->tyyppi->korkeus/2 &&
						sprite_y+sprite_korkeus/2+sprite_b >= sprite2->y - sprite2->tyyppi->korkeus/2)
					{
						spritepalikka.koodi = 0;
						spritepalikka.ala   = (int)sprite2->y + sprite2->tyyppi->korkeus/2;
						spritepalikka.oikea = (int)sprite2->x + sprite2->tyyppi->leveys/2;
						spritepalikka.vasen = (int)sprite2->x - sprite2->tyyppi->leveys/2;
						spritepalikka.yla   = (int)sprite2->y - sprite2->tyyppi->korkeus/2;

						spritepalikka.alas       = BLOCK_WALL;
						spritepalikka.ylos       = BLOCK_WALL;
						spritepalikka.oikealle   = BLOCK_WALL;
						spritepalikka.vasemmalle = BLOCK_WALL;

						if (!sprite2->tyyppi->este_alas)
							spritepalikka.alas		 = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_ylos)
							spritepalikka.ylos		 = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_oikealle)
							spritepalikka.oikealle   = BLOCK_BACKGROUND;
						if (!sprite2->tyyppi->este_vasemmalle)
							spritepalikka.vasemmalle = BLOCK_BACKGROUND;


						spritepalikka.water  = false;

						Block_Set_Barriers(spritepalikka);
						Check_Blocks2(sprite, spritepalikka); //Colision bonus and sprite block
					}
				}

				if (sprite_x < sprite2->x + sprite2->tyyppi->leveys/2 &&
					sprite_x > sprite2->x - sprite2->tyyppi->leveys/2 &&
					sprite_y < sprite2->y + sprite2->tyyppi->korkeus/2 &&
					sprite_y > sprite2->y - sprite2->tyyppi->korkeus/2 &&
					sprite.isku == 0)
				{
					if (sprite2->tyyppi->tyyppi != TYPE_BONUS &&
						!(sprite2 == Player_Sprite && sprite.tyyppi->tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU))
						sprite_a += sprite2->a*(rand()%4);

					// lis�t��n spriten painoon sit� koskettavan toisen spriten paino
					sprite.kytkinpaino += sprite2->tyyppi->paino;

					// samanmerkkiset spritet vaihtavat suuntaa t�rm�tess��n
					if (sprite.tyyppi->indeksi == sprite2->tyyppi->indeksi &&
						sprite2->energia > 0)
					{
						if (sprite.x < sprite2->x) {
							sprite2->a += sprite.a / 3.0;
							oikealle = false;
						}
						if (sprite.x > sprite2->x) {
							sprite2->a += sprite.a / 3.0;
							vasemmalle = false;
						}
						/*
						if (sprite.y < spritet[sprite_index].y)
							alas = false;
						if (sprite.y > spritet[sprite_index].y)
							ylos = false;*/
					}

				}
			}
		}

		// Tarkistetaan ettei menn� mihink��n suuntaan liian kovaa.

		if (sprite_b > 4)
			sprite_b = 4;

		if (sprite_b < -4)
			sprite_b = -4;

		if (sprite_a > 3)
			sprite_a = 3;

		if (sprite_a < -3)
			sprite_a = -3;

		// Lasketaan

		int palikat_x_lkm = 0,
			palikat_y_lkm = 0;

		if (sprite.tyyppi->tiletarkistus)
		{

			palikat_x_lkm = (int)((sprite_leveys) /32)+4;
			palikat_y_lkm = (int)((sprite_korkeus)/32)+4;

			map_vasen = (int)(sprite_vasen)/32;
			map_yla	 = (int)(sprite_yla)/32;

			for (y=0;y<palikat_y_lkm;y++)
				for (x=0;x<palikat_x_lkm;x++)
				{
					Game->palikat[x+y*palikat_x_lkm] = Block_Get(map_vasen+x-1,map_yla+y-1);
				}

			// Tutkitaan t�rm��k� palikkaan

			for (y=0;y<palikat_y_lkm;y++)
				for (x=0;x<palikat_x_lkm;x++)
					Check_Blocks(sprite, Game->palikat[x+y*palikat_x_lkm]);
			/*
			Check_Blocks_Debug(sprite, palikat[x+y*palikat_x_lkm],
					sprite_x,
					sprite_y,
					sprite_a,
					sprite_b,
					sprite_vasen,
					sprite_oikea,
					sprite_yla,
					sprite_ala,
					sprite_leveys,
					sprite_korkeus,
					map_vasen,
					map_yla,
					oikealle,
					vasemmalle,
					ylos,
					alas);*/


		}

		if (vedessa != sprite.vedessa) {
			Effect_Splash((int)sprite_x,(int)sprite_y,32);
			Play_GameSFX(splash_sound, 100, (int)sprite_x, (int)sprite_y, SOUND_SAMPLERATE, true);
		}


		if (!oikealle)
		{
			if (sprite_a > 0)
				sprite_a = -sprite_a/1.5;
		}

		if (!vasemmalle)
		{
			if (sprite_a < 0)
				sprite_a = -sprite_a/1.5;
		}

		if (!ylos)
		{
			if (sprite_b < 0)
				sprite_b = 0;

			sprite.hyppy_ajastin = sprite.tyyppi->max_hyppy;
		}

		if (!alas)
		{
			if (sprite_b >= 0)
			{
				if (sprite.hyppy_ajastin > 0)
				{
					sprite.hyppy_ajastin = 0;
				//	if (/*sprite_b == 4*/!maassa)
				//		Play_GameSFX(pump_sound,20,(int)sprite_x, (int)sprite_y,
				//				      int(25050-sprite.tyyppi->paino*4000),true);
				}

				if (sprite_b > 2)
					sprite_b = -sprite_b/(3+rand()%2);
				else
					sprite_b = 0;
			}
			//sprite_a /= kitka;
			sprite_a /= 1.07;
		}
		else
		{
			sprite_a /= 1.02;
		}

		sprite_b /= 1.5;

		if (sprite_b > 4)
			sprite_b = 4;

		if (sprite_b < -4)
			sprite_b = -4;

		if (sprite_a > 4)
			sprite_a = 4;

		if (sprite_a < -4)
			sprite_a = -4;

		sprite_x += sprite_a;
		sprite_y += sprite_b;

		sprite.x = sprite_x;
		sprite.y = sprite_y;
		sprite.a = sprite_a;
		sprite.b = sprite_b;

		sprite.oikealle = oikealle;
		sprite.vasemmalle = vasemmalle;
		sprite.alas = alas;
		sprite.ylos = ylos;
	}
	else	// jos spriten paino on nolla, tehd��n spritest� "kelluva"
	{
		sprite.y = sprite.alku_y + cos_table[int(degree+(sprite.alku_x+sprite.alku_y))%360] / 3.0;
		sprite_y = sprite.y;
	}

	sprite.paino = sprite.alkupaino;

	int tuhoutuminen;

	// Test if player touches bonus
	if (sprite_x < Player_Sprite->x + Player_Sprite->tyyppi->leveys/2 &&
		sprite_x > Player_Sprite->x - Player_Sprite->tyyppi->leveys/2 &&
		sprite_y < Player_Sprite->y + Player_Sprite->tyyppi->korkeus/2 &&
		sprite_y > Player_Sprite->y - Player_Sprite->tyyppi->korkeus/2 &&
		sprite.isku == 0)
	{
		if (sprite.energia > 0 && Player_Sprite->energia > 0)
		{
			if (sprite.tyyppi->pisteet != 0) {

				char* name = sprite.tyyppi->nimi;
				if (strcmp(name, "big apple") == 0 || strcmp(name, "big apple 2") == 0)
					Game->apples_got++;

				Game->score_increment += sprite.tyyppi->pisteet;
				
				if (!sprite.Onko_AI(AI_BONUS_AIKA)) {

					char luku[10];
					sprintf(luku, "%i", sprite.tyyppi->pisteet);
					
					if (sprite.tyyppi->pisteet >= 50)
						Fadetext_New(fontti2,luku,(int)sprite.x-8,(int)sprite.y-8,100);
					else
						Fadetext_New(fontti1,luku,(int)sprite.x-8,(int)sprite.y-8,100);

				}

			}

			if (sprite.Onko_AI(AI_BONUS_AIKA)) {
				
				float increase_time = sprite.tyyppi->latausaika;

				Game->increase_time += increase_time;

				increase_time *= float(TIME_FPS) / 60;
                int min = int(increase_time / 60);
                int sek = int(increase_time) % 60;
				
				char min_c[8], sek_c[8];
				sprintf(min_c, "%i", min);
				sprintf(sek_c, "%i", sek);

				char luku[8];
				strcpy(luku, min_c);
				strcat(luku, ":");
                if (sek < 10)
                    strcat(luku, "0");
				strcat(luku, sek_c);
				Fadetext_New(fontti1,luku,(int)sprite.x-15,(int)sprite.y-8,100);
				
			}

			if (sprite.Onko_AI(AI_BONUS_NAKYMATTOMYYS))
				Player_Sprite->invisible = sprite.tyyppi->latausaika;

			//Game->map->spritet[(int)(sprite.alku_x/32) + (int)(sprite.alku_y/32)*PK2KARTTA_KARTTA_LEVEYS] = 255;

			if (sprite.tyyppi->tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU)
				Player_Sprite->energia -= sprite.tyyppi->vahinko;

			tuhoutuminen = sprite.tyyppi->tuhoutuminen;

			if (tuhoutuminen != FX_DESTRUCT_EI_TUHOUDU)
			{
				if (tuhoutuminen >= FX_DESTRUCT_ANIMAATIO)
					tuhoutuminen -= FX_DESTRUCT_ANIMAATIO;
				else
				{
					if (sprite.tyyppi->avain)
					{
						Game->keys--;

						if (Game->keys < 1)
							Game->map->Open_Locks();
					}

					sprite.piilota = true;
				}

				if (sprite.Onko_AI(AI_UUSI_JOS_TUHOUTUU)) {
					double ax, ay;
					ax = sprite.alku_x;//-sprite.tyyppi->leveys;
					ay = sprite.alku_y-sprite.tyyppi->korkeus/2.0;
					Sprites_add(sprite.tyyppi->indeksi,0,ax-17, ay,i, false);
					for (int r=1;r<6;r++)
						Particles_New(PARTICLE_SPARK,ax+rand()%10-rand()%10, ay+rand()%10-rand()%10,0,0,rand()%100,0.1,32);

				}

				if (sprite.tyyppi->bonus != -1)
					if (Gifts_Add(sprite.tyyppi->bonus))
						Game->Show_Info(tekstit->Get_Text(PK_txt.game_newitem));

				if (sprite.tyyppi->muutos != -1)
				{
					if (Prototypes_List[sprite.tyyppi->muutos].AI[0] != AI_BONUS)
					{
						Player_Sprite->tyyppi = &Prototypes_List[sprite.tyyppi->muutos];
						Player_Sprite->ammus1 = Player_Sprite->tyyppi->ammus1;
						Player_Sprite->ammus2 = Player_Sprite->tyyppi->ammus2;
						Player_Sprite->alkupaino = Player_Sprite->tyyppi->paino;
						Player_Sprite->y -= Player_Sprite->tyyppi->korkeus/2;
						//Game->Show_Info("pekka has been transformed!");
					}
				}

				if (sprite.tyyppi->ammus1 != -1)
				{
					Player_Sprite->ammus1 = sprite.tyyppi->ammus1;
					Game->Show_Info(tekstit->Get_Text(PK_txt.game_newegg));
				}

				if (sprite.tyyppi->ammus2 != -1)
				{
					Player_Sprite->ammus2 = sprite.tyyppi->ammus2;
					Game->Show_Info(tekstit->Get_Text(PK_txt.game_newdoodle));
				}

				Play_GameSFX(sprite.tyyppi->aanet[SOUND_DESTRUCTION],100, (int)sprite.x, (int)sprite.y,
							  sprite.tyyppi->aani_frq, sprite.tyyppi->random_frq);

				Effect_Destruction(tuhoutuminen, (u32)sprite_x, (u32)sprite_y);
			}
		}
	}

	for (i=0;i<SPRITE_MAX_AI;i++) {

		if (sprite.tyyppi->AI[i] == AI_NONE)
			break;

		switch (sprite.tyyppi->AI[i]) {
		
		case AI_BONUS:				sprite.AI_Bonus(); break;

		case AI_BASIC:				sprite.AI_Perus(); break;

		case AI_MUUTOS_AJASTIN:		if (sprite.tyyppi->muutos > -1)
										sprite.AI_Muutos_Ajastin(Prototypes_List[sprite.tyyppi->muutos]);
									break;

		case AI_TIPPUU_TARINASTA:	sprite.AI_Tippuu_Tarinasta(Game->vibration + Game->button_moving);
									break;

		default:					break;
		
		}
	}

	/* The energy doesn't matter that the player is a bonus item */
	if (sprite.pelaaja != 0)
		sprite.energia = 0;

	return 0;
}

