#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

/**
 * Defines the maximum line size.
 */
#define MAX_LINE_SIZE 1024

/**
 * Defines the expected number of arguments.
 */
const int NUM_OF_ARGS = 4;
/**
 * Defines the file path argument number.
 */
const int FILE_PATH = 1;
/**
 * Defines the first node argument number.
 */
const int FIRST_NODE = 2;
/**
 * Defines the second node argument number.
 */
const int SECOND_NODE = 3;
/**
 * Defines the number that returns when their is an input error.
 */
const int INPUT_ERR = -1;
/**
 * Defines the number that returns when their is a memory error.
 */
const int MEMORY_ERR = -2;
/**
 * Defines the number that returns when the given graph is not a tree.
 */
const int TREE_ERR = -3;
/**
 * Defines the initialize value.
 */
const int INITIALIZE_VAL = 0;
/**
 * Defines the initialize value of the nodes dist.
 */
const int INITIALIZE_DIST_VAL = -1;
/**
 * Defines the constant of no nodes.
 */
const unsigned int NO_NODES = 0;
/**
 * Defines the constant of equal.
 */
const int EQUAL = 0;
/**
 * Defines the constant of one parent.
 */
const unsigned int ONE_PARENT = 1;
/**
 * Defines the massage that need to be printed when the arguments number is not right.
 */
const char *ARGS_MSG = "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n";
/**
 * Defines the massage that need to be printed when their was an input error.
 */
const char *INPUT_ERR_MSG = "Invalid input\n";
/**
 * Defines the massage that need to be printed when their was a memory error.
 */
const char *MEMORY_ERR_MSG = "Memory allocation failed\n";
/**
 * Defines the massage that need to be printed when the given graph is not a tree.
 */
const char *NOT_TREE_MSG = "The given graph is not a tree\n";
/**
 * Defines the space delimeters string.
 */
const char *DELIMS = " \t";
/**
 * Defines the string that said their are no children to the node.
 */
const char *NO_CHILDREN = "-";
/**
 * Defines the string that fscanf function need to get so the file reading will be right.
 */
const char *FSCANF_PARAM = "%[^\n]%*c";

/**
 * Defines Node struct, that has a key value, parent node, number of children nodes, children amount
 * value, parents amount value, dist value and previous node.
 */
typedef struct Node
{
    unsigned int key;
    struct Node *parent;
    struct Node **children;
    unsigned int children_amount;
    unsigned int parents_amount;
    int dist;
    struct Node *prev;
} Node;

/**
 * Allocates a new node.
 * @return The node, and NULL if their was a memory problem.
 */
Node *allocNode()
{
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL)
    {
        return NULL;
    }
    node->key = INITIALIZE_VAL;
    node->children_amount = INITIALIZE_VAL;
    node->parents_amount = INITIALIZE_VAL;
    node->parent = NULL;
    node->children = NULL;
    node->dist = INITIALIZE_DIST_VAL;
    node->prev = NULL;
    return node;
}
/**
 * Frees the given node.
 * @param node The node to free.
 */
void freeNode(Node **node)
{
    if (*node == NULL)
    {
        return;
    }
    free((*node)->children);
    free(*node);
    *node = NULL;
}

/**
 * Defines Graph struct, that has nodes amount value, edges amount value, root node and an array of
 * nodes in the graph.
 */
typedef struct
{
    unsigned int nodes_amount;
    unsigned int edges_amount;
    Node **nodes;
    Node *root;
} Graph;

/**
 * Allocates a new graph.
 * @return The graph, and NULL if their was a memory problem.
 */
Graph *allocGraph()
{
    Graph *graph = (Graph *) malloc(sizeof(Graph));
    if (graph == NULL)
    {
        return NULL;
    }
    graph->nodes_amount = INITIALIZE_VAL;
    graph->edges_amount = INITIALIZE_VAL;
    graph->nodes = NULL;
    graph->root = NULL;
    return graph;
}

/**
 * Frees the given graph.
 * @param graph The graph to free.
 */
void freeGraph(Graph** graph)
{
    {
        if (*graph == NULL)
        {
            return;
        }
        for (unsigned int i = INITIALIZE_VAL; i < (*graph)->nodes_amount; i++)
        {
            freeNode(&((*graph)->nodes[i]));
        }
        free((*graph)->nodes);
        free(*graph);
        *graph = NULL;
    }
}
/**
 * Insert a new node with the given key value to the given graph.
 * @param graph The graph we wnt to insert a new node to.
 * @param key The key f the new node.
 * @return EXIT_SUCCESS if the insert was successful, EXIT_FAILURE otherwise.
 */
int insert(Graph *graph, unsigned int key)
{
    if (graph == NULL)
    {
        return EXIT_FAILURE;
    }
    if (graph->nodes_amount == NO_NODES)
    {
        graph->nodes = (Node **) malloc(sizeof(Node *));
    }
    else
    {
        graph->nodes = (Node **) realloc(graph->nodes, (graph->nodes_amount + 1) * sizeof(Node *));
    }
    if (graph->nodes == NULL)
    {
        return EXIT_FAILURE;
    }
    Node *node = allocNode();
    if (node == NULL)
    {
        return EXIT_FAILURE;
    }
    node->key = key;
    graph->nodes[graph->nodes_amount++] = node;
    return EXIT_SUCCESS;
}

/**
 * Add the given child to the given node.
 * @param graph The graph that the nodes are related to.
 * @param node The node we want to add a child to.
 * @param child The child we want to add.
 * @return EXIT_SUCCESS if the adding was successful, EXIT_FAILURE otherwise.
 */
int addChild(Graph *graph, Node *node, Node *child)
{
    if (node == NULL || child == NULL)
    {
        return EXIT_FAILURE;
    }
    if (node->children_amount == NO_NODES)
    {
        node->children = (Node **) malloc(sizeof(Node *));
    }
    else
    {
        node->children = (Node **) realloc(node->children,
                                           (node->children_amount + 1) * sizeof(Node *));
    }
    if (node->children == NULL)
    {
        return EXIT_FAILURE;
    }
    node->children[node->children_amount++] = child;
    child->parent = node;
    child->parents_amount++;
    graph->edges_amount++;
    return EXIT_SUCCESS;
}

/**
 * Convert a string to int, if it's possible.
 * @param str The string to convert.
 * @return The int that fits to the given string, and -1 if the string can't be converted to int.
 */
int stringToInt(char *str)
{
    char *end = NULL;
    int n = (int) strtol(str, &end, 0);
    if (n < 0 || strlen(end) != 0)
    {
        return INPUT_ERR;
    }
    return n;
}

/**
 * Checks if the current pointer of the file is in the end of the file.
 * @param fp The file we want to check.
 * @return EXIT_SUCCESS if it's not the end of the file, EXIT_FAILURE otherwise.
 */
int endOfFile(FILE *fp)
{
    int first_char = fgetc(fp);
    if (first_char == EOF)
    {
        return EXIT_FAILURE;
    }
    ungetc(first_char, fp);
    return EXIT_SUCCESS;
}

/**
 * Reads the first line of the file and check if it's ok.
 * @param fp The file to check.
 * @return The number of nodes in the graph that describes in the file. If the file is invalid, it
 * will return -1.
 */
int checkFile(FILE* fp)
{
    if (endOfFile(fp))
    {
        return INPUT_ERR;
    }
    char line[MAX_LINE_SIZE];
    fscanf(fp, FSCANF_PARAM, line);
    char *value = strtok(line, DELIMS);
    int n = stringToInt(value);
    if (n == INPUT_ERR || strtok(NULL, DELIMS))
    {
        return INPUT_ERR;
    }
    return n;
}

/**
 * Reads one line of the file (except of the first line) and adding the children appears in the line
 * to node i.
 * @param fp The file that describes the graph.
 * @param graph The graph that contains all the nodes needed.
 * @param i The number of the node.
 * @param line A string that the line will appear in.
 * @return EXIT_SUCCESS if the line is valid and the children adding was successful, -1 if their is
 * an input error, -2 if their was a memory error.
 */
int readOneLine(FILE *fp, Graph *graph, int i, char *line)
{
    if (endOfFile(fp))
    {
        return INPUT_ERR;
    }
    fscanf(fp, FSCANF_PARAM, line);
    if (strcmp(line, NO_CHILDREN) == EQUAL)
    {
        return EXIT_SUCCESS;
    }
    char *value = NULL;
    value = strtok(line, DELIMS);
    while (value)
    {
        int n = stringToInt(value);
        if (n == INPUT_ERR || n >= (int) graph->nodes_amount)
        {
            return INPUT_ERR;
        }
        for (unsigned int j = 0; j < graph->nodes[n]->children_amount; j++)
        {
            if (graph->nodes[n]->children[j] == graph->nodes[j])
            {
                return INPUT_ERR;
            }
        }
        int add_child_ret = addChild(graph, graph->nodes[i], graph->nodes[n]);
        if (add_child_ret)
        {
            return MEMORY_ERR;
        }
        value = strtok(NULL, DELIMS);
    }
    return EXIT_SUCCESS;
}

/**
 * Builds the graph according to the given file.
 * @param fp The file that describes the graph.
 * @param n Number of nodes that should appear in the graph.
 * @param graph An empty graph (NULL).
 * @return The new graph if the building was successful, -1 if the file is invalid, -2 if their was
 * a memory error.
 */
int buildGraph(FILE *fp, int n, Graph **graph)
{
    if ((*graph = allocGraph()) == NULL)
    {
        return MEMORY_ERR;
    }
    for (int i = INITIALIZE_VAL; i < n; i++)
    {
        if (insert(*graph, i))
        {
            return MEMORY_ERR;
        }
    }
    char line[MAX_LINE_SIZE];
    int one_line_ret;
    for (int i = INITIALIZE_VAL; i < n; i++)
    {
        if ((one_line_ret = readOneLine(fp, *graph, i, line)) != EXIT_SUCCESS)
        {
            return one_line_ret;
        }
    }
    if (fgets(line, MAX_LINE_SIZE, fp) != NULL )
    {
        return INPUT_ERR;
    }
    return EXIT_SUCCESS;
}

/**
 * Checks if the given node is a parent of itself.
 * @param node The node to check.
 * @return EXIT_SUCCESS if the node is not a parent of itself, EXIT_FAILURE otherwise.
 */
int parentOfItself(Node *node)
{
    for (unsigned int i = INITIALIZE_VAL; i < node->children_amount; i++)
    {
        if (node->children[i] == node)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * Checks if the given graph is a legal tree, and if so it finds the root of the graph.
 * @param graph The graph to check.
 * @return -3 if the given graph is not a tree, EXIT_SUCCESS otherwise.
 */
int checkTreeAndFindRoot(Graph *graph)
{
    if (graph->edges_amount != graph->nodes_amount - 1)
    {
        return TREE_ERR;
    }
    Node *cur_node;
    for (unsigned int i = INITIALIZE_VAL; i < graph->nodes_amount; i++)
    {
        cur_node = graph->nodes[i];
        if (cur_node->parents_amount == NO_NODES)
        {
            if (graph->root == NULL)
            {
                graph->root = graph->nodes[i];
            }
            else
            {
                return TREE_ERR;
            }
        }
        else if (cur_node->parents_amount > ONE_PARENT || parentOfItself(cur_node))
        {
            return TREE_ERR;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * Goes through all the nodes in the given graph (tree) and update their distance from the given
 * start node and their previous node.
 * @param graph The graph to run on.
 * @param start The start node.
 */
void BFS(Graph *graph, Node *start)
{
    for (unsigned int i = INITIALIZE_VAL; i < graph->nodes_amount; i++)
    {
        (graph->nodes)[i]->dist = INITIALIZE_DIST_VAL;
    }
    start->dist = INITIALIZE_VAL;
    start->prev = NULL;
    Queue *queue = allocQueue();
    enqueue(queue, start->key);
    Node *cur_node;
    while (!queueIsEmpty(queue))
    {
        cur_node = graph->nodes[dequeue(queue)];
        for (unsigned int i = INITIALIZE_VAL; i < cur_node->children_amount; i++)
        {
            if ((cur_node->children)[i]->dist == INITIALIZE_DIST_VAL)
            {
                enqueue(queue, (cur_node->children)[i]->key);
                (cur_node->children)[i]->prev = cur_node;
                (cur_node->children)[i]->dist = cur_node->dist + 1;
            }
        }
        if (cur_node->parent && cur_node->parent->dist == INITIALIZE_DIST_VAL)
        {
            enqueue(queue, cur_node->parent->key);
            cur_node->parent->prev = cur_node;
            cur_node->parent->dist = cur_node->dist + 1;
        }
    }
    freeQueue(&queue);
}

/**
 * Finds the minimum height of the graph.
 * @param graph The graph to run on.
 * @return The minimum height of the graph.
 */
int minHeight(Graph *graph)
{
    int min_dist = (int) graph->edges_amount;
    for (unsigned int i = INITIALIZE_VAL; i < graph->nodes_amount; i++)
    {
        if (graph->nodes[i]->children_amount == NO_NODES && graph->nodes[i]->dist < min_dist)
        {
            min_dist = graph->nodes[i]->dist;
        }
    }
    return min_dist;
}

/**
 * Finds the maximum distance of the nodes in the given graph.
 * @param graph The graph to run on.
 * @return The maximum distance.
 */
int maxDist(Graph *graph)
{
    int max_dist = INITIALIZE_VAL;
    for (unsigned int i = INITIALIZE_VAL; i < graph->nodes_amount; i++)
    {
        if (graph->nodes[i]->dist > max_dist)
        {
            max_dist = graph->nodes[i]->dist;
        }
    }
    return max_dist;
}

/**
 * Finds the diameter of the graph - the longest route between to nodes.
 * @param graph The graph to run on.
 * @return The diameter.
 */
unsigned int findDiameter(Graph *graph)
{
    unsigned int diameter = INITIALIZE_VAL;
    unsigned int cur_max_dist;
    for (unsigned int i = INITIALIZE_VAL; i < graph->nodes_amount; i++)
    {
        BFS(graph, graph->nodes[i]);
        if ((cur_max_dist = maxDist(graph)) > diameter)
        {
            diameter = cur_max_dist;
        }
    }
    return diameter;
}

/**
 * Prints the relevant information about the given graph, and the shortest route from the first
 * given node to the second.
 * @param graph The graph to print the information on.
 * @param first_v Number of the first node.
 * @param second_v Number of the second node.
 */
void printOutputs(Graph *graph, unsigned int first_v, unsigned int second_v)
{
    printf("Root Vertex: %d\n", graph->root->key);
    printf("Vertices Count: %d\n", graph->nodes_amount);
    printf("Edges Count: %d\n", graph->edges_amount);
    BFS(graph, graph->root);
    printf("Length of Minimal Branch: %d\n", minHeight(graph));
    printf("Length of Maximal Branch: %d\n", maxDist(graph));
    printf("Diameter Length: %d\n", findDiameter(graph));
    BFS(graph, graph->nodes[second_v]);
    printf("Shortest Path Between %d and %d: ", first_v, second_v);
    Node *cur_node = graph->nodes[first_v];
    while (cur_node->key != second_v)
    {
        printf("%d ", cur_node->key);
        cur_node = cur_node->prev;
    }
    printf("%d\n", second_v);
}

/**
 * The main function that runs the program.
 * @param argc Number of arguments.
 * @param argv Array of the given arguments.
 * @return 0.
 */
int main(int argc, char **argv)
{
    if (argc != NUM_OF_ARGS)
    {
        fprintf(stderr, "%s", ARGS_MSG);
        return EXIT_FAILURE;
    }
    FILE* fp;
    fp = fopen(argv[FILE_PATH], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "%s", INPUT_ERR_MSG);
        return EXIT_FAILURE;
    }
    int n = checkFile(fp);
    int first_v = stringToInt(argv[FIRST_NODE]);
    int second_v = stringToInt(argv[SECOND_NODE]);
    if (n == INPUT_ERR || first_v == INPUT_ERR || second_v == INPUT_ERR ||
        first_v >= n || second_v >= n)
    {
        fprintf(stderr, "%s", INPUT_ERR_MSG);
        fclose(fp);
        return EXIT_FAILURE;
    }
    Graph *graph = NULL;
    int ret = buildGraph(fp, n, &graph);
    if (ret != EXIT_SUCCESS)
    {
        fclose(fp);
        freeGraph(&graph);
        if (ret == INPUT_ERR)
        {
            fprintf(stderr, "%s", INPUT_ERR_MSG);
        }
        else
        {
            fprintf(stderr, "%s", MEMORY_ERR_MSG);
        }
        return EXIT_FAILURE;
    }
    if (checkTreeAndFindRoot(graph) == TREE_ERR || graph->root == NULL)
    {
        fclose(fp);
        freeGraph(&graph);
        fprintf(stderr, "%s", NOT_TREE_MSG);
        return EXIT_FAILURE;
    }
    printOutputs(graph, first_v, second_v);
    freeGraph(&graph);
    fclose(fp);
    return 0;
}
