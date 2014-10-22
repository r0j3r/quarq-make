struct g_vertex;

struct g_edge
{
	struct g_edge * next;
	struct g_vertex * tail;
};

struct g_vertex
{
	struct g_edge * e; //adjacency list
	struct g_edge * u; //update list; feedback arcs
	struct g_vertex * next;
        int incidence;
        int visited;  
	void * data; 
};

struct iterator 
{
    struct g_edge * c;
};

//add vertex
struct g_vertex * 
create_vertex(void * data);

int
add_vertex(struct g_vertex * v, struct g_vertex ** v_list);

//add edge

int
add_edge(struct g_vertex * from, struct g_vertex * to);

//remove edge

int
remove_edge(struct g_vertex * from, struct g_vertex * to);

//remove vertex

int
remove_vertex(struct g_vertex * target, struct g_vertex ** v_list);

int
if_edge_exists(struct g_vertex * from, struct g_vertex * to);

struct g_vertex *
get_vertex(struct g_vertex * list, int (*match)(void *, void *), void * key);

struct g_vertex *
next_vertex(struct g_vertex ** list);
