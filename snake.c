#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define	D_UP	(-1)
#define D_DOWN	1
#define	D_RIGHT	2
#define D_LEFT	(-2)
#define DEF_LEN	10
#define BODY_COLOR 3
#define DELAY	0.5

typedef struct Node node;
struct Node{
    int len;
    int dir;
    node* next;
    node* prev;
};

int check_yx();
int turn();
void draw_block();
void draw_snake();
void add_node();
void del_node();

int main() {
    initscr();
    int maxx,maxy;
    getmaxyx(stdscr,maxy,maxx);
    noecho(); 
    WINDOW* game_win = newwin(maxy,maxx,0,0);
    box(game_win,0,0);
    curs_set(0);
    noecho();
    keypad(game_win,TRUE);
    nodelay(game_win,1);
    
    node* head = malloc(sizeof(node));
    head->len = DEF_LEN;
    head->dir = D_UP;
    head->next = NULL;
    head->prev = NULL;
    node* tail = head;

    wrefresh(game_win); 
    int game_state = 1;
    int ch;
    int x=10,y=10;

    int nx=0,ny=0;

    int new_dir = 0;
    int status = 0;
    while(game_state) {
        draw_snake(game_win,head,y,x);
	wrefresh(game_win);
	clock_t start = clock();
	clock_t finish = start+(DELAY*CLOCKS_PER_SEC);
	while(clock()<finish) {
	    ch=wgetch(game_win);
	    switch(ch){
	    case KEY_UP: if(new_dir!=D_DOWN) new_dir=D_UP;
		break;
	    case KEY_DOWN: if(new_dir!=D_UP) new_dir=D_DOWN;
		break;
	    case KEY_LEFT: if(new_dir!=D_RIGHT) new_dir=D_LEFT;
		break;
	    case KEY_RIGHT: if(new_dir!=D_LEFT) new_dir=D_RIGHT;
		break;
	    default: //new_dir = 0;
		break;
	    }
	}

	if(turn(&head,&tail,new_dir)) {
	    nx=x+new_dir/2;
	    ny=y+new_dir%2;
	    status=check_yx(ny,nx,head,y,x);
	    if(status){
		game_state=0;
		break;
	    }
	    x=nx;
	    y=ny;
	}
	wclear(game_win);
	box(game_win,0,0);
    }
    endwin();
    printf("Exit status=%d\nnx=%d, ny=%d\nx=%d,y=%d\n",status,nx,ny,x,y);
    return 0;
}
int check_yx(int y,int x,node* head,int sy,int sx) {
    int ny=0,nx=0;
    switch(head->dir){
    case D_UP: 
	nx=sx;
	ny=sy-head->len;
	if ((x==sx) && (y<=sy) && (y>=ny))
	    return 1;
	break;
    case D_DOWN:
	nx=sx;
	ny=sy+head->len;
	if ((x==sx) && (y>=sy) && (y<=ny))
	    return 2;
	break;
    case D_RIGHT:
	ny=sy;
	nx=sx+head->len;
	if((y==sy) && (x>=sx) && (x<=nx))
	    return 3;
	break;
    case D_LEFT:
	ny=sy;
	nx=sx-head->len;
	if((y==sy) && (x<=sx) && (x>=nx))
	    return 4;
	break;
    default:
	break;
    }
    if(!head->next) return 0;
    return check_yx(y,x,head->next,ny,ny);
}

int turn(node** head, node** tail,int dir) {
    if(!dir || dir==(*head)->dir) return 0;
    if((*head)->dir == -dir) {
	(*head)->len++;
    } else {
	add_node(head,-dir,1);
    }

    if((*tail)->len>1) {
        (*tail)->len--;
    } else {
        del_node(tail);
    }
    return 1;
}

void add_node(node** head,int dir,int len) {
    node* tempnode = malloc(sizeof(node));
    tempnode->dir = dir;
    tempnode->len = len;
    tempnode->next = *head;
    (*head)->prev = tempnode;
    *head = tempnode;    
}
void del_node(node** tail) {
    if(!(*tail)->prev) return;
    *tail = (*tail)->prev;
    free((*tail)->next);
    (*tail)->next = NULL;
}

void draw_block(WINDOW* wind,int y,int x,int color) {
    wattron(wind,A_REVERSE);
    mvwprintw(wind,y,x*2,"  ");
    wattroff(wind,A_REVERSE);
}

void draw_snake(WINDOW* wind,node* head,int y,int x) {
    int nx=x,ny=y;
    
    switch(head->dir) {
    case D_UP:  
	    for(;ny>y-(head->len);ny--) 
	        draw_block(wind,ny,nx,BODY_COLOR);
    	break;
    case D_DOWN:
	    for(;ny<y+head->len;ny++)
	        draw_block(wind,ny,nx,BODY_COLOR);
    	break;
    case D_RIGHT:
	    for(;nx<x+head->len;nx++)
	        draw_block(wind,ny,nx,BODY_COLOR);
	    break;
    case D_LEFT:
	    for(;nx>x-head->len;nx--)
	        draw_block(wind,ny,nx,BODY_COLOR);
	    break;
    default:
    	break;    
    }
        
    if(head->next) { 
    	draw_snake(wind,head->next,ny,nx);
    }    

}
