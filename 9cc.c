#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum {
	TK_NUM = 256,
	TK_EOF,
};

typedef struct{
	int ty;
	int val;
	char *input;
} Token;

Token tokens[100];

void tokenize(char *p){
	int i = 0;
	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-'  || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if (isdigit(*p)) {
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}

		fprintf(stderr, "fail tokenaze: %s\n",p);
		exit(1);
	}
	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}

void error(int i){
	fprintf(stderr, "unexpeted token: %s\n", tokens[i].input);
	exit(1);
}


enum {
	ND_NUM = 256,
};

typedef struct Node {
	int ty; // 演算子かND_NUM
	struct Node *lhs;
	struct Node *rhs;
	int val; //when ND_NUM, it have number
} Node;

Node * new_node(int op, Node *lhs, Node *rhs) {
	Node *node = malloc(sizeof(Node));
	node->ty = op;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

Node *expr();
Node *mul();
Node *term();
int pos=0;

Node *expr() {
	Node *lhs = mul();
	if (tokens[pos].ty == TK_EOF) {
		return lhs;
	}

	if (tokens[pos].ty == '+') {
		pos++;
		return new_node('+', lhs, expr());
	}


	if (tokens[pos].ty == '-') {
		pos++;
		return new_node('-', lhs, expr());
	}

	return lhs;
}

Node *mul() {
	Node *lhs = term();
	if (tokens[pos].ty == TK_EOF) {
		return lhs;
	}

	if (tokens[pos].ty == '*') {
		pos++;
		return new_node('*', lhs, mul());
	}

	if (tokens[pos].ty == '/') {
		pos++;
		return new_node('/', lhs, mul());
	}

	return lhs;
}

Node *term() {
	if (tokens[pos].ty == TK_NUM) {
		return new_node_num(tokens[pos++].val);
	}

	if (tokens[pos].ty == '(') {
		pos++;
		Node *node = expr();
		if (tokens[pos].ty != ')') {
			fprintf(stderr, "we expect ')': %s\n", tokens[pos].input);
			exit(1);
		}
		pos++;
		return node;
	}
}

void gen(Node *node) {
	if (node->ty == ND_NUM) {
		printf("\tpush %d\n", node->val);
		return;
	}
	gen(node->lhs);
	gen(node->rhs);
	printf("\tpop rdi\n");
	printf("\tpop rax\n");

	switch (node->ty){
	case '+':
		printf("\tadd rax, rdi\n");
		break;
	case '-':
		printf("\tsub rax, rdi\n");
		break;
	case '*':
		printf("\tmul rdi\n");
		break;
	case '/':
		printf("\tmov rdx, 0\n");
		printf("\tdiv rdi\n");
		break;
	}
	printf("\tpush rax\n");
}

int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "invalid syntaxa\n");
		return 1;
	}

	tokenize(argv[1]);
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);
	printf("\tpop rax\n");
	printf("\tret\n");
	return 0;
}
