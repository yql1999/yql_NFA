#include"stdafx.h"
#include<string>
#include<iostream>
#include<fstream>
#include<map>
#include<cstdlib>
#include<vector>
#include<stack>
#include<cstdio> 
#include<set>
using namespace std;


// 结构体edge：NFA图中的边
struct edge{
	char weight;
	char next;
};


// option：输入的符号集
// expression：输入的正规式
// infix：增加连接符的正规式
// suffix：转换成的后缀表达式
// postion：状态集
string option,expression, suffix, infix, postion;
// precedence：规定运算符优先级
map<char, int> precedence;
char start='@';
// nfa：生成的NFA，即一个顶点与从顶点出发边的集合的映射
map<char, vector<edge> > nfa;
vector<string> tfunction;	//转移函数


//设置运算符优先级
void setprecedence() {
	precedence['*'] = 3;
	precedence['.'] = 2;
	precedence['|'] = 1;
}

//从文件中读取正规式，并且判断是否有非法输入
void input() {
	ifstream in("input.txt");
	if (!in) {
		cout << "文件打开失败！" << endl;
		system("Pause");
		exit(1);
	}
	in >> option >> expression;
	for (int i = 0; i < expression.length(); i++) {
		if (option.find(expression[i])!=option.npos|| expression[i]=='|' || expression[i]=='(' || expression[i] == ')' || expression[i] == '*' || expression[i] == '.') {
		}
		else {
			cout << "非法输入！" << endl;
			system("Pause");
			exit(1);
		}
	}
}

//加入连接符
void JoinConnector() {
	for (int i = 0; i < expression.length(); i++) {
		char tmp = expression[i];
		char next;
		if (i == expression.length() - 1) {
			next = '\0';
		}
		else {
			next = expression[i + 1];
		}
		if (((tmp != '(' && tmp != '.' && tmp != '|') || tmp == ')' || tmp == '*') && (next != ')'&&next != '*'&&next != '|'&&next != '.'&&next != '\0')) {
			infix = infix + tmp + '.';
		}
		else {
			infix = infix + tmp;
		}
	}
}

//中缀转后缀
void tosuffix() {
	stack<char> cstack;
	for (int i = 0; i < infix.length(); i++) {
		char tmp = infix[i];
		//若遇到左括号，则将其压栈
		if (tmp == '(') cstack.push(tmp);
		//若遇到右括号，表达括号内的中缀表达式已经扫描完毕。这时需将栈顶的运算符依次弹出并输出，直至遇到左括号
		else if (tmp == ')') {
			while (cstack.top() != '(') {
				char t = cstack.top();
				cstack.pop();
				suffix = suffix + t;
			}
			cstack.pop();
		}
		//若遇到的是运算符
		else if(tmp == '|'|| tmp == '*' || tmp == '.')
		{
			while (!cstack.empty()) {
				char t = cstack.top();
				if (precedence[t] >= precedence[tmp]) {
					suffix = suffix + t;
					cstack.pop();
				}

				else if (precedence[t] < precedence[tmp]) {
					cstack.push(tmp);
					break;
				}
			}
			if (cstack.empty())cstack.push(tmp);
		}
		else {
			suffix = suffix + tmp;
		}
	}
	while (!cstack.empty()) {
		char t = cstack.top();
		cstack.pop();
		suffix = suffix + t;
	}
}

//找到当前nfa的终态
char findback(char a,set<char> x) {
	if (nfa[a].empty()) return a;
	else {
		x.insert(a);
		int i = 0;
		while (x.find(nfa[a][i].next) != x.end()) {
			i++;
		}
		char t=findback(nfa[a][i].next,x);
		return t;
	}
}

//将后缀表达式转换成NFA
char tonfa() {
	stack<char>h;
	set<char>x;
	for (int i = 0; i < suffix.length(); i++) {
		char tmp = suffix[i];
		if (tmp == '.') {
			edge e;
			e.weight = '#';
			char t = h.top();
			h.pop();
			x.clear();
			char b = findback(h.top(),x);
			e.next = t;
			nfa[b].push_back(e);
		}
		else if (tmp == '*') {
			edge e;
			e.weight = '#';
			start++;
			char t = h.top();
			h.pop();
			x.clear();
			char b = findback(t,x);
			e.next = t;
			nfa[start].push_back(e);
			nfa[b].push_back(e);
			e.next = start + 1;
			nfa[b].push_back(e);
			nfa[start].push_back(e);
			h.push(start);
			postion = postion + start + char(start + 1);
			start++;
		}
		else if (tmp == '|') {
			edge e;
			e.weight = '#';
			start++;
			char t = h.top();
			h.pop();
			x.clear();
			char b = findback(t,x);
			e.next = t;
			nfa[start].push_back(e);
			e.next = start+1;
			nfa[b].push_back(e);
			t = h.top();
			h.pop();
			x.clear();
			b = findback(t,x);
			e.next = t;
			nfa[start].push_back(e);
			e.next = start + 1;
			nfa[b].push_back(e);
			h.push(start);
			postion = postion + start + char(start + 1);
			start++;
		}
		else {
			start++;
			edge e;
			e.weight = tmp;
			e.next = start + 1;
			nfa[start].push_back(e);
			h.push(start);
			postion = postion + start + char(start+1);
			start++;
		}
	}
	return h.top();
}

//获得转移函数
void getfunction() {	
	for (map<char,vector<edge> >::iterator it = nfa.begin();it != nfa.end(); it++) {
		for (int i = 0; i < it->second.size(); i++) {
			string tmp;
			tmp =tmp+ '(' + it->first + ',' + it->second[i].weight + ',' + it->second[i].next + ')';
			tfunction.push_back(tmp);
		}
	}
}


int main() {
	setprecedence();	//设置运算符优先级
	input();	//从文件中读取正规式
	JoinConnector();	//加入连接符
	tosuffix();	//中缀转后缀
	char s=tonfa();	//将后缀表达式转换成NFA
	ofstream out("output.txt");	//输出
    printf("-----正规式转换到非确定有穷自动机转换的一般算法-----");
    
	printf("\ninput\n正规式：");
	cout << expression << endl;
	printf("加入连接点：");
	cout << infix << endl;
	printf("转化为后缀表达式：");
	cout << suffix << endl;
	printf("\nout\n状态集：");
	out << postion << endl;
	cout << postion << endl;
	printf("符号集：");
	out << option << endl;
	cout << option << endl;
	printf("初态集：");
	printf("%c\n", s);
	out << s << endl;
	printf("终态集：");
	set<char>x;
	cout << findback(s,x) << endl;
	out << findback(s, x) << endl;

	printf("转移函数(#为空)：\n");
	//获得转移函数
	getfunction();
	for (int i = 0; i < tfunction.size(); i++) {
		cout << tfunction[i] << endl;
		out << tfunction[i] << endl;
	}
	system("Pause");
	return 0;
}
