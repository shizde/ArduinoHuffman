/*
 * ArduinoHuffman
 * Authors: Rafael Perez
 */

#include "arduinoHuffman.h"


/************************************************************************/
/* STRUCTURES TO BUILD HUFFMAN TREE                                     */
/************************************************************************/

typedef struct node{
	unsigned char character;
	unsigned int frequency;
	struct node *left_node, 
				*right_node, 
				*next_node;
}Node;

typedef struct {
	Node *begin_list;
	int size;
}List;

/************************************************************************/
/* FREQUENCY PROCEDURES                                                 */
/************************************************************************/
/*! \fn void init_frequency(unsigned int frequency[])
    \brief Initialize the frequency array
    \param frequency[] Array to store the frequency of each character
*/
void init_frequency(unsigned int frequency[]){
	for (int i = 0; i < NUM_CHAR; i++){
		frequency[i] = 0;
	}
}

/*! \fn void fill_frequency(unsigned char text[], unsigned int frequency[])
    \brief Fill the frequencies of each character present in the JSON string analyzed
	\param text[] Array to store each unique character present on string t be compressed
    \param frequency[] Array to store the frequency of each character
*/
void fill_frequency(unsigned char text[], unsigned int frequency[]){
	int i = 0;
	while (text[i] != '\0'){
		frequency[text[i]]++;
		i++;
	}
}


/*! \fn void print_frequency(unsigned int frequency[])
    \brief Display the frenquency array
    \param frequency[] Array to store the frequency of each character
*/
void print_frequency(unsigned int frequency[]){
	Serial.println("Frequency Table");
	for (int i = 0; i < NUM_CHAR; i++){
		if (frequency[i] != 0){
			Serial.print(i);
			Serial.print(" = ");
			Serial.write(i);
			Serial.print(" = ");
			Serial.print(frequency[i]);
			Serial.println(" |");
		}
	}
}

/************************************************************************/
/* LIST PROCEDURES                                                      */
/************************************************************************/
/*! \fn void create_list(List *list_huff)
    \brief Create the list in order to organize all the characters and their frequencies
    \param *list_huff Pointer to the list that will be used to organize the characters and their frequencies
*/
void create_list(List *list_huff){
	list_huff->begin_list = NULL;
	list_huff->size = 0;
}

/*! \fn void insert_list(List *list_huff, Node *node)
    \brief Create the list node by node
    \param *list_huff Pointer to the list that will be used to organize the characters and their frequencies
    \param *node Pointer to nodes used to create the list
*/
void insert_list(List *list_huff, Node *node){
	Node *temporary;
	// Verify if list is empty
	if (list_huff->begin_list == NULL){
		list_huff->begin_list = node;
	}
	// If !empty, verify if frequency is lower
	else if (node->frequency < list_huff->begin_list->frequency){
		node->next_node = list_huff->begin_list;
		list_huff->begin_list = node;
	}
	else {
		temporary = list_huff->begin_list;
		while (temporary->next_node && temporary->next_node->frequency <= node->frequency)
			temporary = temporary->next_node;
		node->next_node = temporary->next_node;
		temporary->next_node = node;
	}
	list_huff->size++;
}


/*! \fn void fill_list(unsigned int frequency[], List *list_huff)
    \brief Insert elements in the list node by node
    \param frequency[] Pointer to array f frequency f each unique character
    \param *list_huff Pointer to the list that will be used to organize the characters and their frequencies
*/
void fill_list(unsigned int frequency[], List *list_huff){
	Node *new_node;
	for (int i = 0; i < NUM_CHAR; i++){
		if (frequency[i] > 0){
			new_node = (Node*)malloc(sizeof(Node));
			if (new_node){
				new_node->character = i;
				new_node->frequency = frequency[i];
				new_node->right_node = NULL;
				new_node->left_node = NULL;
				new_node->next_node = NULL;
				
				insert_list(list_huff, new_node);
			}
			else{
				Serial.println("MEMORY ALLOCATION ERROR AT FILL LIST PROCEDURE.");
			}
		}
	}
}


/*! \fn void print_list(List *list_huff)
    \brief Display all the list by its elements
    \param *list_huff Pointer to the list that will be used to organize the characters and their frequencies
*/
void print_list(List *list_huff){
	Node *temporary = (Node *)malloc(sizeof(Node));
	temporary = list_huff->begin_list;

	Serial.println("LIST OF NODES");
	while (temporary){
		Serial.print("Character: ");
		Serial.write(temporary->character);
		Serial.print(" | Frequency: ");
		Serial.println(temporary->frequency);
		temporary = temporary->next_node;
	}
}


/************************************************************************/
/* HUFFMAN TREE                                                         */
/************************************************************************/
/*! \fn Node* remove_begin(List *list_huff)
    \brief	Function to remove nodes once they are merged in order to organize the Binary Tree
    \param *list_huff Pointer to the list that will be used to organize the characters and their frequencies
	
	\return temporary Node to be removed
*/
Node* remove_begin(List *list_huff){
	Node *temporary = NULL;
	
	if (list_huff->begin_list){
		temporary = list_huff->begin_list;
		list_huff->begin_list = temporary->next_node;
		temporary->next_node = NULL;
		list_huff->size--;
	}
	
	return temporary;
}

/*! \fn Node* create_tree(List *list_huff)
    \brief	Function to create the Binary Tree
    \param *list_huff Pointer to the list that will be used to organize the characters and their frequencies
	
	\return list_huff->begin_list List created
*/
Node* create_tree(List *list_huff){
	Node *first_node, *second_node, *new_node;
	
	while (list_huff->size > 1){
		first_node = remove_begin(list_huff);
		second_node = remove_begin(list_huff);
		
		new_node = (Node *)malloc(sizeof(Node));
		if (new_node){
			new_node->character = '+';
			new_node->frequency = first_node->frequency + second_node->frequency;
			new_node->left_node = first_node;
			new_node->right_node = second_node;
			new_node->next_node = NULL;
			
			insert_list(list_huff, new_node);
		}
		else {
			Serial.println("ALLOCATION MEMORY AT CREATE TREE.");
		}
	}
	return list_huff->begin_list;
}


/*! \fn void print_tree(Node *root, int size)
    \brief	Function to display the Binary Tree
    \param size Height of each node in the tree
	\param *root Pointer to the nodes present in the tree
*/
void print_tree(Node *root, int size){
	if (root->left_node == NULL && root->right_node== NULL){
		Serial.print("Leave node: ");
		Serial.write(root->character);
		Serial.print(" | Height: ");
		Serial.println(size);
	}
	else {
		print_tree(root->left_node, size+1);
		print_tree(root->right_node, size+1);
	}
}


/************************************************************************/
/* HUFFMAN DICTIONARY                                                   */
/************************************************************************/

/*! \fn int tree_height(Node *root)
    \brief	Function to calculate the farthest node of the Binary Tree
	\param *root Pointer to the nodes present in the tree
*/
int tree_height(Node *root){
	int tree_left, tree_right;
	if (root == NULL){
		return -1;
	}
	else {
		tree_left = tree_height(root->left_node) + 1;
		tree_right = tree_height(root->right_node) + 1;
		
		if (tree_left > tree_right)
			return tree_left;
		else
			return tree_right;
	}
}


/*! \fn char** dictionary_allocate(int columns)
    \brief	Function to allocate memory for the dictionary used in the binary transformation
	\param int Height of the Binary Tree
	
	\return dict Empty dictionary with memory allocated
*/
char** dictionary_allocate(int columns){
	char **dict = (char **)malloc(sizeof(char*) * NUM_CHAR);
	
	for (int i = 0; i < NUM_CHAR; i++)
		dict[i] = (char *)calloc(columns, sizeof(char));
	
	return dict;
}


/*! \fn void create_dictionary(char **dict, Node *root, char *path, int columns)
    \brief	Function to create the dictionary used in the binary transformation
	\param **dict Dictionary to translate the string in char to binary codes
	\param *root Pointer to the nodes present in the tree
	\param *path Pointer to the path traveled in the tree
	\param int Height of the Binary Tree
	
*/
void create_dictionary(char **dict, Node *root, char *path, int columns){
	char lft[columns], rgt[columns];
	
	if (root->left_node == NULL && root->right_node == NULL)
		strcpy(dict[root->character], path);
	else {
		strcpy(lft, path);
		strcpy(rgt, path);
		
		strcat(lft, "0");
		strcat(rgt, "1");
		
		create_dictionary(dict, root->left_node, lft, columns);
		create_dictionary(dict, root->right_node, rgt, columns);
	}
}


/*! \fn void create_dictionary(char **dict, Node *root, char *path, int columns)
    \brief	Function to display the dictionary used in the binary transformation
	\param **dict Dictionary to translate the string in char to binary codes
	
*/
void print_dictionary(char **dict){
	Serial.println("-- Dictionary --");
	for (int i = 0; i < NUM_CHAR; i++){
		if (strlen(dict[i]) > 0){
			Serial.print(i);
			Serial.print(" : ");
			Serial.println(dict[i]);
		}
	}
}



/************************************************************************/
/* HUFFMAN ENCODING                                                     */
/************************************************************************/

/*! \fn int get_string_size(char **dict, unsigned char *text)
    \brief	Function to calculate the string size
	\param **dict Dictionary to translate the string in char to binary codes
	\param *text Original text to be encoded
	
	\return size+1 String size
	
*/
int get_string_size(char **dict, unsigned char *text){
	int i = 0;
	int size = 0;
	
	while (text[i] != '\0'){
		size = size + strlen(dict[text[i]]);
		i++;
	}
	
	return size + 1;
}

/*! \fn char* encode(char **dict, unsigned char *text)
    \brief	Function to encode the text received
	\param **dict Dictionary to translate the string in char to binary codes
	\param *text Original text to be encoded
	
	\return code Text encoded
*/
char* encode(char **dict, unsigned char *text){
	int i = 0;
	int size = get_string_size(dict, text);
	
	char *code = (char *)calloc(size, sizeof(char));
	
	while (text[i] != '\0'){
		strcat(code, dict[text[i]]);
		i++;
	}
	
	return code;
}


/************************************************************************/
/* COMPACT                                                              */
/************************************************************************/
/*! \fn char* encode(char **dict, unsigned char *text)
    \brief	Function to convert binary strings into binary values
	\param **dict Dictionary to translate the string in char to binary codes
	
	\return bbyte Byte converted
*/
char str2bin(char *dict){
	char mmask, bbyte = 0;
	int j = strlen(dict);
	
	for (int i = 0; i < strlen(dict); i++){
		mmask = 1;
		if (dict[i] == '1'){
			mmask = mmask << (j - 1);
			bbyte = bbyte | mmask;
		}
		j--;
	}

	return bbyte;
}

/*! \fn char* encode(char **dict, unsigned char *text)
    \brief	Function to calculate the size of the dictionary
	\param **dict Dictionary to translate the string in char to binary codes
	
	\return len Lenght of the dictionary
*/
char lendict(char **dict){
	char len = 0;
	
	for (int k = 0; k < NUM_CHAR; k++){
		if (*dict[k] != 0){
			len++;
		}
	}
	
	return len;
}

/* int compact(char str[], char **dict, char *huffman_name)
    Function to apply the Huffman Compression
	str[] Character to be converted to binary
	**dict Dictionary to translate the string in char to binary codes
	*huffman_name Name of the huffman file, alongisde the folder, following the pattern /datestamp/timestamp.huf
	
	CompSize Size of the compressed file
*/
int compact(char str[], char **dict, char *huffman_name){
	File compactHuf=SD.open(huffman_name, FILE_WRITE);
	
	int i = 0, j = 7;
	unsigned char mmask, bbyte = 0;
	int CompSize = 0;
	
	if(compactHuf){
		
		compactHuf.print(lendict(dict));
		CompSize ++;	
		
		for (int k = 0; k < NUM_CHAR; k++){
			if (*dict[k] != 0){
				compactHuf.write((char)k);
				compactHuf.print((char)(strlen(dict[k])));
				compactHuf.print(str2bin(dict[k]));
				CompSize += 3;		
			}
		}
		
		while (str[i] != '\0'){
			mmask = 1;
			if (str[i] == '1'){
				mmask = mmask << j;
				bbyte = bbyte | mmask;
			}
			j--;
			if (j < 0){
				compactHuf.write(bbyte);
				CompSize ++;
				bbyte = 0;
				j = 7;
			}
			i++;
		}
		if (j != 7){
			compactHuf.write(bbyte);
			CompSize ++;
		}
		compactHuf.close();
	} else {
		Serial.println("ERROR CREATING HUFFMAN FILE.");
	}
	
	return CompSize;
}


/*int compress(String* dataset, char *huffman_name)
    Public method that receives the JSON structure and aply all the procedures to perform the Huffman Compression
	*dataset JSON Structure to be converted
	*huffman_name Name of the huffman file, alongisde the folder, following the pattern /datestamp/timestamp.huf
	
	return compact(encoded,dict,huffman_name) 
*/


// PUBLIC METHODS
// Compression
void compress_file(char *dataset, char *filename)
{
	/************************************************************************/
	/* VARIABLES LIST AND USED TO COMPRESS STRING                           */
	/************************************************************************/

	uint16_t len_dataset = dataset->length();
	unsigned char *text = (unsigned char *)malloc(len_dataset + 1);
	memcpy(text, dataset->c_str(), len_dataset + 1);
	unsigned int frequency[NUM_CHAR];
	List list_huff;
	Node *tree;
	int columns;
	char **dict;
	char *encoded, *decoded;

	/************************************************************************/
	/* FILLING FREQUENCY TABLE                                              */
	/************************************************************************/

	init_frequency(frequency);
	fill_frequency(text, frequency);
	// print_frequency(frequency);

	/************************************************************************/
	/* ORDENATING LIST                                                      */
	/************************************************************************/

	create_list(&list_huff);
	fill_list(frequency, &list_huff);
	// print_list(&list_huff);

	/************************************************************************/
	/* HUFFMAN TREE                                                         */
	/************************************************************************/

	tree = create_tree(&list_huff);
	// print_tree(tree, 0);

	/************************************************************************/
	/* HUFFMAN DICTIONARY                                                   */
	/************************************************************************/

	columns = tree_height(tree) + 1;
	dict = dictionary_allocate(columns);
	create_dictionary(dict, tree, "", columns);
	// print_dictionary(dict);

	/************************************************************************/
	/* HUFFMAN ENCODING                                                     */
	/************************************************************************/
	encoded = encode(dict, text);
	// SERIALDEBUG.println("ENCODED TEXT: ");
	// SERIALDEBUG.println(encoded);

	/************************************************************************/
	/* HUFFMAN COMPACT                                                      */
	/************************************************************************/
	compact(encoded, dict, huffman_name);
	Serial.println("FILE COMPRESSED.");
}

void compress_string(char *dataset)
{
	/************************************************************************/
	/* VARIABLES LIST AND USED TO COMPRESS STRING                           */
	/************************************************************************/

	uint16_t len_dataset = dataset->length();
	unsigned char *text = (unsigned char *)malloc(len_dataset + 1);
	memcpy(text, dataset->c_str(), len_dataset + 1);
	unsigned int frequency[NUM_CHAR];
	List list_huff;
	Node *tree;
	int columns;
	char **dict;
	char *encoded, *decoded;

	/************************************************************************/
	/* FILLING FREQUENCY TABLE                                              */
	/************************************************************************/

	init_frequency(frequency);
	fill_frequency(text, frequency);
	// print_frequency(frequency);

	/************************************************************************/
	/* ORDENATING LIST                                                      */
	/************************************************************************/

	create_list(&list_huff);
	fill_list(frequency, &list_huff);
	// print_list(&list_huff);

	/************************************************************************/
	/* HUFFMAN TREE                                                         */
	/************************************************************************/

	tree = create_tree(&list_huff);
	// print_tree(tree, 0);

	/************************************************************************/
	/* HUFFMAN DICTIONARY                                                   */
	/************************************************************************/

	columns = tree_height(tree) + 1;
	dict = dictionary_allocate(columns);
	create_dictionary(dict, tree, "", columns);
	// print_dictionary(dict);

	/************************************************************************/
	/* HUFFMAN ENCODING                                                     */
	/************************************************************************/
	encoded = encode(dict, text);
	// SERIALDEBUG.println("ENCODED TEXT: ");
	// SERIALDEBUG.println(encoded);
	return (dict);

}
// Decompression
void decompreess_file(){

}

void decompress_string(){

}


