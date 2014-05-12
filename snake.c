#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define	D_UP	(-1)
#define D_DOWN	1
#define	D_RIGHT	2
#define D_LEFT	(-2)
#define DEF_LEN	10
#define BODY_COLOR 3
#define	DBG_COLOR 4
#define APPLE_COLOR 5
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

int nodenum=0;
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
    start_color();
    init_pair(BODY_COLOR,0,BODY_COLOR);
    init_pair(DBG_COLOR,DBG_COLOR,DBG_COLOR);
    node* head = malloc(sizeof(node));
    head->len = DEF_LEN;
    head->dir = D_LEFT;
    head->next = NULL;
    head->prev = NULL;
    node* tail = head;

    add_node(&head,D_UP,3);
    add_node(&head,D_RIGHT,3);

    char testar[maxy][maxx];
    for(int ix=0; ix<maxx;ix++)
	for(int iy=0; iy<maxy;iy++)
	    testar[iy][ix] = check_yx(iy,ix,head,10,10,game_win)?'X':'_';


    wrefresh(game_win); 
    int game_state = 1;
    int ch;
    int x=10,y=10;
    int nx=0,ny=0;
    int new_dir = -(head->dir);

    int status = 0;
    while(game_state) {
        draw_snake(game_win,head,y,x);
	mvwprintw(game_win,50,1,"x=%d,y=%d",x,y);
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
	nodenum = 0;
	nx=x+new_dir/2;
	ny=y+new_dir%2;
	status=check_yx(ny,nx,head,y,x,game_win);
	if(status){
	    game_state=0;
	//    break;
	}
	if(turn(&head,&tail,new_dir)) {
	    x=nx;
	    y=ny;
	}

	wclear(game_win);
	box(game_win,0,0);
    }
    endwin();
   
    for(int iy=0; iy<maxy;iy++){
	for(int ix=0; ix<maxx/2;ix++){
	    putchar(testar[iy][ix]);
	    putchar(testar[iy][ix]);
	}
	putchar('\n');
    }
    //printf("Exit status=%d\nnx=%d, ny=%d\nx=%d,y=%d\nnode:%d\n"
//	,status,nx,ny,x,y,nodenum);
    return 0;
}

int check_yx(int y,int x,node* head,int sy,int sx,WINDOW* wind) {
    if(!head) return 0;
    int ny=0,nx=0;
    nodenum++;

    //draw_block(wind,y+1,x+1,0);
    switch(head->dir){
    case D_UP: 
	nx=sx;
	ny=sy-(head->len);
	if ((x==sx) && (y<=sy) && (y>=ny)){
	    return 1;
	}
	break;
    case D_DOWN:
	nx=sx;
	ny=sy+(head->len);
	if ((x==sx) && (y>=sy) && (y<=ny)){
	    return 2;
	}
	break;
    case D_RIGHT:
	ny=sy;
	nx=sx+(head->len);
	if((y==sy) && (x>=sx) && (x<=nx)){
	    return 3;
	}
	break;
    case D_LEFT:
	ny=sy;
	nx=sx-(head->len);
	if((y==sy) && (x<=sx) && (x>=nx)){
	    return 4;
	}
	break;
    default:
	break;

    }
    return check_yx(y,x,head->next,ny,nx,wind);                        
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
    node* tempnode = malloc(sizeof(node*));
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
   //TODO:colors;
    wattron(wind,COLOR_PAIR(color));
    mvwprintw(wind,y,x*2,"  ");
    wattroff(wind,COLOR_PAIR(color));
}

void draw_snake(WINDOW* wind,node* head,int y,int x) {
    int nx=x,ny=y;
    
    //TODO: change to smth more elegant;
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
