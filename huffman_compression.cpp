/*
 * ArduinoHuffman
 * Authors: Rafael Perez
 */

#include "huffman_compression.h"

constexpr int NUM_CHAR = 256; // 256 possible characters



// Function to initialize the frequency table
void init_frequency(std::array<unsigned int, NUM_CHAR> &frequency)
{
	frequency.fill(0); // Modern C++ std::array with fill
}

// Function to fill the frequency table based on the input text
void fill_frequency(const std::string &text, std::array<unsigned int, NUM_CHAR> &frequency)
{
	for (unsigned char ch : text)
	{
		frequency[ch]++;
	}
}

// Function to print the frequency table (for debugging purposes)
void print_frequency(const std::array<unsigned int, NUM_CHAR> &frequency)
{
	std::cout << "Frequency Table:\n";
	for (int i = 0; i < NUM_CHAR; ++i)
	{
		if (frequency[i] > 0)
		{
			std::cout << i << " ('" << static_cast<char>(i) << "') : " << frequency[i] << "\n";
		}
	}
}

// Build Huffman Tree based on frequency table
std::shared_ptr<Node> build_huffman_tree(const std::array<unsigned int, NUM_CHAR> &frequency)
{
	std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Node::Compare> pq;

	// Insert all non-zero frequency characters into the priority queue
	for (int i = 0; i < NUM_CHAR; ++i)
	{
		if (frequency[i] > 0)
		{
			pq.push(std::make_shared<Node>(i, frequency[i]));
		}
	}

	while (pq.size() > 1)
	{
		auto left = pq.top();
		pq.pop();
		auto right = pq.top();
		pq.pop();
		auto newNode = std::make_shared<Node>('+', left->frequency + right->frequency);
		newNode->left = left;
		newNode->right = right;
		pq.push(newNode);
	}

	return pq.top(); // Root of the tree
}

// Recursive function to generate the Huffman dictionary from the tree
void generate_dictionary(const std::shared_ptr<Node> &root, std::string code, std::array<std::string, NUM_CHAR> &dict)
{
	if (!root->left && !root->right)
	{ // Leaf node
		dict[root->character] = code;
		return;
	}

	if (root->left)
		generate_dictionary(root->left, code + '0', dict);
	if (root->right)
		generate_dictionary(root->right, code + '1', dict);
}

// Function to print the generated dictionary (for debugging)
void print_dictionary(const std::array<std::string, NUM_CHAR> &dict)
{
	std::cout << "Huffman Dictionary:\n";
	for (int i = 0; i < NUM_CHAR; ++i)
	{
		if (!dict[i].empty())
		{
			std::cout << i << " ('" << static_cast<char>(i) << "') : " << dict[i] << "\n";
		}
	}
}

// Function to encode the input text using the Huffman dictionary
std::string encode_text(const std::string &text, const std::array<std::string, NUM_CHAR> &dict)
{
	std::string encoded;
	for (unsigned char ch : text)
	{
		encoded += dict[ch];
	}
	return encoded;
}

// Function to write the Huffman dictionary and encoded data into a binary file
void write_compressed_file(const std::string &huffman_name, const std::array<std::string, NUM_CHAR> &dict, const std::string &encoded_text)
{
	std::ofstream outfile(huffman_name, std::ios::binary);

	if (!outfile.is_open())
	{
		std::cerr << "ERROR CREATING HUFFMAN FILE.\n";
		return;
	}

	// Write dictionary
	char dict_size = static_cast<char>(std::count_if(dict.begin(), dict.end(), [](const std::string &s)
													 { return !s.empty(); }));
	outfile.write(&dict_size, sizeof(char));

	for (int i = 0; i < NUM_CHAR; ++i)
	{
		if (!dict[i].empty())
		{
			char character = static_cast<char>(i);
			char length = static_cast<char>(dict[i].length());
			outfile.write(&character, sizeof(char));
			outfile.write(&length, sizeof(char));
			outfile.write(dict[i].c_str(), length); // Write binary representation
		}
	}

	// Write encoded text as binary data
	char buffer = 0;
	int bit_pos = 0;

	for (char bit : encoded_text)
	{
		if (bit == '1')
		{
			buffer |= (1 << (7 - bit_pos));
		}
		bit_pos++;
		if (bit_pos == 8)
		{
			outfile.write(&buffer, sizeof(char));
			buffer = 0;
			bit_pos = 0;
		}
	}

	// Write any remaining bits
	if (bit_pos > 0)
	{
		outfile.write(&buffer, sizeof(char));
	}

	outfile.close();
	std::cout << "File compressed successfully.\n";
}

// Main compression function
void compress_file(const std::string &dataset, const std::string &huffman_name)
{
	// Initialize frequency table
	std::array<unsigned int, NUM_CHAR> frequency;
	init_frequency(frequency);

	// Fill frequency table based on dataset
	fill_frequency(dataset, frequency);

	// Build Huffman Tree
	std::shared_ptr<Node> huffman_tree = build_huffman_tree(frequency);

	// Generate Huffman dictionary
	std::array<std::string, NUM_CHAR> dict;
	generate_dictionary(huffman_tree, "", dict);

	// Encode the input text
	std::string encoded_text = encode_text(dataset, dict);

	// Write compressed file
	write_compressed_file(huffman_name, dict, encoded_text);
}


// Function to read the Huffman dictionary from the file
void read_huffman_dictionary(std::ifstream &infile, std::shared_ptr<Node> &root)
{
	char dict_size;
	infile.read(&dict_size, sizeof(char)); // Read the size of the dictionary

	// For each character and its corresponding Huffman code, reconstruct the tree
	for (int i = 0; i < dict_size; ++i)
	{
		char character;
		char code_length;
		infile.read(&character, sizeof(char));
		infile.read(&code_length, sizeof(char));

		std::string code;
		code.resize(code_length);
		infile.read(&code[0], code_length);

		// Insert the character into the Huffman tree based on its code
		std::shared_ptr<Node> current = root;
		for (char bit : code)
		{
			if (bit == '0')
			{
				if (!current->left)
				{
					current->left = std::make_shared<Node>('+', 0);
				}
				current = current->left;
			}
			else
			{
				if (!current->right)
				{
					current->right = std::make_shared<Node>('+', 0);
				}
				current = current->right;
			}
		}
		current->character = static_cast<unsigned char>(character); // Assign the character to the leaf node
	}
}

// Function to decode the binary data using the Huffman tree
std::string decode_data(std::ifstream &infile, std::shared_ptr<Node> &root, int encoded_length)
{
	std::string decoded_text;
	std::shared_ptr<Node> current = root;

	char buffer;
	int bit_count = 0;
	while (infile.read(&buffer, sizeof(char)))
	{
		for (int i = 7; i >= 0; --i)
		{
			bool bit = (buffer & (1 << i)) != 0;

			// Traverse the Huffman tree based on the bit
			if (bit)
			{
				current = current->right;
			}
			else
			{
				current = current->left;
			}

			// If we reach a leaf node, we append the character to the decoded text
			if (!current->left && !current->right)
			{
				decoded_text += current->character;
				current = root; // Go back to the root for the next character
			}
			bit_count++;
			if (bit_count >= encoded_length)
				break; // Stop when we decode the full length of encoded data
		}
	}

	return decoded_text;
}

// Decompression function
void decompress_file(const std::string &huffman_filename, const std::string &output_filename)
{
	std::ifstream infile(huffman_filename, std::ios::binary);
	if (!infile.is_open())
	{
		std::cerr << "ERROR OPENING HUFFMAN FILE.\n";
		return;
	}

	// Reconstruct the Huffman tree from the file
	std::shared_ptr<Node> root = std::make_shared<Node>('+', 0);
	read_huffman_dictionary(infile, root);

	// Get the length of the encoded data (optional if not explicitly stored)
	infile.seekg(0, std::ios::end);
	std::streampos file_size = infile.tellg();
	infile.seekg(-1, std::ios::cur); // Assume we are reading the last byte for length of encoded data
	int encoded_length = static_cast<int>(file_size);

	// Decode the binary data using the Huffman tree
	std::string decoded_text = decode_data(infile, root, encoded_length);

	// Write the decompressed data to the output file
	std::ofstream outfile(output_filename);
	if (!outfile.is_open())
	{
		std::cerr << "ERROR CREATING OUTPUT FILE.\n";
		return;
	}

	outfile << decoded_text;
	outfile.close();
	infile.close();

	std::cout << "File decompressed successfully.\n";
}
