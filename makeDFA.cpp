#include <iostream>
#include <string>
#include <map>
#include <set>
#include <queue>
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
syntaxTreeNode* constructSyntaxTree(string regex)
{
		
	syntaxTreeNode *itr = NULL;
	syntaxTreeNode *temp, *temp2;
	int i = 0;
	int in;
	char ch;
	while(i<regex.size())
	{
		ch = regex[i];
		if(isChar(ch))
		{
			i++;
			if(itr==NULL)
				itr = new syntaxTreeNode(ch);
			else
				itr->right = new syntaxTreeNode(ch);
		}		
		else if(ch==KLEENE || ch==OR || ch==CONCAT)
		{
			temp = new syntaxTreeNode;
			temp->left = itr;
			temp->symbol = ch;
			itr = temp;
			i++;						
		}
		else if(ch=='(')
		{
			in = i+1;
			while(regex[in]!=')')
				in++;
			temp = constructSyntaxTree(regex.substr(i+1,in-i-1));
			if( (in+1) < regex.size() && regex[in+1]==KLEENE )
			{
				temp2 = new syntaxTreeNode;
				temp2->left = temp;
				temp2->symbol = KLEENE;
				temp = temp2;
				in++;
			}
			if(itr==NULL)
				itr = temp;
			else
				itr->right = temp;
			i = in+1;
		}
	}
	return itr;
}

void addConcatOpr(string &regex)
{
	for(int i = 1; i< regex.size();)
	{
		if( (isChar(regex[i-1]) && isChar(regex[i])) || (regex[i-1]==KLEENE && isChar(regex[i])) || (isChar(regex[i-1]) && regex[i]=='(')){
			regex = regex.substr(0,i) + CONCAT + regex.substr(i);
			i++;
		}
		i++;
	}
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
	map<int,set<int> >::iterator   followPos_set_itr;

	cin>>regex;

	//Adding pound sign to identify the final state
	regex = regex + '#';
	
	//Adding Concatination symbol that is most generally skipped
	addConcatOpr(regex);


	//Constructing the syntax tree
	syntaxTreeNode *root;
	root = constructSyntaxTree(regex);

	//filling isNullable, firstPos, lastPos and followPos attributes
	initialize(root);
	createFollow(root,followPos);

	
	
	//For storing transitions of a particular state of given characters
	map<char, set<int> > 	       nextStateNum;
	map<char, set<int> >::iterator nextStateNum_itr;
	
	

	map<set<int>, int>   setToStateNum;

	//some useful variables	
	int currStateNum = 0;
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

	//For the graphviz
	cout<<"strict digraph{ "<<endl;
	
	while(!que.empty())
	{
		
		currSet = que.front();
		que.pop();
		
		nextStateNum.clear();
		
		//setting the statenumber if not already set. Basically mapping a set to a state number!
		if(setToStateNum.find(currSet)==setToStateNum.end())
			setToStateNum[currSet] = stateNum++;
		
		
		//finding the next positions that are possible for every symbol
		for( set_itr = currSet.begin() ; set_itr != currSet.end() ; set_itr++)
		{
			sym = nextLeafPosToSym[*set_itr];
			nextStateNum[sym].insert(followPos[*set_itr].begin(),followPos[*set_itr].end());	
		}
		
		//checking if the current state is a final state
		currStateNum = setToStateNum[currSet];
		if(currSet.find(nextLeafPos) != currSet.end())
			finalSet.insert(currStateNum);
		
		for( nextStateNum_itr = nextStateNum.begin(); nextStateNum_itr != nextStateNum.end(); nextStateNum_itr++ )
		{
			sym = nextStateNum_itr->first;
			if(sym=='#')
				continue;
			nextSet = nextStateNum_itr->second;
			if(setToStateNum.find(nextSet)==setToStateNum.end()){
				que.push(nextSet);
				setToStateNum[nextSet] = stateNum++;
			}

			cout<<"\t\"Q"<<currStateNum<<"\" -> \"Q"<<setToStateNum[nextSet]<<"\" [ label = \""<<sym<<"\" ];"<<endl;

		}	
	}
	cout<<"\t\"Q0\" [fillcolor=green, style=filled];"<<endl;
	set_itr = finalSet.begin();
	while(set_itr!=finalSet.end())
	{	
		cout<<"\t\"Q"<<(*set_itr)<<"\" [fillcolor=purple, style=filled];"<<endl;
		set_itr++;
	}
	cout<<"}"<<endl;
}
