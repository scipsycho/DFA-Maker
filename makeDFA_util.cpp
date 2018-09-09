#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <stack>
#define KLEENE '*'
#define CONCAT '.'
#define OR     '/'
#define EPSILLON '$'

using namespace std;


int nextLeafPos = 1;
map<int,char> nextLeafPosToSym;


struct syntaxTreeNode{
	
	char            symbol;
	syntaxTreeNode *left;
	syntaxTreeNode *right;
	bool            isNullable;
	set<int>        firstPos;
	set<int>        lastPos;
	int             pos;

	syntaxTreeNode()
	{
		symbol = 0;
		left = NULL;	
		right = NULL;	
		pos = 0;
	}
	syntaxTreeNode(char ch) : symbol(ch), left(NULL), right(NULL)
	{	
		pos = nextLeafPos;
		nextLeafPosToSym[pos] = ch;
		nextLeafPos++; 
	}
};

bool isChar(char c)
{
	if(c==KLEENE || c==OR || c=='(' || c==')' || c==CONCAT )
		return false;
	else
		return true;
}

int getMatchingBrac(string &regex, int i)
{
	stack<int> st;
	st.push(i);
	int in;
	in = i+1;
	while(in<regex.size())
	{
		if(regex[in]=='(')
			st.push(in);
		else if(regex[in]==')'){
			if(st.top()==i)
				return in;
			st.pop();
		}
		in++;
	}
	return -1;
}
syntaxTreeNode* constructSyntaxTree(string regex)
{

	stack<syntaxTreeNode*> st;
	syntaxTreeNode *temp;
	for(int i=0;i<regex.size();)
	{
		if(isChar(regex[i])){
			st.push(new syntaxTreeNode(regex[i]));
		}
		else if(regex[i]==KLEENE)
		{
			temp = new syntaxTreeNode;
			temp->symbol = KLEENE;
			
			if(st.empty()){
				cerr<<"Invalid Regex!!: "<<regex<<" Error Code 0x001"<<endl;
				exit(1);
			}

			temp->right = NULL;
			temp->left = st.top();
			st.pop();
			st.push(temp);
		}
		else if(regex[i]==OR)
		{
			temp = new syntaxTreeNode;
			temp->symbol = OR;
			st.push(temp);
		}

		else if(regex[i]=='(')
		{
			int in = regex.size()-1;
			in = getMatchingBrac(regex,i);
			if(in<0)
			{
				cerr<<"Invalid Regex!! : "<<regex<<" Error Code 0x002"<<endl;
				exit(1);
			}

			st.push(constructSyntaxTree(regex.substr(i+1,in-i-1)));
			i = in+1;
			continue;
		}
		else{

			cerr<<"Invalid Regex!! : "<<regex<<" Error Code  0x003"<<endl;
			exit(1);
		}

		i++;
	}

	syntaxTreeNode *op1, *op2;

	stack<syntaxTreeNode*> stRev;

	while(!st.empty())
	{
		stRev.push(st.top());
		st.pop();
	}

	op1 = op2 = NULL;
	while(!stRev.empty())
	{
		if(op1==NULL)
		{
			op1 = stRev.top();
			stRev.pop();
			continue;
		}

		op2 = stRev.top();
		stRev.pop();

		if(op2->symbol == OR  && (op2->left==NULL || op2->right==NULL))
		{
			if(stRev.empty())
			{
				cerr<<"Invalid Regex!! : "<<regex<<" Error Code 0x004"<<endl;
				exit(1);
			}

			op2->left = op1;
			op2->right = stRev.top();
			stRev.pop();
			op1 = op2;

		}
		else
		{
			temp = new syntaxTreeNode;
			temp->symbol = CONCAT;
			temp->left = op1;
			temp->right = op2;
			op1 = temp;
		}
		
	}

	return op1;
}

void initialize(syntaxTreeNode *head)
{
	if(head==NULL)
		return;
	initialize(head->left);
	initialize(head->right);
	char sym = head->symbol;
	if(isChar(sym)){
		
		if(sym == EPSILLON)
			head->isNullable = true;
		else
			head->isNullable = false;

		head->firstPos.insert(head->pos);
		head->lastPos.insert(head->pos);
	}
	else if(sym == CONCAT){
		if((head->left)->isNullable && (head->right)->isNullable)
			head->isNullable = true;
		else
			head->isNullable = false;

		set<int> &leftF =  head->left->firstPos;
		set<int> &rightF = head->right->firstPos;
		set<int> &leftL = head->left->lastPos;
		set<int> &rightL = head->right->lastPos;

		head->firstPos.insert(leftF.begin(),leftF.end());
		if(head->left->isNullable)
			head->firstPos.insert(rightF.begin(),rightF.end());
		
		head->lastPos.insert(rightL.begin(),rightL.end());
		if(head->right->isNullable)
			head->lastPos.insert(leftL.begin(),leftL.end());
	}
 	else if(sym == KLEENE){
		
		head->isNullable = true;
		head->firstPos = (head->left)->firstPos;
		head->lastPos = (head->left)->lastPos;
	}
	else if(sym == OR){
		
		if((head->left)->isNullable || (head->right)->isNullable)
			head->isNullable = true;
		else
			head->isNullable = false;

			
		set<int> &leftF =  head->left->firstPos;
		set<int> &rightF = head->right->firstPos;
		set<int> &leftL = head->left->lastPos;
		set<int> &rightL = head->right->lastPos;

		head->firstPos.insert(leftF.begin(),leftF.end());
		head->firstPos.insert(rightF.begin(),rightF.end());

		head->lastPos.insert(leftL.begin(),leftL.end());
		head->lastPos.insert(rightL.begin(),rightL.end());
		
	}
}

void createFollow(syntaxTreeNode *head, map<int,set<int> > &follow)
{
	if(head==NULL)
		return;
	createFollow(head->left,follow);
	createFollow(head->right,follow);
	
	set<int> &lastPos = head->left->lastPos;
	set<int>::iterator itr;
	set<int> exch;
	if(head->symbol==CONCAT)
		exch = head->right->firstPos;
	else if(head->symbol==KLEENE)
		exch = head->left->firstPos;

	if(!exch.empty())
		for(itr = lastPos.begin();itr!=lastPos.end();itr++)
			follow[*itr].insert(exch.begin(),exch.end());
		
}
int main(){

	//regular expression
	string regex;
	
	//For storing the set of positions that can follow a current state
	map<int,set<int> > followPos;
	map<int,set<int> >::iterator   followPos_itr;

	cin>>regex;

	//Adding pound sign to identify the final state
	regex = regex + '#';
	
	//Constructing the syntax tree
	syntaxTreeNode *root;
	root = constructSyntaxTree(regex);

	//filling isNullable, firstPos, lastPos and followPos attributes
	initialize(root);
	createFollow(root,followPos);

	
	
	//For storing transitions of a particular state of given characters
	map<char, set<int> > 	       nextState;
	map<char, set<int> >::iterator nextState_itr;
	
	

	map<set<int>, int>   setToStateNum;

	//some useful variables	
	int currStateNum = 0;
	int nextStateNum = 0;
	int stateNum     = 0;
	char sym;


	//Storing set of positions for the current state, the next state and the final state
	set<int> 	   currSet;
	set<int> 	   nextSet;
	set<int>           finalSet;
	set<int>::iterator set_itr;

	//queue for processing the states
	queue<set<int> > que;
	que.push(root->firstPos);

	//to find leaf position of the '#' character for the final set
	nextLeafPos--;


	//stores transitions with respective labels
	map< pair<int,int>,string > transition;
	map< pair<int,int>,string >::iterator transition_itr;
	string temp;
	
	while(!que.empty())
	{
		
		currSet = que.front();
		que.pop();
		
		nextState.clear();
		
		//setting the statenumber if not already set. Basically mapping a set to a state number!
		if(setToStateNum.find(currSet)==setToStateNum.end())
			setToStateNum[currSet] = stateNum++;
		
		
		//finding the next positions that are possible for every symbol
		for( set_itr = currSet.begin() ; set_itr != currSet.end() ; set_itr++)
		{
			sym = nextLeafPosToSym[*set_itr];
			if(followPos.find(*set_itr)!=followPos.end())
				nextState[sym].insert(followPos[*set_itr].begin(),followPos[*set_itr].end());	
		}
		
		//checking if the current state is a final state
		currStateNum = setToStateNum[currSet];
		if(currSet.find(nextLeafPos) != currSet.end())
			finalSet.insert(currStateNum);
		
		for( nextState_itr = nextState.begin(); nextState_itr != nextState.end(); nextState_itr++ )
		{
			sym = nextState_itr->first;
			nextSet = nextState_itr->second;
			if(setToStateNum.find(nextSet)==setToStateNum.end()){
				que.push(nextSet);
				setToStateNum[nextSet] = stateNum++;
			}
			nextStateNum = setToStateNum[nextSet];
			if(transition.find(make_pair(currStateNum,nextStateNum))==transition.end())
				temp = sym;
			else
				temp = string(",") + sym;

			transition[make_pair(currStateNum,nextStateNum)] += temp;	

		}	
	}

	//Printing transitions to draw	

	cout<<"strict digraph{ "<<endl;

	for(transition_itr = transition.begin(); transition_itr!=transition.end(); transition_itr++ )
	{
		currStateNum = (transition_itr->first).first;
		nextStateNum = (transition_itr->first).second;

		
		cout<<"\t\"Q"<<currStateNum<<"\" -> \"Q"<<nextStateNum<<"\" [ label = \""<<transition_itr->second<<"\" ];"<<endl;
	}
	
	cout<<"\t\"Q0\" [fillcolor=green, style=filled];"<<endl;
	set_itr = finalSet.begin();
	while(set_itr!=finalSet.end())
	{	
		cout<<"\t\"Q"<<(*set_itr)<<"\" [shape=doublecircle];"<<endl;
		set_itr++;
	}
	
	cout<<"}"<<endl;
}
