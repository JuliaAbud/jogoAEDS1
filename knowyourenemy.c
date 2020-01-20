#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

const int FPS = 60;
const int SCREEN_W = 800;
const int SCREEN_H = 600;

//funções-------------------------------------------------------------------------------------------------------------------------------
int collision (float playerx, float playery, float playerH, float playerL, float colisaox, float colisaoy, float colisaoH, float colisaoL){
	if (((colisaox + colisaoL) > playerx && colisaox < (playerx + playerL)) && ((colisaoy + colisaoH) > playery&& colisaoy < (playery+ playerH)))
		return 1;
	else
		return 0;
}


void enemyChase (float *enemyx, float *enemyy, float x, float y){
	if (x < *enemyx)
		*enemyx -= 0.5;
	else if (x > *enemyx)
		*enemyx += 0.5;
	if (y < *enemyy)
		*enemyy -= 0.5;
	else if (y > *enemyy)
		*enemyy += 0.5;
}

int main(int argc, char **argv){
//variaveis do allegro-----------------------------------------------------------------------------------------------------------
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *player = NULL;
	ALLEGRO_BITMAP *enemy1 = NULL;
	ALLEGRO_BITMAP *enemy2 = NULL;
	ALLEGRO_BITMAP *enemy3 = NULL;
	ALLEGRO_BITMAP *bomb[3];
	ALLEGRO_BITMAP *fundo = NULL;
	ALLEGRO_BITMAP *fundoPerdeu = NULL;
	ALLEGRO_BITMAP *fundoGanhou = NULL;
	ALLEGRO_BITMAP *fundoMenu = NULL;
	ALLEGRO_BITMAP *fundoTutorial = NULL;
	ALLEGRO_BITMAP *fundoHighscore = NULL;
	ALLEGRO_SAMPLE *musicaJogo = NULL;
	ALLEGRO_SAMPLE_INSTANCE *mainSong = NULL;
	ALLEGRO_SAMPLE *musicaFinal = NULL;
	ALLEGRO_SAMPLE_INSTANCE *ending = NULL;
	ALLEGRO_SAMPLE *musicaWin = NULL;
	ALLEGRO_SAMPLE_INSTANCE *win = NULL;
	ALLEGRO_FONT *txt = NULL;
	ALLEGRO_FONT *txt2 = NULL;
   
  //variaveis do jogo------------------------------------------------------------------------------------------------------------ 
	int playing = 1;
	int i,j;
	int sizePlayerH = 35, sizePlayerL = 30;
	int sizeEnemyH = 30, sizeEnemyL = 30;
	int sizeBomb = 15, sizeBombRaio = 60, sizeBombRaio2 = 60;
	int sizeBrick = 50;
	int countBomb = 0;
	int countTime[3];
	for (j=0;j<3;j++){
		countTime[j] = 0;
	}
	int release[3];
	for(j=0; j<3; j++){
		release[j] = 1;
	}
	char texto [20], texto2[20];
	float pontos = 1;
	char pontosMaior1[20];
	int checkKill1 = 0, checkKill2 = 0, checkKill3 = 0;
	bool menu = true;
	bool start = false;
	bool tutorial = false;
	bool highscore = false;
	FILE *arq;
	float score;
	

 //posicoes----------------------------------------------------------------------------------------------------------------------
   float player_x = (rand()%690)+sizePlayerL;
   float player_y = (rand()%490)+sizePlayerL;
   float player_dx = 5.0;
   float player_dy = 5.0;
   float e1x = (rand()%690)+sizeEnemyL;
   float e1y = (rand()%490)+sizeEnemyL;
   float e1dx = 0.5;
   float e1dy = 0.5;
   float e2x = 500;
   float e2y = 400;
   float e2dx = 0.3;
   float e2dy = 0.3;
   float e3x = (rand()%690)+sizeEnemyL;
   float e3y = (rand()%490)+sizeEnemyL;
   float bomb_x[3];
   float bomb_y[3];
   for(j=0;j<3;j++){
	   bomb_x[j]=2000;
	   bomb_y[j]=2000;
   }	
	
//inicializacao ----------------------------------------------------------------------------------------------
	al_init();
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
    }
 
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
    }
 
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
    }
 
	//bitmaps do background
	al_init_image_addon();
	fundo = al_load_bitmap("Imagens/background.png");
	if(!fundo) {
		fprintf(stderr, "failed to create fundo bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
    }
	fundoPerdeu = al_load_bitmap("Imagens/perdeutela.png");
	if(!fundoPerdeu) {
		fprintf(stderr, "failed to create fundoPerdeu bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
    }
	
	fundoGanhou = al_load_bitmap("Imagens/ganhoutela.png");
	if(!fundoGanhou) {
		fprintf(stderr, "failed to create fundoGanhou bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
    }
	fundoMenu = al_load_bitmap("Imagens/menu.png");
	if(!fundoMenu) {
		fprintf(stderr, "failed to create fundoMenu bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
    }
	fundoTutorial = al_load_bitmap("Imagens/tutorial.png");
	if(!fundoMenu) {
		fprintf(stderr, "failed to create fundoTutorial bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
    }
	fundoHighscore = al_load_bitmap("Imagens/recorde.png");
	if(!fundoMenu) {
		fprintf(stderr, "failed to create fundoHighscore bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
    }
	//bitmaps do personagens
	
	player = al_load_bitmap("Imagens/playerfront.png");
    if(!player) {
      fprintf(stderr, "failed to create player bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
   
    enemy1 = al_load_bitmap("Imagens/enemy1.png");
    if(!enemy1){
		fprintf(stderr, "failed to create enemy1 bitmap!\n");
		al_destroy_bitmap(player);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
   
    enemy2 = al_load_bitmap("Imagens/enemy1.png");
    if(!enemy2){
		fprintf(stderr, "failed to create enemy2 bitmap!\n");
		al_destroy_bitmap(player);
		al_destroy_display(display);
		al_destroy_timer(timer);
		al_destroy_bitmap(enemy1);
		return -1;
	}
	
	enemy3 = al_load_bitmap("Imagens/enemy2.png");
	if (!enemy3){
	fprintf(stderr, "failed to create enemy3!\n");
    al_destroy_bitmap(player);
	al_destroy_display(display);
    al_destroy_timer(timer);
	al_destroy_bitmap(enemy1);
	al_destroy_bitmap(enemy2);
	return -1;
	}
	for(i=0;i<3;i++){
	bomb[i] = al_load_bitmap("Imagens/imageBomb.png");
    }
   
   
//musica---------------------------------------------------------------------------------------------------------
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(50);
	
	musicaJogo = al_load_sample("Som/musicaJogo.ogg");
	mainSong = al_create_sample_instance(musicaJogo);
	al_set_sample_instance_playmode(mainSong, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(mainSong, al_get_default_mixer());
	
	musicaFinal = al_load_sample("Som/end.ogg");
	ending = al_create_sample_instance(musicaFinal);
	al_set_sample_instance_playmode(ending, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(ending, al_get_default_mixer());
	
	musicaWin = al_load_sample("Som/win.ogg");
	win = al_create_sample_instance(musicaWin);
	al_set_sample_instance_playmode(win, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(win, al_get_default_mixer());
	

//eventos-------------------------------------------------------------------------------------------------------- 
   al_install_keyboard();
   
   al_set_target_bitmap(al_get_backbuffer(display));
   al_clear_to_color(al_map_rgb(0,0,0));   
   
   event_queue = al_create_event_queue();
   
    if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(player);
      al_destroy_display(display);
      al_destroy_timer(timer);
	  al_destroy_bitmap(enemy1);
	  al_destroy_bitmap(enemy2);
      return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());   
    al_flip_display();
    al_start_timer(timer);
	//highscore------------------------------------------------------------------------------------------------------
	arq = fopen("highscore.txt", "r");
	fscanf(arq, "%f", &score);
	fclose(arq);

	al_init_font_addon();
	al_init_ttf_addon();
	txt2 = al_load_font("8bitoperator.ttf", 36, 1);
	sprintf(texto2, "%.0f", score);

//JOGO==============================================================================================================
  
	while(playing == 1) {	
		
		while (menu){
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
	
			al_draw_bitmap(fundoMenu,0,0,0);
			al_flip_display();
			
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				switch (ev.keyboard.keycode){
					case ALLEGRO_KEY_ENTER:
					menu = false;
					start = true;
					break;
					case ALLEGRO_KEY_Z:
					menu = false;
					tutorial = true;
					break;
					case ALLEGRO_KEY_H:
					menu = false;
					highscore = true;
				    
				}		
			}
		}
		while (tutorial){
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
		
			al_init_font_addon();
			al_init_ttf_addon();
			al_draw_bitmap(fundoTutorial,0,0,0);
			al_flip_display();
			
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				switch (ev.keyboard.keycode){
					case ALLEGRO_KEY_BACKSPACE:
					tutorial = false;
					menu = true;
					break;
					case ALLEGRO_KEY_ENTER:
					tutorial = false;
					start = true;
					break;
				}
			}	
		}
		while (highscore){
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
			
			al_draw_bitmap(fundoHighscore,0,0,0);
			al_draw_text(txt2, al_map_rgb(255,255,255), 250, 275, 0, texto2);
			al_flip_display();
		
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				switch (ev.keyboard.keycode){
					case ALLEGRO_KEY_BACKSPACE:
					highscore = false;
					menu = true;
					break;
					case ALLEGRO_KEY_ENTER:
					highscore = false;
					start = true;
					break;
				}
			}	
		}
		
		al_flip_display();
		while (start){
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
		
				if (ev.type == ALLEGRO_EVENT_KEY_CHAR){
				
					switch(ev.keyboard.keycode) {
					
						//movimentaçao do player
						case ALLEGRO_KEY_UP:
							al_destroy_bitmap(player);
							player = al_load_bitmap("Imagens/playerback.png");
							if(player_y > (sizePlayerH+20)) 
							player_y -= player_dy;
							break;
				
						case ALLEGRO_KEY_DOWN:
							al_destroy_bitmap(player);
							player = al_load_bitmap("Imagens/playerfront.png");
							if(player_y < SCREEN_H - (sizePlayerH+55))			
							player_y += player_dy;
							break;
	
						case ALLEGRO_KEY_LEFT:
							al_destroy_bitmap(player);
							player = al_load_bitmap("Imagens/playerleft.png");
							if(player_x > sizePlayerL+25) 
							player_x -= player_dx;
							break;
	
						case ALLEGRO_KEY_RIGHT:
							al_destroy_bitmap(player);
							player = al_load_bitmap("Imagens/playerright.png");
							if(player_x < SCREEN_W - (sizePlayerL+sizeBrick)) 
							player_x += player_dx;
							break;
					}
				}
		
				if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				
					switch(ev.keyboard.keycode) {

					//esc para sair do jogo
						case ALLEGRO_KEY_ESCAPE:
							playing = 3;
							start = false;
							break;				
					
						//depositar a bomba
						case ALLEGRO_KEY_SPACE: 
							countTime[countBomb] = 0;
							if (release[countBomb] == 1){
								bomb_x[countBomb] = player_x+7.5;
								bomb_y[countBomb] = player_y;
								release[countBomb] = 0;
							}
							countBomb++;
							if (countBomb == 3)
								countBomb = 0;
							break;
					}
				}
			
				//evento timer------------------------------------------------------------------------------------------------------------------------	
				if(ev.type == ALLEGRO_EVENT_TIMER){
					
					if(!menu)
					al_play_sample_instance(mainSong);
					
					//contagem de pontos
					pontos++;
					
					//movimentaçao dos enemies
					e1x -= e1dx;
					e1y -= e1dy;			
					e2x += e2dx;
					e2y += e2dy;
					//movimentaçao do enemy que persegue
					enemyChase(&e3x, &e3y, player_x, player_y);
						
					//colisao com a parede dos enemies
					if (e1x < sizeBrick || e1x > SCREEN_W - (sizeEnemyL+sizeBrick))
						e1dx = -e1dx;
					if (e1y < sizeBrick || e1y > SCREEN_H - (sizeEnemyL+sizeBrick))
						e1dy = -e1dy;
					if (e2x < sizeBrick || e2x > SCREEN_W - (sizeEnemyL+sizeBrick))
						e2dx = -e2dx;
					if (e2y < sizeBrick || e2y > SCREEN_H - (sizeEnemyL+sizeBrick))
						e2dy = -e2dy;
					
					//colisao do player com os enemies
					if (collision(player_x, player_y, sizePlayerH, sizePlayerL, e1x, e1y, sizeEnemyH, sizeEnemyL)){
						playing = 0;
						start = false;
					}
					if (collision(player_x, player_y, sizePlayerH, sizePlayerL, e2x, e2y, sizeEnemyH, sizeEnemyL)){
						playing = 0;
						start = false;
					}
					if (collision(player_x, player_y, sizePlayerH, sizePlayerL, e3x, e3y, sizeEnemyH, sizeEnemyL)){
						playing = 0;
						start = false;
					}
		
					//criando os bitmaps
					al_draw_bitmap(fundo, 0, 0, 0);
					al_draw_bitmap(player, player_x, player_y, 0);
					al_draw_bitmap(enemy1, e1x, e1y, 0);
					al_draw_bitmap(enemy2, e2x, e2y, 0);
					al_draw_bitmap(enemy3, e3x, e3y, 0);
					
					for(i=0;i<3;i++){
						al_draw_bitmap(bomb[i], bomb_x[i], bomb_y[i], 0);
					}
					
					//colisao bombas
					if (collision(player_x, player_y, sizePlayerH, sizePlayerL, bomb_x[0], bomb_y[0], sizeBombRaio, sizeBombRaio2)&& countTime[0] == 178){
						//playing = 0;
						//start = false;
						
					}
					if (collision(e1x, e1y, sizeEnemyH, sizeEnemyL, bomb_x[0], bomb_y[0], sizeBombRaio, sizeBombRaio2)&&countTime[0] == 178){
						e1x = 2000;
						e1y = 2000;
						checkKill1 = 1;
					}
					if (collision(e2x, e2y, sizeEnemyH, sizeEnemyL, bomb_x[0], bomb_y[0], sizeBombRaio, sizeBombRaio2)&&countTime[0] == 178){
						e2x = 2000;
						e2y = 2000;
						checkKill2 = 1;
					}
					if (collision(e3x, e3y, sizeEnemyH, sizeEnemyL, bomb_x[0], bomb_y[0], sizeBombRaio, sizeBombRaio2)&&countTime[0] == 178){
						e3x = 2000;
						e3y = 2000;
						checkKill3 = 1;
					}
					if (collision(player_x, player_y, sizePlayerH, sizePlayerL, bomb_x[1], bomb_y[1], sizeBombRaio, sizeBombRaio2)&& countTime[1] == 178){
						playing = 0;
						start = false;
						
					}
					if (collision(e1x, e1y, sizeEnemyH, sizeEnemyL, bomb_x[1], bomb_y[1], sizeBombRaio, sizeBombRaio2)&&countTime[1] == 178){
						e1x = 2000;
						e1y = 2000;
						checkKill1 = 1;
					}
					if (collision(e2x, e2y, sizeEnemyH, sizeEnemyL, bomb_x[1], bomb_y[1], sizeBombRaio, sizeBombRaio2)&&countTime[1] == 178){
						e2x = 2000;
						e2y = 2000;
						checkKill2 = 1;
					}
					if (collision(e3x, e3y, sizeEnemyH, sizeEnemyL, bomb_x[1], bomb_y[1], sizeBombRaio, sizeBombRaio2)&&countTime[1] == 178){
						e3x = 2000;
						e3y = 2000;
						checkKill3 = 1;
					}
					if (collision(player_x, player_y, sizePlayerH, sizePlayerL, bomb_x[2], bomb_y[2], sizeBombRaio, sizeBombRaio2)&& countTime[2] == 178){
						playing = 0;
						start = false;
						
					}
					if (collision(e1x, e1y, sizeEnemyH, sizeEnemyL, bomb_x[2], bomb_y[2], sizeBombRaio, sizeBombRaio2)&&countTime[2] == 178){
						e1x = 2000;
						e1y = 2000;
						checkKill1 = 1;
					}
					if (collision(e2x, e2y, sizeEnemyH, sizeEnemyL, bomb_x[2], bomb_y[2], sizeBombRaio, sizeBombRaio2)&&countTime[2] == 178){
						e2x = 2000;
						e2y = 2000;
						checkKill2 = 1;
					}
					if (collision(e3x, e3y, sizeEnemyH, sizeEnemyL, bomb_x[2], bomb_y[2], sizeBombRaio, sizeBombRaio2)&&countTime[2] == 178){
						e3x = 2000;
						e3y = 2000;
						checkKill3 = 1;
					}
					
					//relogio da bomba
					if(countTime[0] < 180)
						countTime[0]++;

					if(countTime[1] < 180)
						countTime[1]++;
					
					if(countTime[2] < 180)
						countTime[2]++;
					
					else{
						for (j=0;j<3;j++){
							countTime[j]=0;
						}
					}
						
					if (countTime[0] == 179){
						bomb_x[0] = 2000;
						bomb_y[0] = 2000;
						release[0] = 1;
					}
						
					if (countTime[1] == 179){
						bomb_x[1] = 2000;
						bomb_y[1] = 2000;
						release[1] = 1;
					}	
						
					if (countTime[2] == 179){
						bomb_x[2] = 2000;
						bomb_y[2] = 2000;
						release[2] = 1;
					}
				//checa se todos os inimigos morreram
				if (checkKill1 == 1 && checkKill2 == 1 && checkKill3 ==1){
					playing = 2;
					start = false;	
				}
			}
				
			//fecha a tela
			if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
				playing = 3;
				start = false;
				break;
			}	
			al_flip_display();
		}
		//fim do start
	}

//FIM DO WHILE========================================================================================================================================	
	
	//contagem de pontos
	pontos = (100000/pontos)*1000;

	//atualização do highscore
	if(pontos > score && playing == 2){
		arq = fopen("highscore.txt", "w");
		score = pontos;
		fprintf(arq, "%.0f", score);
		fclose(arq);
	}
	
	//finalização
	al_stop_sample_instance(mainSong);
	al_init_font_addon();
	al_init_ttf_addon();
	txt = al_load_font("8bitoperator.ttf", 36, 1);

	if (playing == 0){
		al_clear_to_color(al_map_rgb(0,0,0));
		al_flip_display();
		al_draw_bitmap(fundoPerdeu,0,0,0);
		al_flip_display();
		al_play_sample_instance(ending);
		al_rest(4);		
	}

	if (playing == 2) {
		al_clear_to_color(al_map_rgb(0,0,0));
		al_flip_display();
		al_draw_bitmap(fundoGanhou,0,0,0);
		sprintf(texto, "%.0f pontos", pontos);
		al_draw_text(txt, al_map_rgb(255,140,0), 250, 400, 0, texto);
		al_flip_display();
		al_play_sample_instance(win);	
		al_rest(4);		
	}

	if (playing == 3)
		;
		
	//destroys
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_bitmap(player);
	al_destroy_bitmap(enemy1);
	al_destroy_bitmap(enemy2);
	al_destroy_bitmap(enemy3);
	for (i=0;i<3;i++){
		al_destroy_bitmap(bomb[i]);
	}
	al_destroy_bitmap(fundo);
	al_destroy_bitmap(fundoPerdeu);
	al_destroy_bitmap(fundoGanhou);
	al_destroy_bitmap(fundoMenu);
	al_destroy_bitmap(fundoTutorial);
	al_destroy_sample(musicaJogo);
	al_destroy_sample_instance(mainSong);
	al_destroy_sample(musicaFinal);
	al_destroy_sample_instance(ending);
	al_destroy_sample(musicaWin);
	al_destroy_sample_instance(win);
	
	return 0;
}

