#include <json/json.h> 
#include <stdio.h> 
#include <string.h> 
#include <libxml/parser.h> 
#include <libxml/tree.h> 
#include <stdlib.h> 
#include "csv.h"
#define BUFFER_SIZE 1024

/*	Function Names  	*/ 
void json_array_parse(json_object *jobj, char *key); 
void json_array_parse2(json_object *jobj, char *key,xmlNode*parent);
void json_parse2();
json_object *json_arr; 
FILE *read, *write; 
void json_parse(); 
void jsonToCsv(); 
void xmlToCsv(); 
void xmlToJson();
void jsonToXml();
void print_element_names(xmlNode *node, json_object *jobj);
static void xmlParse(xmlNode * a_node, FILE *file);

char *keyArray[256]; 
char *valArray[256]; 
int count=0; 
int countkey=0;
static char inputFileName[256]; 
char outFileName[256]; 
char fileFormat[4]; 
static FILE *file = NULL; 

 

 
int main(int argc, char **argv)
{
	
	int inputFormat = 0;
	char *format;
	
	int i;
	if(argc == 8)
	{
		
	
		for(i = 1;i < argc; i++) // it's only for parsing command line
		{
		if(strcmp(argv[i],"-i") == 0) // for keep inputfile
		{
			
			i++;
			
			sprintf(inputFileName,argv[i]);
			if(strstr(argv[i],".xml") != NULL) 
			{
				inputFormat = inputFormat + 10;
			}
			else if(strstr(argv[i],".csv") != NULL)
			{
				inputFormat = inputFormat + 20;
			}
			else if(strstr(argv[i],".json") != NULL)
			{
				inputFormat = inputFormat + 30;
			}
			
		}
		else if(strcmp(argv[i],"-o") == 0) // To find output file name
		{
			
			strcpy(outFileName,argv[++i]);
		}
		else if(strcmp(argv[i],"-t") == 0) // output format
		{
			i++;
			
			
			if(strcmp(argv[i],"xml") == 0)
			{
				inputFormat = inputFormat + 1;
				format = malloc(strlen(".xml"));
				strcpy(format,".xml");
			}
			else if(strcmp(argv[i],"csv") == 0)
			{
				inputFormat = inputFormat + 2;
				format = malloc(strlen(".csv"));
				strcpy(format,".csv");
			}
			else if(strcmp(argv[i],"json") == 0)
			{
				inputFormat = inputFormat + 3;
				format = malloc(strlen(".json"));
				strcpy(format,".json");
			}
		}
		else if(strcmp(argv[i],"-true") == 0) // Compress mod on
		{
			compress = 1;
		}
		else if(strcmp(argv[i],"-false") == 0) // Compress mod off
		{
			compress = 0;
		}
	}
	
	strcat(outFileName,format); // concat file name and format
	free(format);
	
	if(inputFormat == 12) //xml to csv //
	{
		xmlToCsv();
		printf("Successful Conversion XML to CSV!\n");
	}
	else if(inputFormat == 21) //csv to xml
	{
		csvToXml(inputFileName,outFileName,compress);
		printf("Successful Conversion CSV to XML!\n");
	}
	else if(inputFormat == 13) //xml to json //
	{
		xmlToJson();
		printf("Successful Conversion XML to JSON!\n");
	}
	else if(inputFormat == 31) //json to xml //
	{	
		jsonToXml();
		printf("Successful Conversion JSON to XML!\n");
	}
	else if(inputFormat == 23) //csv to json
	{
		csvToJson(inputFileName,outFileName);
		printf("Successful Conversion CSV to JSON!\n");
	}
	else if(inputFormat == 32) //json to csv //
	{
		jsonToCsv();
		printf("Successful Conversion JSON to CSV!\n");
	}
	else //Wrong file types
	{
		printf("Wrong Conversion Format!\n");
	}
	
	
	} //argc < 7
	else
	{
		printf("Wrong Command Entry!\n");
		printf("Please enter like this ./concube -i [input file] -o [output file] -t [output format] [-true or -false]\n");
	}
	
	
	
	return (0);
}


 



void xmlToCsv(){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	xmlNode *cur_node = NULL;
	doc = xmlReadFile(inputFileName, NULL, 0);
	static char *temp[256][256];
	int i=0;
	int j=0;
	if (doc == NULL)
	{
		printf("error: could not parse file %s\n", inputFileName);
	}
	else
	{	/*Open  new csv for write*/	
		root_element = xmlDocGetRootElement(doc);
		cur_node=root_element;
		file = fopen(outFileName,"w");
		//printf("file opened succesfully %s\n", outFileName);
		fprintf(file,"%s,",root_element->children->content);
		xmlParse(cur_node,file);

	}
	fclose(file);
	xmlFreeDoc(doc); 
	xmlCleanupParser(); 
	xmlMemoryDump(); 
	
}
static void xmlParse(xmlNode * a_node, FILE *file)
{
	xmlNode *cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE && cur_node->children->type == XML_TEXT_NODE ) {
			fprintf(file,"%s,",cur_node->children->content);
		}
		xmlParse(cur_node->children,file);
	}
}


void xmlToJson(){

			xmlDoc *doc = NULL; 
			xmlNode *root_element = NULL; 
			xmlNode *cur_node = NULL; 
			json_object *jobj = json_object_new_object();
			int i=0, j=0;
			char *source_file=inputFileName;
			doc = xmlReadFile(source_file, NULL, 0); 
			char *departure_file=outFileName; 
			write=fopen(departure_file,"w");

			if(doc==NULL){
				fprintf(write,"error: could not parse file");
				}
				else
				{
				//Get the root element node
				root_element = xmlDocGetRootElement(doc);
				print_element_names(root_element->children, jobj);
				}
			fprintf (write,"%s\n",json_object_to_json_string(jobj));
				


			xmlFreeDoc(doc);  
			xmlCleanupParser();  
			xmlMemoryDump();  
}

 void print_element_names(xmlNode *node, json_object *jobj)
{
	if(count == 0)
	{
		json_object *jarray = json_object_new_array();
		xmlNode *cur_node = NULL;
		char rootname[20];
		for (cur_node = node; cur_node; cur_node = cur_node->next) {
        		if (cur_node->type == XML_ELEMENT_NODE) {
				if(count == 1)
 				{
					sprintf(rootname, "%s",cur_node->name);
				
				}

				count++;
				print_element_names(cur_node->children, jarray);
			}
			
        	}
		if (count != 0)
		{
			json_object_object_add(jobj, rootname, jarray);
		}
	}
	else
	{
		xmlNode *cur_node = NULL;
		json_object *jobb = json_object_new_object();
		for (cur_node = node; cur_node; cur_node = cur_node->next) {
        		if (cur_node->type == XML_ELEMENT_NODE) {
        			json_object *newj = json_object_new_string(cur_node->children->content);
				json_object_object_add(jobb,cur_node->name,newj);
			}
        	}
		json_object_array_add(jobj,jobb);
	}
		
}


void jsonToCsv(){ 
//FILE *read, *write; 
char *source_file=inputFileName; 
char *departure_file=outFileName; 
int i=0, n=100; 
char A[BUFFER_SIZE]; 
	 

	//available to acces to file 

	read=fopen(source_file,"r"); 

	write=fopen(departure_file,"w");//here will be a+ 

	 
	 
	if((read==NULL)) 
	{ 
	fprintf(write,"error: file couldn't open\n"); 

	} 
	if(write==NULL) 
	{ 
	fprintf(write,"error: file write is not okey\n"); 
 
	} 
	while(fgets(A,BUFFER_SIZE,read)) 
	{ 
 			 
	json_object *jobj = json_tokener_parse(A); 
	json_parse(jobj); 

               
	} 
	 
for(int i=0; i<countkey; i++) 
{ 
	
	if(i==countkey-1) {
	fprintf(write,"%s \n",keyArray[i]); 
	} 
	else{
 	fprintf(write,"%s,",keyArray[i]); 
	}
}
	
 	int temp=1;
	for(int i=0; i<count; i++) 
	{   
	
		if(countkey==temp){
		//fprintf(write,"%s\n");
		fprintf(write,"%s\n",valArray[i]);
		temp=1;
		}
		else{
		fprintf(write,"%s,",valArray[i]);
		temp++;
		}
	}	 
 
 
		 
		fclose(read); 
		fclose(write); 
		//printf("%i\n",count); 
           	 
	 
 
 
	} 


void json_parse(json_object * jobj) { 

enum json_type type; 
json_object_object_foreach(jobj, key, val) { 
	type = json_object_get_type(val); 
	int kontrol=0;
	switch (type) { 
		case json_type_string:
		
		for(int i=0; i<countkey; i++){
		if(strcmp(keyArray[i],key)==0)
		kontrol=1;
		}  
		if(kontrol==0){

		keyArray[countkey]=malloc(strlen(key)); 
		strcpy(keyArray[countkey],key); 
		countkey++;
		}
		valArray[count]=malloc(strlen(json_object_get_string(val))); 
		strcpy(valArray[count],json_object_get_string(val)); 
		//printf("%s\n", json_object_get_string(val));
		count++; 
				 
		break; 
		case json_type_int: 
		keyArray[count]=malloc(strlen(key)); 
		strcpy(keyArray[count],key); 
		valArray[count]=malloc(strlen(json_object_get_string(val))); 
		strcpy(valArray[count],json_object_get_string(val)); 
		count++; 
		 
		break; 
		case json_type_boolean: 
		fprintf(write,"key: %s\n", key); 
		fprintf(write,"value: %s\n", json_object_get_boolean(val) ? "true":"false"); 
		break; 
		case json_type_array: 
		//fprintf(write,"key: %s\n",json_object_get_int(val) ); 
		json_array_parse(jobj, key); 
		break; 
		/*case json_type_object: 
		jobj = json_object_object_get(jobj, key); 
		json_parse(jobj); 
		break; */
		case json_type_null: 
		fprintf(write,"files are empty"); 

		} 
	} 
} 
void json_array_parse(json_object *jobj, char *key) 
{ 
	 
	enum json_type type; 
	 
	json_object *value; 
	if (key)   //If the key value is not null(no array inside array) 
	{	 
		json_arr = json_object_object_get(jobj, key); 
	} 
	else 
	{	 
		json_arr = jobj; 
	} 
	 
	for (int i = 0; i < json_object_array_length(json_arr); i++) 
	{ 
		 
		value = json_object_array_get_idx(json_arr, i); //iç içe array varsa ona bakıyor. 
		type = json_object_get_type(value); 
		if (type == json_type_array) 
		{ 
			 
			json_array_parse(value,NULL); 
		} 
		else 
		{ 
			 
			json_parse(value);  //gelen değer array değilse bunu pars etmek için gönderiyor. 
		} 
	} 
} 

void json_parse2(json_object * jobj,xmlNode*parent) {
enum json_type type;

json_object_object_foreach(jobj, key, val) {

type = json_object_get_type(val);

	switch (type) {

		case json_type_string: 

		keyArray[count]=malloc(strlen(key));

		strcpy(keyArray[count],key);

		valArray[count]=malloc(strlen(json_object_get_string(val)));

		strcpy(valArray[count],json_object_get_string(val));

		count++;

		break;

		case json_type_int:

		keyArray[count]=malloc(strlen(key));

		strcpy(keyArray[count],key);

		valArray[count]=malloc(strlen(json_object_get_string(val)));

		strcpy(valArray[count],json_object_get_string(val));

		count++;

		break;

		


		case json_type_boolean:

		fprintf(write,"key: %s\n", key);

		fprintf(write,"value: %s\n", json_object_get_boolean(val) ? "true":"false");

		break;

		case json_type_array:

		//fprintf(write,"key: %s\n",json_object_get_int(val) );
		printf("asd0\n");
		json_array_parse2(val, key,parent);

		break;
/*
		case json_type_object:

		jobj = json_object_object_get(jobj, key);

		json_parse(jobj);

		break;*/

		case json_type_null:

		fprintf(write,"files are empty");
		}
	}
}

void json_array_parse2(json_object *jobj, char *key,xmlNode*parent)
{
	enum json_type type;
	json_object *value;
	json_object *j_arr = jobj;
	if (key)   //If the key value is not null(no array inside array)
	{
		
		j_arr = json_object_object_get(jobj, key);
	}

	else
	{	
		j_arr = jobj;
	}
	
	for (int i = 0; i < json_object_array_length(j_arr); i++)
	{	
		value = json_object_array_get_idx(j_arr, i); //iç içe array varsa ona bakıyor.
		type = json_object_get_type(value);
		if (type == json_type_array)
		{	
			json_array_parse2(value,key,parent);
		}
		else
		{	
			xmlNodePtr node = xmlNewChild(parent,NULL,BAD_CAST key,NULL);
			json_parse2(value,node);  //gelen değer array değilse bunu pars etmek için gönderiyor.

		}

	}

}
void jsonToXml()
{	
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL;/* node pointers */ 
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	//FILE *read, *write;
	char *source_file=inputFileName;
	int i=0, n=100;
	char A[BUFFER_SIZE];
	//available to acces to file

		json_object *jobj = json_object_from_file(inputFileName);
		json_parse2(jobj,root_node);
		xmlSaveFormatFileEnc(outFileName, doc, "UTF-8", 1); 
			
}
