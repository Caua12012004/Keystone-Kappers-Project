#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define NUM_ANDARES 3
#define NUM_SALAS 3
#define COP_VEL 3
#define GRAVIDADE 0.1
#define JUMP_VEL 3
#define DESCONTO_VEL_LAMA 0.15
#define TRUE 1
#define FALSE 0
#define MAX_JUMPS 2.\


typedef struct Pessoa {
	float x, x_mall, y, yvel;
	int x_esq;
	int x_dir;
	float vel;
	int sala;
	int andar;
	int pulando;
	int is_cop;
	int numero_pulos;
	ALLEGRO_COLOR cor;
} Pessoa;

typedef struct Mud {
	int x, y, w, x_mall;
} Mud;

const int PESSOA_W = 30;
const int PESSOA_H = 50;

const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int PISO_H = 30;
const int MAX_POCAS = 9;

int score = 0;

//------------ Global Vars ------------------
ALLEGRO_COLOR BKG_COLOR;
ALLEGRO_FONT *size_32;  	
int FLOOR_H;
int MALL_W;


void init_global_vars() {
	BKG_COLOR = al_map_rgb(15,15,15);
	size_32 = al_load_font("arial.ttf", 32, 1);   	
	MALL_W = SCREEN_W * NUM_SALAS;
	FLOOR_H = SCREEN_H / NUM_ANDARES;
}
//-------------------------------------------

void initCop(Pessoa *cop) {
	cop->cor = al_map_rgb(102,178,255);
	cop->x = PESSOA_W;
	cop->y = SCREEN_H - PISO_H;
	cop->x_mall = cop->x;
	cop->x_dir = 0;
	cop->x_esq = 0;
	cop->vel = COP_VEL;	
	cop->sala = 0;
	cop->andar = 0;
	cop->pulando = 0;
	cop->numero_pulos = 0;
	cop->yvel = JUMP_VEL;
	cop->is_cop = TRUE;
}

void initThief(Pessoa *thief) {
	thief->cor = al_map_rgb(255,255,255);
	thief->x = PESSOA_W;
	thief->y = SCREEN_H - (1*(SCREEN_H/NUM_ANDARES)) - PISO_H;
	thief->x_dir = 0;
	thief->x_esq = 0;
	thief->pulando = 0;
	thief->yvel = 0;
	thief->vel = (float) COP_VEL/2.5;

	thief->sala = (int) NUM_SALAS/2;

		if (NUM_ANDARES%2 == 0) 
			thief->andar = (int) NUM_ANDARES/2 - 1;
		else 
			thief->andar = (int) NUM_ANDARES/2;

	thief->x_mall = SCREEN_W*(thief->sala + 1) - PESSOA_W;
	thief->is_cop = FALSE;
}

int colisaoCopThief(Pessoa cop, Pessoa thief) {

	if ((cop.x + PESSOA_H >= thief.x) && (cop.x + PESSOA_H <= thief.x + PESSOA_H)
		&& (cop.x_dir == 1) && (cop.y == thief.y) && (cop.sala == thief.sala)) {
		printf("\nO policial prendeu o ladrao !");
		return TRUE;
	}

	else if ((cop.x - PESSOA_H <= thief.x) && (cop.x + PESSOA_H >= thief.x - PESSOA_H)
		&& (cop.x_esq == 1) && (cop.y == thief.y)  && (cop.sala == thief.sala)) {
		printf("\nO policial prendeu o ladrao !");
		return TRUE;
	}

	else if ((cop.x + PESSOA_H >= thief.x) && (cop.x + PESSOA_H <= thief.x + PESSOA_H)
		&& (cop.x_dir == 1) && (cop.y == thief.y - PESSOA_W/2) && (cop.sala == thief.sala)) {
		printf("\nO policial prendeu o ladrao !");
		return TRUE;
	}

	else if ((cop.x - PESSOA_H <= thief.x) && (cop.x + PESSOA_H >= thief.x - PESSOA_H)
		&& (cop.x_esq == 1) && (cop.y == thief.y - PESSOA_W/2)  && (cop.sala == thief.sala)) {
		printf("\nO policial prendeu o ladrao !");
		return TRUE;
	}

	return FALSE;
}

int fuga_ladrao(Pessoa thief, int *ladrao_venceu) {

	if (thief.sala == NUM_SALAS - 1 && thief.andar == NUM_ANDARES - 1 
		&& thief.x + PESSOA_H >= SCREEN_W && thief.x_dir == 1) {
		*ladrao_venceu = TRUE;
		return TRUE;
	}

	if (thief.sala == 0 && thief.andar == NUM_ANDARES - 1 
		&& thief.x - PESSOA_H <= 0 && thief.x_esq == 1) {
		*ladrao_venceu = TRUE;
		return TRUE;
	}

	return FALSE;
}

int random (int min, int max) {

	return min + rand()%(max - min + 1);

}



void desenha_cenario(Pessoa cop, Mud obst[][NUM_ANDARES]) {

   al_clear_to_color(BKG_COLOR);   

   int delta_cor = cop.sala* (255/NUM_SALAS) ;

   int i, y_andar;
   for(i=0; i<NUM_ANDARES; i++) {
   	y_andar = SCREEN_H - (i*(SCREEN_H/NUM_ANDARES));
   	al_draw_filled_rectangle(0, y_andar, SCREEN_W, y_andar - PISO_H, al_map_rgb(0 + delta_cor ,255 - delta_cor ,0 + delta_cor)); 
   }

   char text_score[4];
	sprintf(text_score, "%d", score);
	al_draw_text(size_32, al_map_rgb(102,178,255), SCREEN_W - 70, 20, 0, text_score);   


	//desenha as pocas de lama:
	//for...

	int j, x, aux;

	for(i=0; i<NUM_SALAS; i++) {
		for(j=0; j<NUM_ANDARES; j++) {

			x = obst[cop.sala][j].x;
			al_draw_filled_rectangle(x, obst[cop.sala][j].y, x+ obst[cop.sala][j].w, obst[cop.sala][j].y - PISO_H, al_map_rgb(153, 76, 0)); 

		}
	}

}

void desenha_pessoa(Pessoa p, Pessoa *cop) {

	if (p.is_cop == TRUE) {
		if(p.x_dir == 0 && p.x_esq == 0) {
		al_draw_filled_triangle(p.x - PESSOA_W/2.0, p.y, 
							p.x + PESSOA_W/2.0, p.y,
							p.x, p.y - PESSOA_H,
							p.cor);
		}
		else {
		al_draw_filled_triangle(p.x, p.y, 
							p.x, p.y - PESSOA_W,
							p.x + PESSOA_H * pow(-1, 2*p.x_dir + p.x_esq), p.y - PESSOA_W/2.0,
							p.cor);
		}
	}

	else if (p.is_cop == FALSE && p.sala == cop->sala) {
		if(p.x_dir == 0 && p.x_esq == 0) {
		al_draw_filled_triangle(p.x - PESSOA_W/2.0, p.y, 
							p.x + PESSOA_W/2.0, p.y,
							p.x, p.y - PESSOA_H,
							p.cor);
		}
		else {
		al_draw_filled_triangle(p.x, p.y, 
							p.x, p.y - PESSOA_W,
							p.x + PESSOA_H * pow(-1, 2*p.x_dir + p.x_esq), p.y - PESSOA_W/2.0,
							p.cor);
		}
	}
	
}

void elevador(Pessoa *p) {

	printf("\nUma pessoa chegou no elevador");

	if (p->andar % 2 == 0 && p->x_mall - PESSOA_H <= 0 && p->andar > 0) {
	
		p->andar -= 1;   
		p->x_esq = FALSE;
		p->pulando = FALSE;

	}

	else if (p->andar %2 == 0 && p->x_mall + PESSOA_H >= MALL_W) {
		
		p->andar += 1;
		p->x_dir = FALSE;
		p->pulando = FALSE;

	}

	else if (p->andar % 2 != 0 && p->x_mall - PESSOA_H <= 0) {
		
		p->andar += 1;
		p->x_esq = FALSE;
		p->pulando = FALSE;
	}

	else if (p->andar %2 != 0 && p->x_mall + PESSOA_H >= MALL_W) {
		
		p->andar -= 1;
		p->x_dir = FALSE;
		p->pulando = FALSE;

	}

}

int colisaoLama(Pessoa *p, Mud obst[][NUM_ANDARES]) {

	int i, j;

	for(i=0; i<NUM_SALAS; i++) {
			for(j=0; j<NUM_ANDARES; j++) {
			
			if (p->x_mall>= obst[i][j].x_mall && p->x_mall <= obst[i][j].x_mall + obst[i][j].w
				&& p->y == obst[i][j].y - PESSOA_W) {
					printf("\nO policial esta na lama!");
					return TRUE;
				}


			}
	
	}

	return FALSE;
	
}

void pulo(Pessoa *p, float y_base) {

	p->yvel -= GRAVIDADE;
	p->y -= p->yvel;

	if (p->y >= y_base) {
		p->y = 0;
		p->yvel = 0;
		p->pulando = FALSE;
		p->numero_pulos = 0;
	}
	
}

void update_person(Pessoa *p, Mud obst[][NUM_ANDARES]) {

	if (p->is_cop == TRUE) {
		int y_base = SCREEN_H - (p->andar)*FLOOR_H - PISO_H;

		if(p->x_esq) {

			if (colisaoLama(p, obst) == TRUE){
				p->x_mall -= (p->vel)*DESCONTO_VEL_LAMA;
				p->pulando = FALSE;
				p->numero_pulos = 0;
			}
			else if(p->x_mall - PESSOA_H > 0)
				p->x_mall -= (p->vel);
			else {
				elevador(p);

			}

		}

		if(p->x_dir) {

			if (colisaoLama(p, obst) == TRUE) {
				p->x_mall += (p->vel)*DESCONTO_VEL_LAMA;
				p->pulando = FALSE;
				p->numero_pulos = 0;
			}
			else if(p->x_mall + PESSOA_H < MALL_W)
				p->x_mall += (p->vel);
			else {
				elevador(p);
			}

		}

		if (p->pulando == TRUE && p->numero_pulos <= MAX_JUMPS) {
			pulo(p, y_base);
		}
		else 
			p->y = SCREEN_H - (p->andar)*FLOOR_H - PISO_H;

		p->x = ((int)p->x_mall) % SCREEN_W;

		p->sala = p->x_mall / SCREEN_W;

	}

	else if (p->is_cop == FALSE) {

		int y_base = SCREEN_H - (p->andar)*FLOOR_H - PISO_H;

		if (p->andar % 2 == 0) p->x_dir = 1;
		else p->x_esq = 1;

		if(p->x_esq) {

			if(p->x_mall - PESSOA_H > 0)
				p->x_mall -= (p->vel);
			else {
				elevador(p);
			}

		}

		if(p->x_dir) {

			if(p->x_mall + PESSOA_H < MALL_W)
				p->x_mall += (p->vel);
			else {
				elevador(p);
			}

		}

		p->x = ((int)p->x_mall) % SCREEN_W;

		p->sala = p->x_mall / SCREEN_W;

		p->y = SCREEN_H - (p->andar)*FLOOR_H - PISO_H;

	}
}

 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	int i, j;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
   
   //inicializa o modulo que permite carregar imagens no jogo
	al_init_image_addon();
	
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
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


	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
	al_init_ttf_addon();


	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    //ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   	

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
   
	al_install_keyboard();   

	//registra na fila de eventos que eu quero identificar quando a tela foi alterada
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila de eventos que eu quero identificar quando eventos de teclado
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	
	//---------------- Global vars ------------
	init_global_vars();
	//-----------------------------------------
		
	//---------------- Pessoa -------------------
	Pessoa cop, thief;
	initCop(&cop);
	initThief(&thief);
	//-----------------------------------------

	//----------------- Obstacles -------------	
	Mud obst[NUM_SALAS][NUM_ANDARES];

	for(i=0; i<NUM_SALAS; i++) {
		for(j=0; j<NUM_ANDARES; j++) {

			//cria uma poça de lama;
			obst[i][j].x = random(90, SCREEN_W - 400); 

			obst[i][j].x_mall = obst[i][j].x + i*(SCREEN_W);

			obst[i][j].y = SCREEN_H - (j*(SCREEN_H/NUM_ANDARES));

			obst[i][j].w = random(120, 210);

		}
	}


	//-----------------------------------------

	int playing = TRUE, ladrao_venceu = FALSE;

	//inicia o temporizador
	al_start_timer(timer);





	
	ALLEGRO_EVENT final_event;
	//enquanto playing for verdadeiro, faca:
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);


		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			desenha_cenario(cop, obst);
			
			update_person(&cop, obst);
			update_person(&thief, obst);
			
			desenha_pessoa(thief, &cop);
			desenha_pessoa(cop, &cop);

			if (fuga_ladrao(thief, &ladrao_venceu) == TRUE || colisaoCopThief(cop, thief) == TRUE)
				playing = FALSE;
	
			//reinicializo a tela
			al_flip_display();
			if(al_get_timer_count(timer)%(int)FPS == 0) {
				score += 1;
				//printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
			}
		}
		//se for um evento de apertar alguma tecla do teclado
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//verifica qual tecla foi
			switch(ev.keyboard.keycode) {

				case ALLEGRO_KEY_A:
					cop.x_esq = 1;
					
				   break;

				case ALLEGRO_KEY_D:
					cop.x_dir = 1;
					
				   break;

				case ALLEGRO_KEY_ESCAPE:
					playing = 0;
				   break;
					
				case ALLEGRO_KEY_SPACE:
					
					if (cop.numero_pulos < MAX_JUMPS) {
						cop.numero_pulos++;
						cop.pulando = TRUE;
						cop.yvel = JUMP_VEL;	
					}
					
					printf("\nImplementar o pulo!");
				   break;					
				}
			}	 
		//se for um evento de soltar alguma tecla do teclado
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			//verifica qual tecla foi
			switch(ev.keyboard.keycode) {

				case ALLEGRO_KEY_A:
					cop.x_esq = 0;
				break;

				case ALLEGRO_KEY_D:
					cop.x_dir = 0;
					
				break;
			}

		}		
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = FALSE;
		}

		final_event = ev;

	} //fim do while

	
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_rest(1);
	
	if(ladrao_venceu) {
		al_clear_to_color(al_map_rgb(0,0,0));
		ALLEGRO_BITMAP *ending = al_load_bitmap("ending2.png");
		//coloca na tela a imagem armazenada na variavel image nas posicoes x=50, y=100
		al_draw_bitmap(ending, 20, 0, 0);

		printf("\nLadrao ganhou...");

		//atualiza a tela
		al_flip_display();

		//pausa a tela por 3.0 segundos
		al_rest(8.0);	
		al_destroy_bitmap(ending);		
	}
	else {
	
		char my_text[20];	
		int record;
		//colore toda a tela de preto
		al_clear_to_color(al_map_rgb(230,240,250));
		sprintf(my_text, "Score: %d", score);
		al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);


		//implementar o sistema de recorde aqui:

		FILE *recorde;
		int recorde_atual, recorde_final;
		char aux[100];
		char my_record[20];

		recorde = fopen("recorde.txt", "r");

		fscanf(recorde, "%s", &aux);

		recorde_atual = atoi(aux);

		fclose(recorde);

		if (score < recorde_atual && final_event.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {

			recorde = fopen("recorde.txt", "w");
			fprintf(recorde, "%d", score);
			fclose(recorde);

			recorde_final = score;
			sprintf(my_record, "NEW RECORD!", recorde_final);
			al_draw_text(size_32, al_map_rgb(0, 255, 0), SCREEN_W/3, SCREEN_H/2 + 150, 0, my_record);
		}

		else {
			recorde_final = recorde_atual;
			sprintf(my_record, "Record: %d", recorde_final);
			al_draw_text(size_32, al_map_rgb(0, 255, 0), SCREEN_W/3, SCREEN_H/2 + 150, 0, my_record);
		}

		fclose(recorde);
		
		//reinicializa a tela
		al_flip_display();	
	   al_rest(3);	
	 }
	
  
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}