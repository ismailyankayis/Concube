#define csv_h
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <json/json.h>

int compress = 0;
int row = 0;
int column = 0;

static char *array[256][256]; // array for csv to xml

int readCsv(char *inputFileName);
//void csvToJson(char *inputFileName,char *outFileName)

extern void csvToXml(char *inputFileName,char *outFileName,int compresst){
	int flag = readCsv(inputFileName);
	
	if(flag == 1)
	{
	compress = compresst;
	
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */ 
	xmlAttrPtr attribute;
	char tagName[256]; // tag name
	char temp2[100]; //temp
	char tagContent[100]; // tag content
	char key[100]; // attribute key
	char value[100]; // attribute value
	
	
	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	int i,j;
	
	sprintf(tagName,"node");
	char *pch;
	char *pch2;
	
	for(i = 1;i < row; i++)
	{
		if(strstr(array[0][0],"_") != NULL)
		{
			strcpy(temp2,array[0][0]);
			pch = strtok(temp2,"_");
			sprintf(tagName,pch);
			
		}
		node = xmlNewChild(root_node,NULL,BAD_CAST tagName,NULL);
		
		for(j = 0;j < column; j++)
		{
		if(array[i][j] != NULL)
		{
			strcpy(tagContent,array[i][j]);
			if(strstr(array[i][j],"\"") != NULL) //it has attribute
			{
				strcpy(temp2,array[i][j]);
				pch2 = strtok(temp2,"\"");
				strcpy(tagContent,pch2); //tag content
				
				pch2 = strtok(NULL,"\"");
				strcpy(temp2,pch2); // key and value
				
				pch2 = strtok(temp2,":");
				strcpy(key,pch2); //key
				
				pch2 = strtok(NULL,":"); 
				strcpy(value,pch2); //value
			}
			
			if(strstr(array[0][j],"_") != NULL) //if it has a parent
			{
				strcpy(temp2,array[0][j]);
				pch = strtok(temp2,"_");
				pch = strtok(NULL,"_");
			}
			else // if it has no parent
			{
				strcpy(pch,array[0][j]);
			}
			
			if(compress == 0)
			{
				if(strstr(array[i][j],"\"") != NULL) // it has attribute
				{
					xmlNewProp(node1, BAD_CAST key, BAD_CAST value);
				}
				
				node1 = xmlNewChild(node,NULL,BAD_CAST pch,BAD_CAST tagContent);
			}
			else if(compress == 1)
			{
				if(strstr(array[i][j],"\"") != NULL) // it has attribute
				{
					xmlNewProp(node, BAD_CAST key, BAD_CAST value);
				}
				xmlNewProp(node, BAD_CAST pch, BAD_CAST tagContent);
			}
			
			
			
			free(array[i][j]);
		}
		}
	}
	
	
	xmlSaveFormatFileEnc(outFileName, doc, "UTF-8", 1); 
	
	
	xmlFreeDoc(doc); 
	xmlCleanupParser(); 
	xmlMemoryDump(); 
	
	}
}

extern int readCsv(char *inputFileName){
	FILE *file = fopen(inputFileName,"r");
	char line[256];
	char *pch;
	
	int tempcol;
	int i = 0;
	int j = 0;
	if(file == NULL)
	{
		printf("FILE NOT FOUND!\n");
	return 0;
	}
	else
	{
		
		fgets(line,256,file);
		if(strcmp(line,"") == 0)
		{
		printf("The csv file is empty !!\n");
			return 0;
		}
		
		while( feof(file) == 0 ) //Read Lines
		{
			
			j = 0;
			tempcol = 0; // To keep number of column
			
			strcpy(line,strtok(line,"\n"));//remove new line at the end of line
			pch = strtok(line,","); // parsing current line by comma
			
			array[i][j] = malloc(strlen(pch));// Memory allocation for words
			strcpy(array[i][j], pch); // Assign word to array
			
			
			tempcol++;
			
			/*	Continue of parsing	*/
			for(pch = strtok(NULL , ","); pch != NULL; pch = strtok(NULL , ","))
			{
				j++;
				
				array[i][j] = malloc(strlen(pch));
				strcpy(array[i][j], pch);
			
				tempcol++;
			
			}
			
			if(tempcol > column){ // To keep max column number
				column = tempcol;
			} 
			i++;
			row++;
			fgets(line,256,file);
			
		}
		fclose(file);
	
		
		
		return 1;
	}
}

extern void csvToJson(char *inputFileName,char *outFileName)
{
	int flag = readCsv(inputFileName);
	
	if(flag == 1)
	{
		
		//FILE *fp;
		int i,j;
		char *temp;
		char *temp2;
		char *temp_p;
		int parent = 0;
		
		
		if(strstr(array[0][0],"_") != NULL) // it has parent name
		{
			temp2 = malloc(strlen(array[0][0]));
			strcpy(temp2,array[0][0]);
			temp = strtok(temp2,"_");
			parent = 1;
			
		}
		else
		{
			temp = malloc(4);
			strcpy(temp,"node");
		}
		
		json_object *root = json_object_new_object();
		json_object *jarray = json_object_new_array();
		json_object *jstring;
		
		
		
		
		for(i=1; i<row; i++)
		{
			
			json_object *parent_obj = json_object_new_object();

			for(j=0; j<column; j++)
			{
			if(array[i][j] != NULL)
			{
				
				if(parent == 0)
				{
					temp_p = malloc(strlen(array[0][j]));
					strcpy(temp_p,array[0][j]);
				}
				else
				{
					temp2 = malloc(strlen(array[0][j]));
					strcpy(temp2,array[0][j]);
					
					temp_p = strtok(temp2,"_");
					temp_p = strtok(NULL,"_");
					
					free(temp2);
				}
				jstring = json_object_new_string(array[i][j]);
				json_object_object_add(parent_obj,temp_p,jstring);
				
				
			}
			}
			json_object_array_add(jarray,parent_obj);
		}
		
		
		json_object_object_add(root,temp,jarray);
		json_object_to_file(outFileName,root);
		//free(temp);
		//free(temp2);
		
		
	}
}
