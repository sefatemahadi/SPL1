#include <bits/stdc++.h>
#include <gtk/gtk.h>

using namespace std;

struct method
{
	string name;
	float weight;
};

struct trieNode
{
	struct trieNode *children[26];
	int isLeaf;
};

set < string > exactMethods;
set < string > partialMethods;
vector < method > partialMethods1;
vector < method > partialMethods2;
vector < method > partialMethods3;
vector < string > methodList;

set < string > words,answer;

int totalLineOfCode,*counter;
char *fileNameWithoutComment;

void initializer(method &newMethod,string str)
{
	newMethod.name=str;
	newMethod.weight=0.0;
}

int countTotalLineOfCode(char *fileName)
{
	FILE *fp;
	int lineCounter=0;

	if((fp=fopen(fileName,"r")) == NULL)
	{
		printf("%s\n",fileName);
		printf ("Error opening source code\n");
		exit(0);
	}

	for( ; ; )
	{
		char ch;
		ch=fgetc(fp);
		if(ch==EOF) break;
		if(ch=='\n')
		{
			//totalLineOfCode++;
			lineCounter++;
		}
	}

	if(fclose(fp) == EOF)
	{
		printf ("Error closing source code\n");
		exit(0);
	}

	return lineCounter;
}

string detectLanguage(char *fileName)
{
	string language="";
	int dotIndex;

	for(int i=0;i<strlen(fileName);i++)
	{
		if(fileName[i]=='.')
		{
			dotIndex=i;
			break;
		}
	}

	for(int i=dotIndex+1;i<strlen(fileName);i++) language+=fileName[i];

	return language;
}

void removeCommentFromCode(char *name)
{
	ifstream ifstrm;
	ofstream ofstrem;

	ifstrm.open(name);
	ofstrem.open("without_singleLine_comment.txt");

	string currentLine;
	int flag=0,cpos;

	for(int i=0;i<=totalLineOfCode;i++)
	{
		getline(ifstrm, currentLine);
		cpos=currentLine.find("//");

		if(cpos!=string:: npos)
		{
			for(int j=cpos;j<currentLine.length();j++) currentLine[j]=' ';
		}

		ofstrem << currentLine << endl;
	}

	ifstrm.close();
	ofstrem.close();

	ifstrm.open("without_singleLine_comment.txt");

	if(detectLanguage(name) == "c")
	{
		ofstrem.open("without_comment.c");
		fileNameWithoutComment="without_comment.c";
	}

	else
	{
		ofstrem.open("without_comment.java");
		fileNameWithoutComment="without_comment.java";
	}

	for(int i=0;i<=totalLineOfCode;i++)
	{
		getline(ifstrm,currentLine);
		cpos=currentLine.find("/*");
		if(cpos!=string:: npos) flag=1;
		if(!flag) ofstrem << currentLine << endl;
		cpos=currentLine.find("*/");
		if(cpos!=string:: npos) flag=0;
	}

	ifstrm.close();
	ofstrem.close();
}

bool checkCurrentLine(string currentLine,string nextCurrentLine)
{
	int bracketPos=currentLine.find("(");

	if(bracketPos!=string:: npos)
	{
		string currentLinePart1 (currentLine.begin()+bracketPos+1,currentLine.end());
		if(currentLinePart1.find(")")!=string:: npos)
		{
			if(currentLine.find("for")==string:: npos && currentLine.find("while")==string:: npos && currentLine.find("if")==string:: npos && currentLine.find("else if")==string:: npos)
			{
				if(currentLine.find("{")!=string:: npos || nextCurrentLine.find("{")!=string:: npos)
				{
					return true;
				}
			}
		}
	}

	return false;
}

void startAndEndLineNumber(char *fileName,int startLine, int &endLine)
{
	ifstream ifstrm;
	ifstrm.open(fileName);
	stack < char > brackets;
	string currentLine;
	int flag=0;
	brackets.push('}');

	for(int i=0;i<totalLineOfCode;i++)
	{
		getline(ifstrm,currentLine);

		if(i >= startLine)
		{
			if(currentLine.find("{") != string:: npos)
			{
				brackets.push('{');
				flag=1;
			}
			else if(currentLine.find("}") != string:: npos) brackets.pop();
		}

		if(brackets.size() == 1 && flag)
		{
			endLine=i;
			break;
		}
	}

	ifstrm.close();
}

void saveCurrentMethod(char *fileName,string methodName,int startLine,int endLine)
{
	ifstream ifstrm;
	ofstream ofstrm;

	methodName+=".txt";
	ifstrm.open(fileName);
	ofstrm.open(methodName);
	string currentLine;

	for(int i=0;i<totalLineOfCode;i++)
	{
		getline(ifstrm,currentLine);

		if(i >= startLine && i<= endLine)
		{
			ofstrm << currentLine << endl;
		}
	}

	ifstrm.close();
	ofstrm.close();
}

int addToList(string name)
{
	string firstLetter;
	firstLetter.push_back(name[0]);
	string fileOfMethods=firstLetter+".txt";

	for(int i=0;i<firstLetter.size();i++)
	{
		firstLetter[i]=tolower(firstLetter[i]);
	}

	ifstream ifstrm;
	ifstrm.open(fileOfMethods);

	string str;

	for( ;ifstrm >> str; )
	{
		if(str == name) return 0;
	}

	ifstrm.close();

	ofstream ofstrm;
	ofstrm.open(fileOfMethods, std::ios_base::app);
	ofstrm << name << endl;
	ofstrm.close();

	return 1;
}

void searchMethods(char *fileName)
{
	ifstream ifstrm,nextIfstrm;
	string firstLine,name;
	int startLine,endLine;

	nextIfstrm.open(fileNameWithoutComment);
	ifstrm.open(fileNameWithoutComment);

	getline(nextIfstrm,firstLine);

	for(int i=0;i<totalLineOfCode;i++)
	{
		string currentLine,nextCurrentLine;

		getline(ifstrm,currentLine);
		getline(nextIfstrm,nextCurrentLine);

		if(checkCurrentLine(currentLine,nextCurrentLine))
		{
			int startBracketPosition=currentLine.find("(") ,spacePosition;

			for(int j=startBracketPosition;j>=0;j--)
			{
				if(currentLine[j]== ' ')
				{
					spacePosition=j;
					break;
				}
			}

			string name (currentLine.begin()+spacePosition+1,currentLine.begin()+startBracketPosition);
			istringstream iss(name);
			iss >> name;
			//cout << name << endl;
			if(addToList(name)) methodList.push_back(name);
			//addToList(name);
			startLine=i;
			startAndEndLineNumber(fileNameWithoutComment,startLine,endLine);
			//cout << name << " ";
			//printf ("%d %d\n",startLine,endLine);
			saveCurrentMethod(fileNameWithoutComment,name,startLine,endLine);
		}
	}

	ifstrm.close();
	nextIfstrm.close();

    ifstrm.open(fileNameWithoutComment);
    ofstream ofstrm;
    ofstrm.open(methodList[methodList.size()-1]+".txt");
    //printf("total line of code %d\n",totalLineOfCode);
    for(int i=0;i<=totalLineOfCode;i++)
    {
        string str;
        getline(ifstrm,str);
        if(i>=startLine)
        {
            ofstrm << str << endl;
        }
    }

    ifstrm.close();
    ofstrm.close();
}

void changeCase(string &str0, string &str1)
{
	for(int i=0;i<str0.length();i++) str0[i]=tolower(str0[i]);
	for(int i=0;i<str1.length();i++) str1[i]=tolower(str1[i]);
}

bool cmp1(struct method m0, struct method m1)
{
	if(m0.weight > m1.weight) return true;
	else return false;
}

void exactMatching(string searchKey)
{
	string fileToOpen,name;

	fileToOpen.push_back(searchKey[0]);
	fileToOpen+=".txt";

	ifstream ifstrm;
	ifstrm.open(fileToOpen);

	for( ;ifstrm >> name; )
	{
		string backUp=name;
		changeCase(name,searchKey);

		if(name == searchKey)
		{
			//exactMethods.insert(name);
			exactMethods.insert(backUp);
		}
	}

	ifstrm.close();
}

int lcs(string searchKey,string name,int l1,int l2)
{
	if(!l1 || !l2) return 0;
	if(searchKey[l1-1] == name[l2-1]) return 1+lcs(searchKey,name,l1-1,l2-1);
	else return max(lcs(searchKey,name,l1,l2-1),lcs(searchKey,name,l1-1,l2));
}

int calculateIndex(char ch)
{
	return (int) ch-(int) 'a';
}

struct trieNode *createNode(void)
{
	struct trieNode *aNode =NULL;

	aNode =(struct trieNode *) malloc(sizeof(struct trieNode));

	if(aNode!=NULL)
	{
		for(int i=0;i<26;i++)
		{
			aNode->children[i] =NULL;
		}

		aNode->isLeaf=0;
	}

	return aNode;
}

void insert(struct trieNode *root, string str)
{
	int strLength =str.size(),index;

	struct trieNode *crawler =root;

	for(int i=0;i<strLength;i++)
	{
		index =calculateIndex(str[i]);
		//printf("%d\n",index);

		if(crawler->children[index] == NULL)
		{
			crawler->children[index] =createNode();
		}

		crawler =crawler->children[index];
	}

	crawler->isLeaf =1;
}

int searchTrie(string searchKey,string text)
{
	int length =searchKey.size();
	int index,i;

	trieNode *root =createNode();
	insert(root,text);

	trieNode *crawler =root;

	for(i=0;i<length;i++)
	{
		index =calculateIndex(searchKey[i]);

		if(crawler->children[index] == NULL)
		{
			return -1;
		}

		crawler =crawler->children[index];
	}

	return i;
}

void clearUp(string &str)
{
	string itemsToClear="!@#$^&*(){}[]|:;''.,=";

	for(int i=0;i<str.length();i++)
	{
		for(int j=0;j<itemsToClear.length();j++)
		{
			if(str[i] == itemsToClear[j]) str[i]=' ';
		}
	}
}

void constructSet(void)
{
	for(int i=0;i<methodList.size();i++)
	{
		words.insert(methodList[i]);
	}
}

void edit(string head,string tail,int depth)
{
	if(words.find(head+tail)!=words.end())
	{
		 partialMethods.insert(head+tail);
	}
	if (depth==0) return;

	//insertion
	for(char c=65;c<123;c++)
	{
		edit(head+c,tail,depth-1);
	}

	if(tail=="") return ;
	//deletion
	edit(head,tail.substr(1),depth-1);

	//replace
	for(char c=65;c<123;c++)
	{
		if(c!=tail[0]) edit(head+c,tail.substr(1),depth-1);
		else  edit(head+c,tail.substr(1),depth);
	}

}

void partialMatching(string searchKey)
{

	constructSet();
	edit("",searchKey,2);

	string fileToOpen;

	fileToOpen.push_back(searchKey[0]);
	fileToOpen+=".txt";

	ifstream ifstrm;
	ifstrm.open(fileToOpen);

	string name;

	while(ifstrm >> name)
	{
		struct method newMethod;
		initializer(newMethod,name);
		changeCase(searchKey,name);

		if(name == searchKey) continue;

		if(name.length())
		{
			//newMethod.weight=(1.00/name.length())*lcs(searchKey,name,searchKey.length(),name.length());
			//cout << searchTrie(searchKey,name) << endl;
			if(searchTrie(searchKey,name)!=-1)
			{
				newMethod.weight =(1.00/name.length())*searchTrie(searchKey,name);
				partialMethods1.push_back(newMethod);
			}
			else
			{
				newMethod.weight=(1.00/name.length())*lcs(searchKey,name,searchKey.length(),name.length());
				partialMethods2.push_back(newMethod);
			}
		}
	}

	for(int i=0;i<methodList.size();i++)
	{
		if(searchKey[0] == methodList[i][0]) continue;

		struct method newMethod;
		initializer(newMethod,methodList[i]);

		string str=methodList[i];
		changeCase(str,str);

		if(str.find(searchKey)!=string::npos)
		{
			newMethod.weight=searchKey.size();
		}

		else newMethod.weight=0;

		partialMethods3.push_back(newMethod);
	}

	ifstrm.close();
}

double calculateTfIdf(string fileName,string searchKey)
{
	fileName+=".txt";
	ifstream ifstrm;
	ifstrm.open(fileName);

	char charArrayOfName[fileName.length()+1];

	for(int i=0;i<fileName.length();i++) charArrayOfName[i]=fileName[i];

	charArrayOfName[fileName.length()]='\0';
	//printf ("%s\n",charArrayOfName);
	int totalLine=countTotalLineOfCode(charArrayOfName);

	string currentLine;
	int termWordNumber=0,totalWordNumber=0;

	for(int i=1;i<=totalLine;i++)
	{
		getline(ifstrm,currentLine);
		clearUp(currentLine);
		istringstream iss(currentLine);
		string word;
		for( ;iss >> word; )
		{
			totalWordNumber++;
			if(word == searchKey) termWordNumber++;
		}
	}

	double tf,idf;

	if(totalWordNumber) tf=termWordNumber/totalWordNumber;
	else tf=0;

	if(termWordNumber) idf=log(totalWordNumber/termWordNumber);
	else idf=0;

	double tfidf=tf*idf;

	ifstrm.close();

	return tfidf;
}


void addTfIdf(string searchKey)
{
	double var;

	for(int i=0;i<partialMethods1.size();i++)
	{
		var=calculateTfIdf(partialMethods1[i].name,searchKey);
		if(var)
		{
			partialMethods1[i].weight*=var;
		}
	}

	for(int i=0;i<partialMethods2.size();i++)
	{
		var=calculateTfIdf(partialMethods2[i].name,searchKey);
		if(var)
		{
			partialMethods2[i].weight*=var;
		}
	}

	for(int i=0;i<partialMethods3.size();i++)
	{
		var=calculateTfIdf(partialMethods3[i].name,searchKey);
		if(var)
		{
			partialMethods3[i].weight*=var;
		}
	}
}

void view(string &name)
{	
	set < string > setList;
	vector < string > viewList;
	int index;

	for(auto str: exactMethods)
	{
		//viewList.push_back(str);
		if(setList.find(str)==setList.end())
		{
			setList.insert(str);
			viewList.push_back(str);
		}
	}

	for(auto str:partialMethods)
	{
		//viewList.push_back(str);
		if(setList.find(str)==setList.end())
		{
			setList.insert(str);
			viewList.push_back(str);
		}
	}

	for(int i=0;i<partialMethods1.size();i++)
	{	
		if(setList.find(partialMethods1[i].name)==setList.end())
		{
			setList.insert(partialMethods1[i].name);
			viewList.push_back(partialMethods1[i].name);
		}
	}

	for(int i=0;i<partialMethods2.size();i++)
	{	
		if(setList.find(partialMethods2[i].name)==setList.end())
		{
			setList.insert(partialMethods2[i].name);
			viewList.push_back(partialMethods2[i].name);
		}
	}

	for(int i=0;i<partialMethods3.size();i++)
	{	
		if(setList.find(partialMethods3[i].name)==setList.end())
		{
			if(partialMethods3[i].weight >= 0.25)
			{
				setList.insert(partialMethods3[i].name);
				viewList.push_back(partialMethods3[i].name);
			}
		}
	}

	setList.clear();

	printf("Suggestions are:\n");

	for(int i=0;i<viewList.size();i++)
	{
		cout << i+1 << "." << viewList[i]<< endl;
	}

	cout << "Enter an index: " << endl;
	cin >> index;

	index--;

	string fileToOpen=viewList[index];
	fileToOpen+=".txt";
	name =fileToOpen;

	ifstream ifstrm;
	ifstrm.open(fileToOpen);

	char charArrayOfName[fileToOpen.length()+1];

	for(int i=0;i<fileToOpen.length();i++) charArrayOfName[i]=fileToOpen[i];

	charArrayOfName[fileToOpen.length()]='\0';

	int totalLine=countTotalLineOfCode(charArrayOfName);

	for(int i=0;i<totalLine;i++)
	{
		string currentLine;
		for( ;getline(ifstrm,currentLine); )
		{
			cout << currentLine << endl;
		}
	}

	viewList.clear();
}

void replace(string name)
{
	string searchKey1,searchKey2;
	printf ("Enter text to replace:\n");
	cin >> searchKey1;

	printf ("Enter pattern to replace:\n");
	cin >> searchKey2;

	string pat=searchKey1,rep=searchKey2;

	ifstream ifstrm;
	ifstrm.open(name);

	string text;
	//cout << name << endl;

	for( ;getline(ifstrm,text); )
	{
		for( ; ; )
		{
			int pos =text.find(pat);
			if(pos==string ::npos) break;
			else if(!pos)
			{
				string cut(text.begin()+pat.length(),text.end());
				text=rep+cut;
			}
			else if((pos+pat.length()) == text.length())
			{
				string cut(text.begin(),text.begin()+pos);
				text =cut+rep;
			}
			else
			{
				string cut1(text.begin(),text.begin()+pos);
				string cut2(text.begin()+pos+pat.length(),text.end());
				text=cut1+rep+cut2;
			}
		}

		cout << text << endl;
	}

	ifstrm.close();
}

void saveList(void)
{
	ofstream ofstrm;

	ofstrm.open("entireList.txt");

	for(int i=0;i<methodList.size();i++)
	{
		if(i == methodList.size()-1) ofstrm << methodList[i];
		else ofstrm << methodList[i] << endl;
	}

	ofstrm.close();
}

void readList(void)
{
	ifstream ifstrm;
	ifstrm.open("entireList.txt");

	if(!ifstrm.is_open()) cout << "Not found " << endl;

	string currentName;

	while(ifstrm >> currentName)
	{
		methodList.push_back(currentName);
	}

	ifstrm.close();
}

void constructMatrix(char *name)
{
    int dim =methodList.size();

    int matrix[dim][dim];

    for(int i=0;i<dim;i++)
    {
        for(int j=0;j<dim;j++)
        {
            matrix[i][j]=0;
        }
    }

    for(int i=0;i<methodList.size();i++)
    {
    	string fileToOpen =methodList[i]+".txt";

    	ifstream ifstrm;

    	ifstrm.open(fileToOpen);

    	if(!ifstrm.is_open())
    	{
    		printf("Can't open file\n");
    		continue;
    	}

    	string str;

    	for( ;getline(ifstrm,str); )
    	{	
    		clearUp(str);
    		istringstream iss(str);

    		string word;
    		for( ;iss >> word; )
    		{	
    			for(int j=0;j<methodList.size();j++)
    			{	
    				if(word == methodList[j] && i!=j)
    				{
    					matrix[i][j]=1;
    				}
    			}
    		}
    	}
    }

    string cname;

    for(int i=0;i<strlen(name);i++)
    {
    	cname.push_back(name[i]);
    }

    ofstream ofstrm;

    ofstrm.open(cname+".csv");

    if(!ofstrm.is_open())
    {
    	printf("Can't open csv file\n");
    	return;
    }

    ofstrm << " " << ",";

    for(int i=0;i<methodList.size();i++)
    {
    	ofstrm << methodList[i]  <<",";
    }

    ofstrm << endl;

    for(int i=0;i<methodList.size();i++)
    {	
    	ofstrm << methodList[i] << ",";

    	for(int j=0;j<methodList.size();j++)
    	{
    		ofstrm << matrix[i][j] << ",";
    	}

    	ofstrm << endl;
    }

    ofstrm.close();
}

GtkWidget *text;
char p[100];

static void action_a(GtkWidget *widget, gpointer data)
{
    const gchar *t;
    t = gtk_entry_get_text(GTK_ENTRY(text));
    strcpy(p,t);
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int choice;
string searchKey;

static void action_b(GtkWidget *widget, gpointer data)
{
    choice=1;
}

static void action_c(GtkWidget *widget, gpointer data)
{
    choice=2;
}

static void action_d(GtkWidget *widget, gpointer data)
{
    const gchar *t;
    t=gtk_entry_get_text(GTK_ENTRY(text));
    //printf("%s\n",t);
    for(int i=0;i<strlen(t);i++)
    {
    	searchKey.push_back(t[i]);
    }
}

int main(int argc, char *argv[])
{	
	readList();
	GtkWidget *window,*vbox,*label,*button;
	char name[100];

	gtk_init(&argc, &argv);

	//readList();

	for( ; ; )
	{

		window =gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window),"GtkFixed");
		gtk_window_set_default_size(GTK_WINDOW(window),500,400);
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

		vbox = gtk_vbox_new(TRUE, 1);
  		gtk_container_add(GTK_CONTAINER(window), vbox);

  		label =gtk_label_new("Enter source code name(exit to quit):");
  		text =gtk_entry_new();
  		button =gtk_button_new_with_label("Enter");

  		gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
  		gtk_box_pack_start(GTK_BOX(vbox), text, TRUE, TRUE, 0);
  		gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

  		g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(window));

  		g_signal_connect(button, "clicked", G_CALLBACK(action_a), NULL);

  		g_signal_connect_swapped (button, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);
		gtk_widget_show_all(window);
		gtk_main();

		if(!strcmp(p,"exit"))
		{
			saveList();
			constructMatrix(p);
			return 0;
		}

		totalLineOfCode=countTotalLineOfCode(p);
		//printf ("%d\n",totalLineOfCode);
		removeCommentFromCode(p);
		searchMethods(p);

		for( ; ; )
		{
			searchKey.clear();
			window =gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_window_set_title(GTK_WINDOW(window),"GtkFixed");
			gtk_window_set_default_size(GTK_WINDOW(window),300,200);
			gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

			vbox = gtk_vbox_new(TRUE, 1);
  			gtk_container_add(GTK_CONTAINER(window), vbox);

			GtkWidget *button1;

			button =gtk_button_new_with_label("Search");
			button1 =gtk_button_new_with_label("Exit");

			g_signal_connect(G_OBJECT(window), "destroy",
        		G_CALLBACK(gtk_main_quit), G_OBJECT(window));


			g_signal_connect(button, "clicked", G_CALLBACK(action_b), NULL);

			g_signal_connect_swapped (button, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);

			g_signal_connect(button1, "clicked", G_CALLBACK(action_c), NULL);

			g_signal_connect_swapped (button1, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);

			gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
  			gtk_box_pack_start(GTK_BOX(vbox), button1, TRUE, TRUE, 0);
  			gtk_widget_show_all(window);
  			gtk_main();

			if(choice == 1)
			{	
				//string searchKey;
				/*printf("Enter a searchKey:\n");
				cin >> searchKey;*/


				exactMethods.clear();
				partialMethods.clear();
				partialMethods1.clear();
				partialMethods2.clear();
				partialMethods3.clear();

				window =gtk_window_new(GTK_WINDOW_TOPLEVEL);
				gtk_window_set_title(GTK_WINDOW(window),"GtkFixed");
				gtk_window_set_default_size(GTK_WINDOW(window),300,200);
				gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

				vbox = gtk_vbox_new(TRUE, 1);
  				gtk_container_add(GTK_CONTAINER(window), vbox);

  				GtkWidget *newTextEntry;

  				text =newTextEntry;

				text =gtk_entry_new();
				button =gtk_button_new_with_label(" Enter search key:\n");

				g_signal_connect(G_OBJECT(window), "destroy",
        		G_CALLBACK(gtk_main_quit), G_OBJECT(window));

        		g_signal_connect(button, "clicked", G_CALLBACK(action_d), NULL);

				g_signal_connect_swapped (button, "clicked",
			      G_CALLBACK (gtk_widget_destroy),window);

				gtk_box_pack_start(GTK_BOX(vbox), text, TRUE, TRUE, 0);
  				gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

  				gtk_widget_show_all(window);
  				gtk_main();

				exactMatching(searchKey);
				partialMatching(searchKey);
				addTfIdf(searchKey);
				//saveList();
				string sname;
				sort(partialMethods1.begin(),partialMethods1.end(),cmp1);
				sort(partialMethods2.begin(),partialMethods2.end(),cmp1);
				sort(partialMethods3.begin(),partialMethods3.end(),cmp1);
				view(sname);
				replace(sname);
				/*exactMethods.clear();
				partialMethods.clear();
				partialMethods1.clear();
				partialMethods2.clear();
				partialMethods3.clear();*/
				//constructMatrix();
			}

			else if(choice == 2)
			{	saveList();
				constructMatrix(p);
				break;
			}
		}
	}

	return 0;
}