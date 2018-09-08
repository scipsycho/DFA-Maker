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


int currentPos = 1;
map<int,char> posToSym;
struct syntaxTree{
	
	char        symbol;
	syntaxTree *left;
	syntaxTree *right;
	bool        isNullable;
	set<int>    firstPos;
	set<int>    lastPos;
	int         pos;

	syntaxTree()
	{
		symbol = 0;
		left = NULL;	
		right = NULL;	
		pos = 0;
	}
	syntaxTree(char ch) : symbol(ch), left(NULL), right(NULL)
	{	
		pos = currentPos;
		posToSym[pos] = ch;
		currentPos++; 
	}
};

bool isChar(char c)
{
	if(c==KLEENE || c==OR || c=='(' || c==')' || c==CONCAT )
		return false;
	else
		return true;
}
syntaxTree* constructSyntaxTree(string regex)
{
		
	syntaxTree *itr = NULL;
	syntaxTree *temp, *temp2;
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
				itr = new syntaxTree(ch);
			else
				itr->right = new syntaxTree(ch);
		}		
		else if(ch==KLEENE || ch==OR || ch==CONCAT)
		{
			temp = new syntaxTree;
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
				temp2 = new syntaxTree;
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

void printSpace(int num){

        for(int i=0;i<num;i++)
                std::cout<<" ";
}
void printTree(syntaxTree* head)
{
        if(head==NULL)
                return;

        int space=100;
        int diff = 5;
        syntaxTree* curr;
        syntaxTree* special = new syntaxTree;
        syntaxTree* leftArrow = new syntaxTree;
        std::queue<syntaxTree*> Q;
        Q.push(head);
        Q.push(special);
        std::queue<int> pos;
        pos.push(space);
        int spacesPrinted=0;

        while(!Q.empty()){

                curr = Q.front();
                Q.pop();
                if(curr==special){
                  if(!Q.empty())
                    Q.push(special);
                  std::cout<<std::endl;
                  spacesPrinted = 0;
                  continue;
                }
                space = pos.front();
                pos.pop();
                printSpace(space - spacesPrinted);
                spacesPrinted = space;
                if(curr!=NULL){
                    cerr<<curr->symbol;
                }
                else
                  cerr<<"N";

                if(curr!=NULL ){

                  Q.push(curr->left);
                  pos.push(space-diff);


                  Q.push(curr->right);
                  pos.push(space + diff);
                }
        }

}
void addConcatOpr(string &regex)
{
	for(int i = 1; i< regex.size();)
	{
		if( (isChar(regex[i-1]) && isChar(regex[i])) || (regex[i-1]==KLEENE && (isChar(regex[i]) ||  regex[i]=='(')) || (isChar(regex[i-1]) && regex[i]=='(')){
			regex = regex.substr(0,i) + CONCAT + regex.substr(i);
			i++;
		}
		i++;
	}
}

void initialize(syntaxTree *head)
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
void showVal(syntaxTree *head)
{
	if(head == NULL)
		return;
	showVal(head->left);
	cerr<<head->symbol<<": "<<head->pos<<endl;
	cerr<<"isNullable: "<<head->isNullable<<endl;
	cerr<<"firstPos: ";
	set<int>::iterator itr;
	for(itr = head->firstPos.begin();itr!=head->firstPos.end();itr++)
	{
		//cout<<*itr<<", ";
	}
	cerr<<endl;
	cerr<<"lastPos: ";
	for(itr = head->lastPos.begin();itr!=head->lastPos.end();itr++)
	{
		cerr<<*itr<<", ";
	}
	cerr<<endl;
	cerr<<"-----------------------------"<<endl;
	showVal(head->right);
}

void createFollow(syntaxTree *head, map<int,set<int> > &follow)
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

	string regex;
	map<int,set<int> > followSym;
/*
	cout<<"+------------Special Characters-----------+ "<<endl;
	cout<<"|    *  : zero or more previous characters| "<<endl;
	cout<<"|    $  : Epsillon or No character        | "<<endl;
	cout<<"|    /  : or operation                    | "<<endl;
	cout<<"|   ()  : used to group characters        | "<<endl;
	cout<<"|    .  : and operation                   | "<<endl;
	cout<<"+-----------------------------------------+ "<<endl;
	cout<<"Enter the regular experession: ";
*/
	cin>>regex;

	cerr<<"Initial Regex: "<<regex<<endl;
	regex = regex + '#';
	cerr<<"Regex with pound: "<<regex<<endl;
	addConcatOpr(regex);
	cerr<<"Regex with concat: "<<regex<<endl;
	syntaxTree *rootSyntaxTree;

	rootSyntaxTree = constructSyntaxTree(regex);

	initialize(rootSyntaxTree);
	printTree(rootSyntaxTree);
	cout<<endl;
	createFollow(rootSyntaxTree,followSym);

	map<int,set<int> >::iterator fItr;
	map<char, set<int> > nextStateSet;
	map<char, set<int> >::iterator tItr;
	set<int>::iterator itr;
	showVal(rootSyntaxTree);
	cerr<<"Follow: "<<endl;
	for(fItr = followSym.begin();fItr!=followSym.end();fItr++)
	{
		cerr<<fItr->first<<": ";
		for(itr=(fItr->second).begin();itr!=(fItr->second).end();itr++)
		{
			cerr<<(*itr)<<", ";
		}
		cerr<<endl;
	}

	map<set<int>, int> stateSetToStateNum;
	int currStateNum = 0;
	int stateNum = 0;
	queue<set<int> > que;
	set<int> currSet;
	set<int> nextSet;
	set<int> finalSet;
	que.push(rootSyntaxTree->firstPos);

	
	char sym;
	currentPos--;
	cerr<<"CurrentPos: "<<currentPos<<endl;
	cout<<"strict digraph{ "<<endl;
	while(!que.empty())
	{
		currSet = que.front();
		que.pop();
		nextStateSet.clear();
		if(stateSetToStateNum.find(currSet)==stateSetToStateNum.end())
			stateSetToStateNum[currSet] = stateNum++;
		for(itr = currSet.begin();itr!=currSet.end();itr++)
		{
			sym = posToSym[*itr];
			nextStateSet[sym].insert(followSym[*itr].begin(),followSym[*itr].end());	
		}
		currStateNum = stateSetToStateNum[currSet];
		if(currSet.find(currentPos) != currSet.end())
			finalSet.insert(currStateNum);
		for(tItr = nextStateSet.begin();tItr!=nextStateSet.end();tItr++)
		{
			sym = tItr->first;
			if(sym=='#')
				continue;
			nextSet = tItr->second;
			if(stateSetToStateNum.find(nextSet)==stateSetToStateNum.end()){
				que.push(nextSet);
				stateSetToStateNum[nextSet] = stateNum++;
			}

			cout<<"\t\"Q"<<currStateNum<<"\" -> \"Q"<<stateSetToStateNum[nextSet]<<"\" [ label = \""<<sym<<"\" ];"<<endl;

		}	
	}
	cout<<"\t\"Q0\" [fillcolor=green, style=filled];"<<endl;
	itr = finalSet.begin();
	while(itr!=finalSet.end())
	{	
		cout<<"\t\"Q"<<(*itr)<<"\" [fillcolor=purple, style=filled];"<<endl;
		itr++;
	}
	cout<<"}"<<endl;
}
